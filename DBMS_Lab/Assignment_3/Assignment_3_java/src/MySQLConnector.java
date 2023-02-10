import java.sql.*;
import java.util.HashMap;
import java.util.Scanner;
import javax.swing.*;
import javax.swing.table.DefaultTableModel;

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

                String query = MySQLQueries.get(key);

                if(query == null){
                    System.out.println("Invalid query number");
                    continue;
                }

                Statement statement = connection.createStatement();

                ResultSet results = statement.executeQuery(query);

                ResultSetMetaData md = results.getMetaData();
                int cc = md.getColumnCount();

                String[] columns = new String[cc];

                for(int i = 0;i < cc;i++){
                    columns[i] = md.getColumnName(i+1);
                }

                DefaultTableModel model = new DefaultTableModel(columns, 0);

                while (results.next()){
                    String[] row = new String[cc];
                    for(int i = 0;i < cc;i++){
                        row[i] = results.getString(i+1);
                    }
                    model.addRow(row);
                }

                JTable table = new JTable(model);

                JScrollPane scrollPane = new JScrollPane(table);

                JFrame frame = new JFrame();
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frame.add(scrollPane);
                frame.pack();
                frame.setVisible(true);
            }

            connection.close();
        }
        catch (Exception err){
            err.printStackTrace();
        }
    }
}
