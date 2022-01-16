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

#define LOCAL_HOST "127.0.0.1"

using namespace std;

typedef struct data ////Structure to store the scores.
{
	string node = "";
	string score = "";
}Scores;

Scores scores[512];
int lines = 0;



int main()
{   
    //Creating and Binding the UDP socket for connections towards the Central Server.
    int ssock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ssock ==-1)
        {
            cerr<<"Cant create a socket."<<endl;
            return -1;
        }
    
    
    sockaddr_in hints;
    memset(&hints, 0, sizeof(hints));
    hints.sin_family = AF_INET;
    hints.sin_port = htons(22998);
    inet_pton(AF_INET, LOCAL_HOST, &hints.sin_addr);

    if(bind(ssock, (sockaddr*)&hints, sizeof(hints))==-1) 
    {
        cerr<<"Can't bind the addresses to the socket"<<endl;
        return -1;
    }
    

    //Information of the socket at the central server.
    sockaddr_in hintc;
    memset(&hintc, 0, sizeof(hintc));
    socklen_t hintc_size = sizeof(hintc);


    cout<<"The ServerS is up and running using UDP on port 22998."<<endl;

    ifstream scorelist("scores.txt");

    lines=0;
    while(scorelist>>scores[lines].node>>scores[lines].score) //Extracting the scores from the text file and storing it in a structure.
    lines++;

    //Always ON Server.

    while(true)
    {
        char number_of_nodes_char[10];
        if(recvfrom(ssock, number_of_nodes_char, 10, 0, (sockaddr*)&hintc, &hintc_size)==-1)
        {
            cerr<<"Error in receiving node count from central"<<endl;
            return -1;
        }
        
        string number_of_nodes_str = number_of_nodes_char;
        int number_of_nodes = stoi(number_of_nodes_str);
        
        cout<<"The ServerS received a request from Central to get the scores."<<endl;


        char buffer[512];
        for(int i=0; i<number_of_nodes; i++)
        {   
            memset(buffer, 0, 512);
            if(recvfrom(ssock, buffer, 512, 0, (sockaddr*)&hintc, &hintc_size)==-1)
            {
                cerr<<"Error in receiving node information from central"<<endl;
                return -1;
            }

            for(int j=0; j<lines; j++)
            if(buffer==scores[j].node)
            {
                if(sendto(ssock, scores[j].score.c_str(), scores[j].score.size() + 1, 0, (sockaddr*)&hintc, sizeof(hintc))==-1)
                {
                    cerr<<"Error in sending scores to the central server"<<endl;
                    return -1;
                }
                
            }
        }

        cout<<"The ServerS finished sending the scores to Central."<<endl;
        cout<<endl;

    }    

    return 0;

}