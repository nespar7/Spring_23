// Include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

// Store the queries in a global array 
const char *queries[] = {
    "select distinct ph.name from physician as ph join trained_in as tr on tr.physician = ph.employeeid join `procedure` as pr on pr.code = tr.treatment where pr.name = 'bypass surgery';",
    "select distinct ph.name from physician as ph join affiliated_with as aw on (aw.physician, aw.department) = ( ph.employeeid, ( select departmentid from department where name = 'cardiology' ) ) join trained_in as tr on tr.physician = ph.employeeid join `procedure` as pr on pr.code = tr.treatment where pr.name = 'bypass surgery';",
    "select distinct name from nurse as n join room as r on r.number = 123 join on_call as oc on (oc.blockfloor, oc.blockcode) = (r.blockfloor, r.blockcode) where oc.nurse = n.employeeid;",
    "select pt.name, pt.address from patient as pt join medication as mc on mc.name = 'remdesivir' join prescribes as pc on pc.medication = mc.code where pt.ssn = pc.patient;",
    "select distinct pt.name, pt.insuranceid from patient as pt join room as r on r.type = 'icu' join stay as s on s.patient = pt.ssn where timestampdiff(DAY, s.start, s.end) >= 15;",
    "select distinct n.name as nurse_name from nurse as n join `procedure` as pr on pr.name = 'bypass surgery' join undergoes as u on u.`procedure` = pr.code where u.assistingnurse = n.employeeid;",
    "select distinct n.name as assisting_nurse, n.position as nurse_position, ( select name from patient as pt where u.patient = pt.ssn ) as patient_name, ( select name from physician as ph where u.physician = ph.employeeid ) as physician_name from nurse as n join `procedure` as pr on pr.name = 'bypass surgery' join undergoes as u on u.`procedure` = pr.code where n.employeeid = u.assistingnurse;",
    "select distinct ( select  physician.name from  physician where  physician.employeeid = undergoes.physician  ) as physician from undergoes where undergoes.`procedure` not in ( select  treatment from trained_in where trained_in.physician = undergoes.physician and undergoes.date >= trained_in.certificationdate );",
    "select distinct ph.name from physician as ph join trained_in as tr on tr.physician = ph.employeeid join undergoes as u on u.physician = ph.employeeid where tr.treatment = u.`procedure` and u.date > tr.certificationexpires;",
    "select distinct ph.name as physician, ( select name from `procedure` as pr where pr.code = u.`procedure` ) as `procedure`, u.date, ( select name from patient as pt where pt.ssn = u.patient ) as patient from physician as ph join trained_in as tr on tr.physician = ph.employeeid join undergoes as u on u.physician = ph.employeeid where tr.treatment = u.`procedure` and u.date > tr.certificationexpires;",
    "with temp as ( select pt.name as patient, pt.pcp as physician, ( select count(*) from appointment as ap where ap.patient = pt.ssn and ap.physician in ( select ph.employeeid from physician as ph join affiliated_with as aw on aw.physician = ph.employeeid join department as dp on dp.departmentid = aw.department where dp.name = 'cardiology' ) ) as appointmentswithcardiologist, ( select count(*) from prescribes as pr where pr.patient = pt.ssn and pr.physician = pt.pcp ) as physicianprescribed, ( select count(*) from undergoes as u join `procedure` as pr on pr.code = u.`procedure` where u.patient = pt.ssn and pr.cost > 5000 ) as costatleastfivek from patient as pt ) select distinct patient, ( select physician.name from physician where physician.employeeid = temp.physician ) as physician from temp where appointmentswithcardiologist >= 2 and costatleastfivek >= 1 and physicianprescribed >= 1 and physician not in ( select head from department );",
    "with medicinepatient as ( select distinct mc.name as medicine, mc.brand as brand, pc.patient as patient from medication as mc join prescribes as pc on pc.medication = mc.code ), countprescribed as ( select medicine, brand, count(*) as times_prescribed from medicinepatient group by medicine, brand ), maxprescribed as ( select max(times_prescribed) as max_pres from countprescribed ) select medicine, brand from countprescribed, maxprescribed where countprescribed.times_prescribed = maxprescribed.max_pres;",
    // I cut this query in the middle so that I can concatenate the procedure later
    "select distinct ph.name from physician as ph join trained_in as tr on tr.physician = ph.employeeid join `procedure` as pr on pr.code = tr.treatment where pr.name = '"
};

// Function to print the delimiter in the +----+---+ format
void print_delim(int columns, int *field_lengths){
    printf("+");
    for(int i = 0;i < columns;i++){
        int len = field_lengths[i]+2;
        for(int j = 0;j < len;j++){
            printf("-");
        }
        printf("+");
    }
    printf("\n");   
}

// Main function
int main(){
    // Initialise connection, result and row objects
    MYSQL *connection;
    MYSQL_RES *result;
    MYSQL_ROW row;

    // Initialise details of database
    char *server = "10.5.18.69";
    char *username = "20CS10038";
    char *password = "20CS10038";
    char *database = "20CS10038";
    int port = 3306;

    // Initialise new connection object
    connection = mysql_init(NULL);

    // Establish connection with the database (blocks until connected or error occurs)
    if(!mysql_real_connect(connection, server, username, password, database, port, NULL, 0)){
        perror("cannot connect to mysql");
        exit(1);
    }

    // Infinite loop that exits when user enters 0
    while(1){
        int query_num;
        int columns;

        // Take query number input
        printf("Select a query to run(1-13), 0 to exit: ");
        scanf("%d", &query_num);
        // Exit on 0
        if(query_num == 0){
            printf("Bye have a nice day ;))\n");
            break;
        }
        // Print invalid on invalid query
        else if(query_num < 0 || query_num > 13){
            printf("Invalid query number entered\n");
            continue;
        }
        // For valid queries
        else{
            // If first 12 queries, just run queries[index]
            if(query_num != 13){
                if(mysql_query(connection, queries[query_num-1])){
                    perror("Could not run query: ");
                    exit(1);
                }
            }
            // If 13th query is requested
            else{
                // Assuming procedure name would not be longer than 40
                char procedure[40];
                char *formatted_query;

                // Taking procedure name as input
                printf("Enter procedure name: ");
                getchar();
                fgets(procedure, 40, stdin);

                // Allocating memory for the formatted query
                int len = strlen(procedure) + strlen(queries[12]) + 3;
                formatted_query = (char *)malloc(len*sizeof(char));
                if(formatted_query == NULL){
                    printf("malloc error\n");
                    exit(1);
                }

                // First copy queries[12] into fq, then concatenate the procedure
                // And add `';` at the end
                strcpy(formatted_query, queries[12]);
                strcat(formatted_query, procedure);
                formatted_query[strlen(formatted_query)-1] = '\0';
                strcat(formatted_query, "';");

                // Run the query
                if(mysql_query(connection, formatted_query)){
                    perror("Could not run query: ");
                    exit(1);
                }

                // Free the space allocated
                free(formatted_query);
            }

            // Store the result of the execution in result
            result = mysql_store_result(connection);

            // columns stores the count of columns in the result
            // fields stores the information about the fields like name, etc.
            columns = mysql_num_fields(result);
            MYSQL_FIELD *fields = mysql_fetch_fields(result);
            
            // rows stores the number of rows in the result
            int rows = mysql_num_rows(result);

            // If no rows returned print Empty Set and return
            if(rows == 0){
                printf("\nEmpty Set\n\n");
                continue;
            }

            /* 
                The next part is for the formatted table printing
                field_lengths stores the maximum size of value in that column
             */
            int *field_lengths = (int *)malloc(columns * sizeof(int));
            memset(field_lengths, 0, columns*sizeof(int));

            // Updating maximum field length for the columns
            for(int i = 0;i < columns;i++){
                int len = strlen(fields[i].name);
                if(len > field_lengths[i]) field_lengths[i] = len;
            }

            while((row = mysql_fetch_row(result)) != NULL){
                for(int i = 0;i < columns;i++){
                    int len = strlen(row[i]);
                    if(len > field_lengths[i]) field_lengths[i] = len;
                }
            }

            // Print starting delimiter line
            print_delim(columns, field_lengths);

            // Print the column names in a formatted manner
            printf("|");
            for(int i = 0;i < columns;i++){
                printf(" %-*s |", field_lengths[i], fields[i].name);
            }
            printf("\n");

            // Print delimiter line
            print_delim(columns, field_lengths);

            // Format and print each row
            mysql_data_seek(result, 0);
            while((row = mysql_fetch_row(result))){
                printf("|");
                for(int i = 0;i < columns;i++){
                    printf(" %-*s |", field_lengths[i], row[i]);
                }
                printf("\n");
            }

            // Print ending delimiter line
            print_delim(columns, field_lengths);

            // Free allocated space for field_lengths
            free(field_lengths);
        }
        // Free result after each query execution
        mysql_free_result(result);
    }

    // Close connection
    mysql_close(connection);
    return 0;
}
