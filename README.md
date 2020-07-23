# Client-Server-using-AF_UNIX-socket-in-c
Creation in C of a client program and a server program. The two processes interact using AF_UNIX sockets.

SERVER:
- manages multiple connections at a time via threads
- for each connection launch a new thread that will manage the connection with the client
- the reply message is a string equal to the original message but each character has inverted the "letter case", i.e., uppercase characters become lowercase and vice versa
- Example: string received "hello" -> string returned "hello"
- The server checks if the string contains "white-space" characters ('', \ f, \ t, \ r, \ t, \ v). In case they are found, it does NOT convert and returns an error to the client

CLIENT:
- opens a connection to the server and sends a string
- waits for the result before sending a new string
- ends when it receives as input (from user) string "quit"

Implementation of signal management in the server following the reception of one of the following signals:
- SIGINT, SIGQUIT, SIGTERM, SIGHUP
- If one of these signals is received, the server must terminate leaving the consistent state
- Use a dedicated signal management thread


Instructions for execution:
- download all the files from the repository
-open a terminal in the folder containing the files
- use the "make" terminal command to compile all the files and obtain an executable for the server and one for the client
- launch the server with "./server"
- subsequently launch (in a new terminal for each client) one or more clients with the con "./client"
- the connection can be closed with the "quit" command from the client as seen above
- to clean up the compiled files use "make clean"
