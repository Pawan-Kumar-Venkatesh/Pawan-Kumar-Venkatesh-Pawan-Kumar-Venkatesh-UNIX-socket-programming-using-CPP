#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <bits/stdc++.h>
#include <sstream>

using namespace std;

#define LOCAL_HOST "127.0.0.1"


//Creating a structure of distinct nodes and their corresponding scores.

typedef struct nodes
{
	string node = "";
	string score = "";
}Nodes;

Nodes distinct_nodes[512];
string node_list[65536]; //For storing all the individual usernames in the reduced graph, including the repeated ones along multiple paths.
string paths[4096]; //For storing the paths received from the Topology server.
int n_nodes = 0; //For counting the nodes stored in the node_list.
int number_of_nodes = 0; //For counting the number of distinct nodes in the reduced graph.


unsigned int node_exist(string s, Nodes n[]) //Checking if the node is already in the list of distinct nodes or not.
{
	unsigned int exists =0;
	for(int i=0; i<number_of_nodes; i++)
		if(s == n[i].node)
			exists = 1;
	return exists;
}

void read_nodes() //For populating the distinct nodes in the distinct node structure. 
{   
    number_of_nodes=0;
	for(int i=0; i<n_nodes; i++)
	{
		if (!node_exist(node_list[i], distinct_nodes))
		{
			distinct_nodes[number_of_nodes].node = node_list[i];
			number_of_nodes++;
		}

	}
}

//Creating a structure of distinct nodes ends here.

int main()
{

    //Creating and Binding the TCP socket for client A

    int asock = socket(AF_INET, SOCK_STREAM, 0);
    if (asock ==-1)
        {
            cerr<<"Cant create a socket."<<endl;
            return -1;
        }
    
    sockaddr_in hinta;
    hinta.sin_family = AF_INET;
    hinta.sin_port = htons(25998);
    inet_pton(AF_INET, LOCAL_HOST, &hinta.sin_addr);

    if(bind(asock, (sockaddr*)&hinta, sizeof(hinta))==-1) 
    {
        cerr<<"Can't bind the addresses to the socket"<<endl;
        return -1;
    }    
    
    //Creating and Binding the TCP socket for client B

    int bsock = socket(AF_INET, SOCK_STREAM, 0);
    if (bsock ==-1)
    {
        cerr<<"Cant create a socket."<<endl;
        return -1;
    }
    
    sockaddr_in hintb;
    hintb.sin_family = AF_INET;
    hintb.sin_port = htons(26998);
    inet_pton(AF_INET, LOCAL_HOST, &hintb.sin_addr);

    if(bind(bsock, (sockaddr*)&hintb, sizeof(hintb))==-1) 
    {
        cerr<<"Can't bind the addresses to the socket"<<endl;
        return -1;
    }

    
    //Creating and Binding the UDP socket for Backend servers
    int backendsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (backendsock ==-1)
    {
        cerr<<"Cant create a socket."<<endl;
        return -1;
    }

    sockaddr_in hintbackend;
    memset(&hintbackend, 0, sizeof(hintbackend));
    hintbackend.sin_family = AF_INET;
    hintbackend.sin_port = htons(24998);
    inet_pton(AF_INET, LOCAL_HOST, &hintbackend.sin_addr);

    if(bind(backendsock, (sockaddr*)&hintbackend, sizeof(hintbackend))==-1) 
    {
        cerr<<"Can't bind the addresses to the socket"<<endl;
        return -1;
    }


    int portt = 21998;
    int ports = 22998;
    int portp = 23998;

    //server T port information.
    
    sockaddr_in hintt;
    memset(&hintt, 0, sizeof(hintt));
    hintt.sin_family = AF_INET;
    hintt.sin_port = htons(portt);
    inet_pton(AF_INET, LOCAL_HOST, &hintt.sin_addr);

    //server S port information.

    sockaddr_in hints;
    memset(&hints, 0, sizeof(hints));
    hints.sin_family = AF_INET;
    hints.sin_port = htons(ports);
    inet_pton(AF_INET, LOCAL_HOST, &hints.sin_addr);

    //server P port information.

    sockaddr_in hintp;
    memset(&hintp, 0, sizeof(hintp));
    hintp.sin_family = AF_INET;
    hintp.sin_port = htons(portp);
    inet_pton(AF_INET, LOCAL_HOST, &hintp.sin_addr);


    sockaddr_in clientA; //For holding client A's IP and port number
    sockaddr_in clientB; //For holding client B's IP and port number
    int child_sock_a; //For holding child socket number of client A communication
    int child_sock_b; //For holding child socket number of client B communication



    //Listening on the two TCP sockets.

    if (listen(asock, SOMAXCONN)==-1)
    {
        cerr<<"Cant listen!"<<endl;
        return -1;
    }

    if (listen(bsock, SOMAXCONN)==-1)
    {
        cerr<<"Cant listen!"<<endl;
        return -1;
    }

    
    cout<<"The Central server is up and running."<<endl;


    fd_set current_sock; //For supporting 2 clients at the same time. This allows us to get input from either of the clients in any order, 
                         //i.e., client A first and then client B "OR" Client B first and then client A.
    FD_ZERO(&current_sock);
    FD_SET(asock, &current_sock);
    FD_SET(bsock, &current_sock);

    char user_namea[512];
    char user_nameb[512];

    

    string str; //a temporary string to store the character array and split into nodes.
    char user_nameb_tmp [1024]; //To store the character array received from the client B
    char user_nameb_bonus [512] = ""; //To store the second username from B.

	//Always ON server

	while (true)
    {   
        for(int j=0; j<2; j++)
        {
            fd_set ready_socket = current_sock;
            if (select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL)<0)
            {
                cerr<<"Error in select"<<endl;
                return -1;
            }
            for(int i=0; i<FD_SETSIZE; i++)
            {
                if(FD_ISSET(i, &ready_socket))
                {
                    if(i==asock)
                    {   
                        //Accepting connection from Client A
                        socklen_t clientSizeA = sizeof(clientA);
                        child_sock_a = accept(asock, (sockaddr*)&clientA, &clientSizeA);
                        if (child_sock_a == -1)
                        {
                            cerr<<"Problem with client A connecting."<<endl;
                            return -1;
                        }
                        
                        memset(user_namea, 0, 512);
                        if(recv(child_sock_a, user_namea, 512, 0)==-1)
						{
							cerr<<"Error in receiving username from client A"<<endl;
							return -1;
						}

                        cout<<"The Central server received input=\""<<user_namea<<"\" from the client using TCP over port 25998."<<endl;

             
    
                    }
                    else if(i==bsock)
                    {   
                        //Accepting connection from Client B
                        socklen_t clientSizeB = sizeof(clientB);                 
                        child_sock_b = accept(bsock, (sockaddr*)&clientB, &clientSizeB);
                        if (child_sock_b == -1)
                        {
                            cerr<<"Problem with client B connecting."<<endl;
                            return -1;
                        }
                        
                        memset(user_nameb_tmp, 0, 512);
                        if(recv(child_sock_b, user_nameb_tmp, 512, 0)==-1)
						{
							cerr<<"Error in receiving username from client B"<<endl;
							return -1;
						}

                         

                    }

                }
            }
        }
		
		string user_nameb_split [2];
		str = user_nameb_tmp;
		istringstream ss(str);
		string word;
		int username_index=0;
		int bonus_count=1;
		
		
		while(ss>>word) //Splitting the combined username from client B into usernames.
		{
			user_nameb_split[username_index]=word;
			username_index++;
		}
		
		strcpy(user_nameb, user_nameb_split[0].c_str());
		strcpy(user_nameb_bonus, user_nameb_split[1].c_str());
		
		

		if (strcmp(user_nameb_bonus, "") !=0)
		{
			bonus_count=2;
		}

		if(bonus_count==2)
		cout<<"The Central server received input=\""<<user_nameb<<"\" and \""<<user_nameb_bonus<<"\" from the client using TCP over port 26998."<<endl;
		else
		cout<<"The Central server received input=\""<<user_nameb<<"\" from the client using TCP over port 26998."<<endl;
		
        char bonus_count_char = '0' + bonus_count;

        //Informing client A if there are 2 usernames from B or not.
        if(send(child_sock_a, &bonus_count_char, 1, 0)==-1)
        {
            cerr<<"Error in sending the number of client B usernames to client A"<<endl;
            return -1;
        }

		for(int b=0; b<bonus_count; b++)
		{

			// Sending A's username to Topology server

			if(sendto(backendsock, user_namea, sizeof(user_namea), 0, (sockaddr*)&hintt, sizeof(hintt))==-1)
			{
				cerr<<"Could not send A's username to server T"<<endl;
				return -1;
			}      

			// Sending B's username to Topology server                        
			if(sendto(backendsock, user_nameb, sizeof(user_nameb), 0, (sockaddr*)&hintt, sizeof(hintt))==-1)
			{
				cerr<<"Could not send B's username to server T"<<endl;
				return -1;
			} 

			cout<<"The Central server sent a request to Backend-Server T."<<endl;


			char line_count[10];
			socklen_t hintt_size = sizeof(hintt);
			if(recvfrom(backendsock, line_count, 10, 0, (sockaddr*)&hintt, &hintt_size)==-1) //Receiving the information about number of lines in the reduced graph.
			{
				cerr<<"Error in receiving line count from server T"<<endl;
				return -1;
			}
			
			string line_count_str = line_count;
			int count = stoi(line_count_str);

			char file_buffer[16384];
			ofstream reduced_graph;
			reduced_graph.open("reduced_graph.txt");

			//Receiving the reduced graph from the topology server and writing into the file reduced_graph.txt
			for(int i=0; i<count; i++)
			{   memset(file_buffer, 0, 16384);
				if(recvfrom(backendsock, file_buffer, 16384, 0, (sockaddr*)&hintt, &hintt_size)==-1)
				{
					cerr<<"Error in receiving path from server T"<<endl;
					return -1;
				}

				reduced_graph<<file_buffer<<"\n";
				paths[i] = file_buffer;

			}
			reduced_graph.close();

			cout<<"The Central server received information from Backend-Server T using UDP over port 21998."<<endl;

			ifstream read_reduced_graph("reduced_graph.txt");

			n_nodes=0;
			while(read_reduced_graph>>node_list[n_nodes]) //Computing the number of nodes in the reduced graph(including the repeated ones)
			n_nodes++;

			read_nodes();

			string number_of_nodes_str = "";
			number_of_nodes_str = to_string(number_of_nodes);			

			//Sending the number of distinct nodes in the reduced graph to the score server.
			
			if(sendto(backendsock, number_of_nodes_str.c_str(), number_of_nodes_str.size() + 1, 0, (sockaddr*)&hints, sizeof(hints))==-1) 
			{
				cerr<<"Error in sending the node count to server S"<<endl;
				return -1;
			}

			cout<<"The Central server sent a request to Backend-Server S."<<endl;

			//Sending a list of distinct nodes to server S and getting the scores for each of them.

			socklen_t hints_size = sizeof(hints);
			char scores_char[512];
			for (int i=0; i<number_of_nodes; i++)
			{
				if(sendto(backendsock, distinct_nodes[i].node.c_str(), distinct_nodes[i].node.size() + 1, 0, (sockaddr*)&hints, sizeof(hints))==-1)
				{
					cerr<<"Error in sending the node information to server S"<<endl;
					return -1;
				}

				memset(scores_char, 0, 512);
				if(recvfrom(backendsock, scores_char, 512, 0, (sockaddr*)&hints, &hints_size)==-1)
				{
					cerr<<"Error in receiving score information from server S"<<endl;
					return -1;
				}
				distinct_nodes[i].score = scores_char;

			}

			cout<<"The Central server received information from Backend-Server S using UDP over port 22998."<<endl;


			//Sending required score list to the Processing server.

			if(sendto(backendsock, number_of_nodes_str.c_str(), number_of_nodes_str.size() +1, 0, (sockaddr*)&hintp, sizeof(hintp))==-1)
			{
				cerr<<"Error in sending node count to server P"<<endl;
				return -1;
			}
		
			for (int i=0; i<number_of_nodes; i++)
			{
				if(sendto(backendsock, distinct_nodes[i].node.c_str(), distinct_nodes[i].node.size() + 1, 0, (sockaddr*)&hintp, sizeof(hintp))==-1)
				{
					cerr<<"Error in sending node information to server P"<<endl;
					return -1;
				}
				
				usleep(1);

				if(sendto(backendsock, distinct_nodes[i].score.c_str(), distinct_nodes[i].node.size() + 1, 0, (sockaddr*)&hintp, sizeof(hintp))==-1)
				{
					cerr<<"Error in sending score information to server P"<<endl;
					return -1;
				}
				
				usleep(1);
			}

			//Sending the paths to the Processing server.

			if(sendto(backendsock, line_count, 10, 0, (sockaddr*)&hintp, sizeof(hintp))==-1) //Sending the number or lines in the reduced path.
			{
				cerr<<"Error in sending line count to server P"<<endl;
				return -1;
			}    
				

			for (int i=0; i<count; i++)
			{
				if(sendto(backendsock, paths[i].c_str(), paths[i].size() + 1, 0, (sockaddr*)&hintp, sizeof(hintp))==-1)
				{
					cerr<<"Error in sending paths to server P"<<endl;
					return -1;
				}
				usleep(1);

			}
			
			cout<<"The Central server sent a processing request to Backend-Server P."<<endl;


			//Receiving shortest path from the processing server.
			char shortest_path[16384];
			char shortest_weight[512];
			socklen_t hintp_size = sizeof(hintp);
			
			memset(shortest_path, 0, 16384);
			if(recvfrom(backendsock, shortest_path, 16384, 0, (sockaddr*)&hintp, &hintp_size)==-1)
			{
				cerr<<"Error in receiving shortest path from server P"<<endl;
				return -1;
			}
				
			memset(shortest_weight, 0, 512);
			if(recvfrom(backendsock, shortest_weight, 512, 0, (sockaddr*)&hintp, &hintp_size)==-1)
			{
				cerr<<"Error in receiving smallest compatibility score from server P"<<endl;
				return -1;
			}
			
			cout<<"The Central server received the results from backend server P."<<endl;

			//Sending the client B information to A.
			if(send(child_sock_a, user_nameb, 512, 0)==-1)
			{
				cerr<<"Could not send the Client B information to Client A!"<<endl;
				return -1;
			}

			//Sending the shortest path to client A.
			if(send(child_sock_a, shortest_path, 16384, 0)==-1)
			{
				cerr<<"Could not send the path to Client A!"<<endl;
				return -1;
			}

			if(send(child_sock_a, shortest_weight, 512, 0)==-1)
			{
				cerr<<"Could not send the compatibility score to Client A!"<<endl;
				return -1;
			}

			cout<<"The Central server sent the results to client A."<<endl;


			//Sending the client A information to B.
			if(send(child_sock_b, user_namea, 512, 0)==-1)
			{
				cerr<<"Could not send the Client A information to Client B!"<<endl;
				return -1;
			}

			//Sending the shortest path to client B.

			if(send(child_sock_b, shortest_path, 16384, 0)==-1)
			{
				cerr<<"Could not send the path to Client B!"<<endl;
				return -1;
			}

			if(send(child_sock_b, shortest_weight, 512, 0)==-1)
			{
				cerr<<"Could not send the compatibility score to Client B!"<<endl;
				return -1;
			}
			
			cout<<"The Central server sent the results to client B."<<endl;
			
			//Deleting reduced_graph.txt file
			remove("reduced_graph.txt");
			
			strcpy(user_nameb, user_nameb_bonus);

			for(int i=0; i<4096; i++) //To clear the contents of the string array after each iteration.
        	{
            paths[i] = "";
			}
			
			for(int i=0; i<512; i++) //To clear the contents of distinct_nodes structure after each iteration.
			{
				distinct_nodes[i].node = "";
				distinct_nodes[i].score = "";
			}

			cout<<endl;			
		}


        //Closing the child sockets.
        close(child_sock_a); 
        close(child_sock_b);

    }
    return 0;
  
}