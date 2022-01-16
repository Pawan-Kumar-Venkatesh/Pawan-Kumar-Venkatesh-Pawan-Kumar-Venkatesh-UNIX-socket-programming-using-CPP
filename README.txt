Name: Pawan Kumar Venkatesh

What have I done in this assignment?:
This project is an implementation of client server socket programming which involves accepting two usernames from different clients, computing the paths between them and selecting the smallest matching gap based on the compatibility scores.

I have completed all the four phases stated in the project description.
In Phase 1,
	1. All the servers boot up.
	2. Once the servers are booted up, the two clients will run and accept the usernames as command line arguments.
	3. The clients then establish TCP connections and send the usernames to the central server.
	4. The central server then establishes UDP connections with the backend servers.

In Phase 2,
	1. The central server will contact server T and server S for the path and relevant score information.
	2. Once the central server receives these, it will then send them to server P for finding the matching gap and inturn finding the shortest path.

In Phase 3,
	1. After processing the data received from central server, server P will send these results back to the central server.
	2. This information is then forwarded to the clients which is to be displayed as on-screen messages.

In Phase 4,
	Client B will send two usernames and the servers will compute the compatibility scores of each of these usernames with the username from client A.


Implemented the optional part?: YES

What do the code files do?:

	clientA.cpp & clientB.cpp
	Code for communicating with the Central server over a TCP connection. The ports here are assigned dynamically and it establishes a socket connection with the Central server's previously known port number.
	Here, the clients send the username information to the central server and wait for the shortest path response.
	In addition, the code in client B also supports two usernames to be entered in the command line argument(optional section).

	central.cpp
	Code for the central server which does the following:
	1. Receives the usernames as inputs from both the clients. 
	2. Forwards the received inputs to the Topology server to get the paths between Client A and Client B.
	3. Once it receives the topology information, it will request for the scores of the relevant nodes from the score server.
	4. Once it receives the scores, it will then send both this score information as well as the path information to the processing server where the shortest path will be computed.
	7. This shortest path is then sent back to the clients.

	serverT.cpp
	Code for calculating the paths from Client A to client B.
	The code does the following:
	1. Reads the edgelist.txt and stores it into a structure.
	2. Uses DFS to calculate multiple paths between clientA and client B.
	3. Stores these reduced paths in a file called reduced_edgelist.txt.
	4. This file is then sent back to the central server for further processing.

	serverS.cpp
	Code for returning the scores of the requested nodes to the central server.

	serverP.cpp
	Code for finding the shortest path.
	The code does the following.
	1. Receives the nodes, scores and path information from the central server.
	2. It then calculates the matching gap between each of the two neighbors and adds them altogether to find the compatibility score(weight).
	3. The server then computes the smallest among these weights to find the shortest path.
	4. This path is then sent back to the central server to be relayed to the clients.
	
Format of the messages exchanged:
	In general, the messages are exchanged as character arrays. These are then either stored in a structure or in a text file.
	
	Details:
	
		Clients A&B:
			Client A and B sends the username to central server as a character array.
		
		Central:
			1. Receives the usernames from the clients as a character array.
			2. Forwards the received usernames as a character array to Server T.
			3. Receives the paths from Server T as a character array one path at a time and stores it into a file.
			4. Requests for the scores from the Server S by sending the relevant nodes(names) as a character array, one node at a time.
			5. Receives the scores from Server S as a character array, one score at a time.
			6. Sends the paths received from server T to server P as a character array, one path at a time.
			7. Sends the relevant nodes and its scores received from server S to server P as a character array, one node at a time.
			8. Receives the Shortest path and its corresponding compatibility score(weight) as a character array.
		
		Server T:
			1. Receives the usernames from the central server as a character array.
			2. Sends the paths to the central server as a character array, one path at a time.
		
		Server S:
			1. Receives the relevant nodes(names) from the central server as a character array, one node at a time.
			2. Sends the relevant scores back to the central server as a character array, one score at a time.
		
		Server P:
			1. Receives the paths from the central server as a character array, one path at a time.
			2. Receives the relevant nodes and its scores from the central server as a character array, one node at a time.
			3. Sends the shortest path to the central server as a character array.
			4. Sends the smallest compatibility score(weight) to the central server as a character array.

Idiosyncrasies:
Below are the buffer sizes allocated in the code. Beyond this buffer capacity, the code might not work.
	1. Character array for usernames - 512
	2. String array for storing the nodes in the final shortest path - 512
	3. String array for storing all the paths between two clients - 4096
	4. String array for storing the individual nodes(names) in the reduced graph - 65536
	5. Structure for storing distinct nodes and their corresponding scores - 512
	6. Structure for storing the edgelist - 256
	7. Structure for storing the scores - 512
	8. Structure for storing the paths and the corresponding weights - 4096	

Reused code and References:
	1. Referred Beej's Socket programming guide and some youtube videos for implementing socket connections in my code.
	2. Reused the DFS code by Geek-for-Geeks with few modifications.
	   URL: https://www.geeksforgeeks.org/find-paths-given-source-destination/?ref=lbp

Sample output of my code with single input at Client A and Client B --->
	
	Central:
		The Central server is up and running.
		The Central server received input="Noah" from the client using TCP over port 25998.
		The Central server received input="Rachit" and "" from the client using TCP over port 26998.
		The Central server sent a request to Backend-Server T.
		The Central server received information from Backend-Server T using UDP over port 21998.
		The Central server sent a request to Backend-Server S.
		The Central server received information from Backend-Server S using UDP over port 22998.
		The Central server sent a processing request to Backend-Server P.
		The Central server received the results from backend server P.
		The Central server sent the results to client A.
		The Central server sent the results to client B.
	
	Server T:
		The ServerT is up and running using UDP on port 21998.
		The ServerT received a request from Central to get the topology.
		The ServerT finished sending the topology to Central.
	
	Server S:
		The ServerS is up and running using UDP on port 22998.
		The ServerS received a request from Central to get the scores.
		The ServerS finished sending the scores to Central.
	
	Server P:
		The ServerP is up and running using UDP on port 23998.
		The ServerP received the topology and score information.
		The ServerP finished sending the results to the Central.
	
	Client A:
		The client is up and running.

		The client sent Noah to the Central server.
		
		Found compatibility for Noah and Rachit:
		Noah --- Rachael --- Rachit
		Compatibility score: 0.808
	
	Client B:
		The client is up and running.

		The client sent Rachit to the Central server.

		Found compatibility for Rachit and Noah:
		Rachit --- Rachael --- Noah
		Compatibility score: 0.808
	

Sample output of my code with single input at Client A and two inputs at Client B --->
	
	Central:
		The Central server is up and running.
		The Central server received input="Noah" from the client using TCP over port 25998.
		The Central server received input="Rachit" and "Rachael" from the client using TCP over port 26998.
		The Central server sent a request to Backend-Server T.
		The Central server received information from Backend-Server T using UDP over port 21998.
		The Central server sent a request to Backend-Server S.
		The Central server received information from Backend-Server S using UDP over port 22998.
		The Central server sent a processing request to Backend-Server P.
		The Central server received the results from backend server P.
		The Central server sent the results to client A.
		The Central server sent the results to client B.

		The Central server sent a request to Backend-Server T.
		The Central server received information from Backend-Server T using UDP over port 21998.
		The Central server sent a request to Backend-Server S.
		The Central server received information from Backend-Server S using UDP over port 22998.
		The Central server sent a processing request to Backend-Server P.
		The Central server received the results from backend server P.
		The Central server sent the results to client A.
		The Central server sent the results to client B.
	
	Server T:
		The ServerT is up and running using UDP on port 21998.
		The ServerT received a request from Central to get the topology.
		The ServerT finished sending the topology to Central.

		The ServerT received a request from Central to get the topology.
		The ServerT finished sending the topology to Central.
		
	Server S:
		The ServerS is up and running using UDP on port 22998.
		The ServerS received a request from Central to get the scores.
		The ServerS finished sending the scores to Central.

		The ServerS received a request from Central to get the scores.
		The ServerS finished sending the scores to Central.
	
	Server P:
		The ServerP is up and running using UDP on port 23998.
		The ServerP received the topology and score information.
		The ServerP finished sending the results to the Central.

		The ServerP received the topology and score information.
		The ServerP finished sending the results to the Central.
	
	Client A: 
		The client is up and running.

		The client sent Noah to the Central server.

		Found compatibility for Noah and Rachit:
		Noah --- Rachael --- Rachit
		Compatibility score: 0.808

		Found compatibility for Noah and Rachael:
		Noah --- Rachael
		Compatibility score: 0.34
	
	Client B:
		The client is up and running.

		The client sent Rachit & Rachael to the Central server.

		Found compatibility for Rachit and Noah:
		Rachit --- Rachael --- Noah
		Compatibility score: 0.808

		Found compatibility for Rachael and Noah:
		Rachael --- Noah
		Compatibility score: 0.34
