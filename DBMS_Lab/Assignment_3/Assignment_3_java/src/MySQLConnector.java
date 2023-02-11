// Imports
import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

// The MySQLConnector class to create connection, add queries and execute queries
public class MySQLConnector {
    // declaring fields url, username, password
    // MySQLQueries stores the queries as a key value map
    private String url;
    private String username;
    private String password;
    private HashMap<java.lang.Integer, String> MySQLQueries;

    // Constructor
    public MySQLConnector(String url, String username, String password){
        this.url = url;
        this.username = username;
        this.password = password;
        this.MySQLQueries = new HashMap<java.lang.Integer, String>();
    }

    // This function adds queries to the MySQLQueries map
    public void addQuery(java.lang.Integer key, String query){
        this.MySQLQueries.put(key, query);
    }

    // connect function to connect to database and execute queries
    public void connect(){
        try{
            // create a connection object that connects to the url using username, password
            Connection connection = DriverManager.getConnection(url, username, password);

            // Scanner to take user input
            Scanner scan = new Scanner(System.in);

            // Infinite loop exits on 0
            while(true){
                // Take query number as input
                System.out.print("Select a query to run(1-13), 0 for exit: ");
                java.lang.Integer key = scan.nextInt();

                // If zero, exit
                if(key == 0) {
                    System.out.println("Bye have a nice day ;))");
                    break;
                }

                // String to store query
                String query;

                // If key is not 13 just load the query from the map
                if(key != 13){
                    query = MySQLQueries.get(key);
                }
                // If key is 13
                else {
                    // Take procedure as user input
                    Scanner scanner = new Scanner(System.in);
                    System.out.print("Enter procedure: ");
                    String procedure = scanner.nextLine();

                    // replace the %s in 13th query with procedure
                    query = String.format(MySQLQueries.get(key), procedure);
                }

                // If query is null (out of bound accessing the map)
                // print invalid query message and go to start of loop
                if(query == null){
                    System.out.println("Invalid query number entered");
                    continue;
                }

                // create a Statement object for executing the query
                Statement statement = connection.createStatement();

                // Store the results of execution in results
                ResultSet results = statement.executeQuery(query);

                // md contains data about results, column_count is number of columns
                ResultSetMetaData md = results.getMetaData();
                int column_count = md.getColumnCount();

                // Store rows in a list of string array(each row contains one entry per column)
                List<String[]> rows = new ArrayList<>();
                // Column names stored as an array of Strings
                String[] columns = new String[column_count];

                // Store column names from the metadata
                for(int i = 0;i < column_count;i++){
                    columns[i] = md.getColumnName(i+1);
                }

                // Add the column names to the rows List
                rows.add(columns);

                // While there is a result
                while (results.next()){
                    // Store the values for each column in a String array row
                    String[] row = new String[column_count];
                    for(int i = 0;i < column_count;i++){
                        row[i] = results.getString(i+1);
                    }
                    // Add row to rows
                    rows.add(row);
                }

                // If there is only one row(the column names), exit
                if(rows.size() == 1) {
                    System.out.println();
                    System.out.println("Empty Set");
                    System.out.println();
                }
                // else print the formatted table
                else{
                    // maxColumnSize stores the maximum length of a field for each column
                    int[] maxColumnSize = new int[column_count];
                    for (String[] row: rows){
                        // increment maxColumnSize of that column if the current field is
                        // longer than the previous maximum
                        for(int i = 0;i < column_count;i++){
                            maxColumnSize[i] = Math.max(maxColumnSize[i], row[i].length());
                        }
                    }

                    // String to print formatted rows
                    String rowFormatString = "|";
                    for(int size: maxColumnSize){
                        rowFormatString += " %-" + (size) + "s |";
                    }

                    // The delimiter string
                    String delim = "+";
                    for(int size: maxColumnSize){
                        // print (size+2) whitespaces and replace then with "-"
                        delim += String.format("%-" + (size+2) + "s+", "-").replace(" ", "-");
                    }

                    // Print starting delimiter
                    System.out.println(delim);
                    int columns_printing = 1;
                    // Print each row
                    for(String[] row: rows){
                        // Print formatted row
                        System.out.format(rowFormatString, (Object[]) row);
                        System.out.println();
                        // Print delimiter after columns
                        if(columns_printing == 1){
                            System.out.println(delim);
                            columns_printing = 0;
                        }
                    }
                    // Print ending delimiter
                    System.out.println(delim);
                }
            }

            // Close the connection
            connection.close();
        }
        // If error trace it back
        catch (Exception err){
            err.printStackTrace();
        }
    }
}
