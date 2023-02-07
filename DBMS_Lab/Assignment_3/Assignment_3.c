#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <libssh/libssh.h>
// #include <my_global>
#include <mysql/mysql.h>

#define BUFFSIZE 500

int main(){
    ssh_session session;
    int response;
    MYSQL *connection;
    MYSQL_RES *result;
    MYSQL_ROW row;

    session = ssh_new();
    ssh_options_set(session, SSH_OPTIONS_HOST, "10.5.18.69");
    ssh_options_set(session, SSH_OPTIONS_USER, "20CS10038");
    ssh_options_set(session, SSH_OPTIONS_PORT, 3306);
    response = ssh_connect(session);
    if(response != SSH_OK){
        perror("Cannot connect to ssh server");
        ssh_free(session);
        exit(-1);
    }
    response = ssh_userauth_password(session, NULL, "Surya_7777");
    if(response != SSH_AUTH_SUCCESS){
        perror("Invalid passwprd");
        ssh_disconnect(session);
        ssh_free(session);
        exit(-1);
    }

    connection = mysql_init(NULL);
    if(mysql_real_connect(connection, "localhost", "20CS10038", "20CS10038", "20CS10038", 0, NULL, 0)){
        perror("Could not connect to mysql server");
        mysql_close(connection);
        ssh_disconnect(session);
        ssh_free(session);
        exit(-1);
    }

    char buff[100];
    getcwd(buff, 100);
    printf("%s\n", buff);

    if(mysql_query(connection, "select * from physician")){
        perror("Error executing query");
        mysql_close(connection);
        ssh_disconnect(session);
        ssh_free(session);
        exit(-1);
    }
    result = mysql_use_result(connection);
    while((row = mysql_fetch_row(result)) != NULL){
        printf("%s\n", row[0]);
    }

    mysql_free_result(result);
    mysql_close(connection);
    ssh_disconnect(session);
    ssh_free(session);
    return 0;
}