import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

public class MySQLConnector {
    private String url;
    private String username;
    private String password;
    private HashMap<java.lang.Integer, String> MySQLQueries;

    public MySQLConnector(String url, String username, String password){
        this.url = url;
        this.username = username;
        this.password = password;
        this.MySQLQueries = new HashMap<java.lang.Integer, String>();
    }

    public void addQuery(java.lang.Integer key, String query){
        this.MySQLQueries.put(key, query);
    }

    public void connect(){
        try{
            Class.forName("com.mysql.cj.jdbc.Driver");

            Connection connection = DriverManager.getConnection(url, username, password);

            Scanner scan = new Scanner(System.in);

            while(true){
                System.out.println("Enter query number(1-13, 0 for exit): ");
                java.lang.Integer key = scan.nextInt();

                if(key == 0) break;

                String query;

                if(key == 13){
                    query = MySQLQueries.get(key);
                }
                else {
                    System.out.println("Enter procedure ");
                    query = String.format(MySQLQueries.get(key), )
                }

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
