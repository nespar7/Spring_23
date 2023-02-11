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
                System.out.println("Select a query to run(1-13), 0 for exit: ");
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
                    query = String.format(MySQLQueries.get(key), procedure.toLowerCase());
                }

                // If query is null (out of bound accessing the map) print invalid query message
                if(query == null){
                    System.out.println("Invalid query number");
                    continue;
                }

                Statement statement = connection.createStatement();

                ResultSet results = statement.executeQuery(query);

                ResultSetMetaData md = results.getMetaData();
                int cc = md.getColumnCount();

                List<String[]> rows = new ArrayList<>();
                String[] columns = new String[cc];

                for(int i = 0;i < cc;i++){
                    columns[i] = md.getColumnName(i+1);
                }
                rows.add(columns);

                while (results.next()){
                    String[] row = new String[cc];
                    for(int i = 0;i < cc;i++){
                        row[i] = results.getString(i+1);
                    }
                    rows.add(row);
                }

                int[] maxColumnSize = new int[cc];
                for (String[] row: rows){
                    for(int i = 0;i < cc;i++){
                        maxColumnSize[i] = Math.max(maxColumnSize[i], row[i].length());
                    }
                }

                String rowFormatString = "|";
                for(int size: maxColumnSize){
                    rowFormatString += " %-" + (size) + "s |";
                }

                String delim = "+";
                for(int size: maxColumnSize){
                    delim += String.format("%-" + (size+2) + "s+", "-").replace(" ", "-");
                }

                if(rows.size() == 1){
                    System.out.println("Empty Set");
                }
                else{
                    System.out.println(delim);
                    int i = 1;
                    for(String[] row: rows){
                        System.out.format(rowFormatString, (Object[]) row);
                        System.out.println();
                        if(i == 1){
                            System.out.println(delim);
                            i = 0;
                        }
                    }
                    System.out.println(delim);
                }
            }

            connection.close();
        }
        catch (Exception err){
            err.printStackTrace();
        }
    }
}
