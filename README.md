# Client-Server-AF_UNIX-socket
A basic ***client-server*** program written in c using `AF_UNIX` socket.

## Server:
- Manages multiple connections at a time via threads
- For each connection launch a new thread that will manage the connection with the client
- The reply message is a string equal to the original message but each character has inverted the "letter case", i.e., uppercase characters become lowercase and vice versa
    - *Example*: string received `hello` -> string returned `HELLO`
- The server checks if the string contains "white-space" characters (`'', \ f, \ t, \ r, \ t, \ v`). In case they are found, it does **NOT** convert and returns an error to the client.

## Client:
- Opens a connection to the server and sends a string
- Waits for the result before sending a new string
- Ends when it receives as input (from user) string `quit`

## Implementation info
Implementation of signal management in the server following the reception of one of the following signals:
- `SIGINT, SIGQUIT, SIGTERM, SIGHUP`
- If one of these signals is received, the server must terminate leaving the consistent state
- Use a dedicated signal management thread


### Instructions for execution:
- Download all the files from the repository
- Open a terminal in the folder containing the files
- Use the ```make``` terminal command to compile all the files and obtain an executable for the server and one for the client
- Launch the server with ```./server```
- Subsequently launch (in a new terminal for each client) one or more clients with the con ```./client```
- The connection can be closed with the `quit` command from the client as seen above
- To clean up the compiled files use ```make clean```
