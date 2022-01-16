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
#include <sstream>
#include <list>
#include <math.h>
#include <bits/stdc++.h>
#include <float.h>

#define LOCAL_HOST "127.0.0.1"

using namespace std;

typedef struct data //Structure to store the scores sent by the central server.
{
	string node = "";
	string score = "";
}Scores;

Scores scores[512];

typedef struct path //Structure to store the paths sent by the Central server.
{
	string p = "";
	double weight = 0;
}Paths;

Paths paths[4096];

string nodes_in_line [256]; //String array to store the node names in a given path
int num_nodes_in_line; //Integer variable to store the number of nodes in a path.


void split_into_nodes(string strpath) //Used to split the string into words
{	num_nodes_in_line=0;

    istringstream strstream(strpath);
    string word; // variable for storing each word

    while (strstream >> word) 
    {
            nodes_in_line[num_nodes_in_line] = word;
			num_nodes_in_line++;
    }
}


int main()
{   
    //Creating and Binding the UDP socket for connections towards the Central Server.
    int psock = socket(AF_INET, SOCK_DGRAM, 0);
    if (psock ==-1)
        {
            cerr<<"Cant create a socket."<<endl;
            return -1;
        }


    sockaddr_in hintp;
    memset(&hintp, 0, sizeof(hintp));
    hintp.sin_family = AF_INET;
    hintp.sin_port = htons(23998);
    inet_pton(AF_INET, LOCAL_HOST, &hintp.sin_addr);


    if(bind(psock, (sockaddr*)&hintp, sizeof(hintp))==-1) 
        {
            cerr<<"Can't bind the addresses to the socket"<<endl;
            return -1;
        }


    //Information of the socket at the central server.

    sockaddr_in hintc;
    memset(&hintc, 0, sizeof(hintc));
    socklen_t hintc_size = sizeof(hintc);

    cout<<"The ServerP is up and running using UDP on port 23998."<<endl;

    //Always ON Server.

    while(true)
    {

        //Receiving relevant scores from central.
        char number_of_nodes_char[10];
        if(recvfrom(psock, number_of_nodes_char, 10, 0, (sockaddr*)&hintc, &hintc_size)==-1)
        {
            cerr<<"Error in receiving node count from central"<<endl;
            return -1;
        }

        string number_of_nodes_str = number_of_nodes_char;
        int number_of_nodes = stoi(number_of_nodes_str);


        char buffer[512];
        for(int i=0; i<number_of_nodes; i++)
        {   
            memset(buffer, 0, 512);
            if(recvfrom(psock, buffer, 512, 0, (sockaddr*)&hintc, &hintc_size)==-1)
            {
                cerr<<"Error in receiving nodes(names) from central"<<endl;
                return -1;
            }
            scores[i].node = buffer;
            
            memset(buffer, 0, 512);
            if(recvfrom(psock, buffer, 512, 0, (sockaddr*)&hintc, &hintc_size)==-1)
            {
                cerr<<"Error in receiving scores from central"<<endl;
                return -1;
            }
            scores[i].score = buffer;

        }
        
        //Receiving paths from central server.

        char line_count[10];
        if(recvfrom(psock, line_count, 10, 0, (sockaddr*)&hintc, &hintc_size)==-1)
        {
            cerr<<"Error in receiving line count from central"<<endl;
            return -1;
        }

        string line_count_str = line_count;
        int count = stoi(line_count_str);
        

        char file_buffer[16384];
        for(int i=0; i<count; i++)
        {   
            memset(file_buffer, 0, 16384);
            if(recvfrom(psock, file_buffer, 16384, 0, (sockaddr*)&hintc, &hintc_size)==-1)
            {
                cerr<<"Error in receiving paths from central"<<endl;
                return -1;
            }
            paths[i].p = file_buffer;
        }


        cout<<"The ServerP received the topology and score information."<<endl;


        //Calculating weights for each path and subsequently determining the shortest path.
        double shortest_path_weight=DBL_MAX;
        int shortest_path_index=0;

        for(int i=0; i<count; i++)
        {
            double score_calc=0;
            double s1=0;
            double s2=0;
            double matching_score=0;
            split_into_nodes(paths[i].p);
            for(int j=0; j<num_nodes_in_line-1; j++)
            {	
                matching_score=0;
                s1=0;
                s2=0;
                
                for(int k=0; k<number_of_nodes; k++)
                {
                    if(nodes_in_line[j]==scores[k].node)
                    {   
                        s1=stod(scores[k].score);
                    }
                    else if(nodes_in_line[j+1]==scores[k].node)
                    {
                        s2=stod(scores[k].score);
                    }
                
                }
                matching_score = (fabs(s1-s2))/(s1+s2);
                score_calc+=matching_score;
                
            }
            paths[i].weight = score_calc;
            
            //Calculating smallest weight.
            if(score_calc<shortest_path_weight)
            {
                shortest_path_weight = score_calc;
                shortest_path_index = i;
            }

            
        }

        string shortest_weight = "";
        shortest_weight = to_string(paths[shortest_path_index].weight);


        if(sendto(psock, paths[shortest_path_index].p.c_str(), paths[shortest_path_index].p.size() + 1, 0, (sockaddr*)&hintc, sizeof(hintc))==-1)
        {
            cerr<<"Error in sending shortest path to central"<<endl;
            return -1;
        }

        usleep(1);

        if(sendto(psock, shortest_weight.c_str(), shortest_weight.size() + 1, 0, (sockaddr*)&hintc, sizeof(hintc))==-1)
        {
            cerr<<"Error in sending smallest compatibility score to central"<<endl;
            return -1;
        }

        cout<<"The ServerP finished sending the results to the Central."<<endl;
        cout<<endl;

        for(int i=0; i<4096; i++)
        {
            paths[i].p = "";
            paths[i].weight = 0;
        }

        for(int i=0; i<512; i++)
        {
            scores[i].node = "";
            scores[i].score = "";
        }

    }

    return 0;
}