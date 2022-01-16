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
#include <bits/stdc++.h>
#include <sstream>

#define LOCAL_HOST "127.0.0.1"

using namespace std;

int num_nodes;
string nodes[512];

void split_into_nodes(string strpath) //Splitting the string(path) into words(nodes)
{	num_nodes=0;
    istringstream strstream(strpath);
    string word;
  
    while (strstream >> word) 
    {   
        nodes[num_nodes] = word;
        num_nodes++;
    }
}


int main(int argc, char* argv[])
{   
    string user_name = argv[1]; //Accepting the username from the command line argument.

    //Creating a TCP socket
    int asock = socket(AF_INET, SOCK_STREAM, 0);
    if (asock==-1)
    {
	cerr<<"Cant create a socket."<<endl;
	return -1;
    }


    //Information of the socket at the central server.
    int port =25998;
    
    sockaddr_in hintc;
    hintc.sin_family = AF_INET;
    hintc.sin_port = htons(port);
    inet_pton(AF_INET, LOCAL_HOST, &hintc.sin_addr);


    cout<<"The client is up and running."<<endl<<endl;

    //Establishing connection and sending the username to the central server

    int aconnect = connect(asock, (sockaddr*)&hintc, sizeof(hintc));
    if (aconnect ==-1)
    {
        cerr<<"Could not connect"<<endl;
        return -1;
    }
    
    int send_status = send(asock, user_name.c_str(), user_name.size() + 1, 0);
    if (send_status == -1)
    {
        cerr<<"Error in sending username to central"<<endl;
        return -1;
    }
    
    cout<<"The client sent "<<user_name<< " to the Central server."<<endl<<endl;

    char bonus_count_char;
    if(recv(asock, &bonus_count_char, 1, 0)==-1)
    {
        cerr<<"Error in receiving B's username count from central"<<endl;
    }

    int bonus_count = (int)bonus_count_char -48;

    for(int b=0; b<bonus_count; b++)
    {
        //Receiving Client B information from central server.
        char user_nameb[512];
        memset(user_nameb, 0, 512);
        recv(asock, user_nameb, 512, 0);

        //Receiving shortest path from central.

        char shortest_path[16384];
        char shortest_weight[512];

        memset(shortest_path, 0, 16384);
        if(recv(asock, shortest_path, 16384, 0)==-1)
        {
        cerr<<"Error in receiving paths from Central"<<endl;
        return -1;
        }

        memset(shortest_weight, 0, 512);
        if(recv(asock, shortest_weight, 512, 0)==-1)
        {
        cerr<<"Error in receiving compatibility score from Central"<<endl;
        return -1;
        }        

        cout.precision(3);


        //Displaying as per order

        string shortest_path_str = shortest_path;
        split_into_nodes(shortest_path_str);

       
        if(nodes[0] == user_name)
        {	
            cout<<"Found compatibility for "<<nodes[0]<<" and "<<nodes[num_nodes-1]<<":"<<endl;
            
            for(int i=0; i<num_nodes-1; i++)
            {
                cout<<nodes[i]<<" --- ";
            }
            cout<<nodes[num_nodes-1]<<endl;
            cout<<"Compatibility score: "<<stod(shortest_weight)<<endl;
        }
        else if(nodes[num_nodes-1] == user_name)
        {
            cout<<"Found compatibility for "<<nodes[num_nodes-1]<<" and "<<nodes[0]<<":"<<endl;
            for(int i=num_nodes-1; i>0; i--)
            {
                cout<<nodes[i]<<" --- ";
            }
            cout<<nodes[0]<<endl;
            cout<<"Compatibility score: "<<stod(shortest_weight)<<endl;
        }
        else
        {
            cout<<"Found no compatibility for "<<user_name<<" and "<<user_nameb<<"."<<endl;
        }

        for(int c; c<512; c++) //Reinitializing the string array to 0.
        nodes[c] = "";
        
        cout<<endl;
    }
    close(asock);
    return 0;

}

