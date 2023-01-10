# Question 1

### When the client connects, the server sends the local time on it and the client displays the time

## Server
Run the server with `gcc time_server.c -o time_server && ./time_server`

## Client
Run the client with `gcc time_client.c -o time_client && ./time_client`

# Question 2

### The client sends an algebraic expression as a string, it is evaluated on the server side and displayed on the client

## Server
Run the server with `gcc server_it.c -o server && ./server`

## Client
Run the client with `gcc client.c -o client && ./client`