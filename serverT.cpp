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

string username [2];



typedef struct data //Structure to store the edgelist.
{
	string first = "";
	string second = "";
}Edges;

typedef struct datanum //Structure to store integer mappings of the edgelist.
{
	int first = 0;
	int second = 0;
}Edges_num;

typedef struct s //Structure to store distinct nodes.
{
	string n = "";
}name;

name nodes[512]; //For storing distinct nodes.
int number_of_nodes = 0;

Edges edges[256];
Edges_num edges_num[256];
int lines = 0 ;
ofstream reduced_edgelist;


unsigned int nodes_exist(string s, name a[]) //Checking if the node is already in the list of distinct nodes or not.
{
	unsigned int exists =0;
	for(int i=0; i<number_of_nodes; i++)
		if(s == a[i].n)
			exists = 1;
	return exists;
}

void read_nodes() //For populating the distinct nodes in the distinct node structure. 
{   number_of_nodes=0;
	for(int i=0; i<lines; i++)
	{   
		if (!nodes_exist(edges[i].first, nodes))
		{
			nodes[number_of_nodes].n = edges[i].first;
			number_of_nodes++;
		}
		if (!nodes_exist(edges[i].second, nodes))
		{
			nodes[number_of_nodes].n = edges[i].second;
			number_of_nodes++;
		}
	}
}


//The following DFS code has been sourced from Geek for Geeks.
//URL: https://www.geeksforgeeks.org/find-paths-given-source-destination/?ref=lbp
//This code has been modified as per the requirements of this project.

class Graph {
    int V; // No. of vertices in graph
    list<int>* adj; // Pointer to an array containing adjacency lists
 
    // A recursive function used by path_calculation()
    void path_calculation_recursion(int, int, bool[], int[], int&);
 
public:
    Graph(int V); 
    void addEdge(int u, int v);
    void path_calculation(int s, int d);
};
 
Graph::Graph(int V)
{
    this->V = V;
    adj = new list<int>[V];
}

 
void Graph::addEdge(int u, int v)
{
    adj[u].push_back(v); // Add v to u’s list.
    adj[v].push_back(u);
}
 
// Calculate all paths from 's' to 'd'
void Graph::path_calculation(int s, int d)
{
    bool* visited = new bool[V];
 
    int* path = new int[V];
    int path_index = 0; 
 
    for (int i = 0; i < V; i++)
        visited[i] = false;
 
    path_calculation_recursion(s, d, visited, path, path_index);
}
 
// A recursive function to calculate all paths from 'u' to 'd'.

void Graph::path_calculation_recursion(int u, int d, bool visited[],
                              int path[], int& path_index)
{
    visited[u] = true;
    path[path_index] = u;
    path_index++;

    if (u == d) {
        for (int i = 0; i < path_index; i++)
        {
            if(i==path_index-1) //Implementing it like this so that we dont get a white space at end of each line.
            {
                reduced_edgelist<< nodes[path[i]].n;
            }
            else 
            {            
                reduced_edgelist<< nodes[path[i]].n << " ";
            }
        }
        reduced_edgelist<<"\n";
    }
    else // If current vertex is not destination
    {
        // Recur for all the vertices adjacent to current vertex
        list<int>::iterator i;
        for (i = adj[u].begin(); i != adj[u].end(); ++i)
            if (!visited[*i])
                path_calculation_recursion(*i, d, visited, path, path_index);
    }
 
    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited[u] = false;
}

//The externally sourced DFS code ends here.

int main()
{   
    //Creating and Binding the UDP socket for connections towards the Central Server.
    int tsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tsock ==-1)
    {
        cerr<<"Cant create a socket."<<endl;
        return -1;
    }
    
    
    sockaddr_in hintt;
    memset(&hintt, 0, sizeof(hintt));
    hintt.sin_family = AF_INET;
    hintt.sin_port = htons(21998);
    inet_pton(AF_INET, LOCAL_HOST, &hintt.sin_addr);


    if(bind(tsock, (sockaddr*)&hintt, sizeof(hintt))==-1) 
    {
        cerr<<"Can't bind the addresses to the socket"<<endl;
        return -1;
    }

    
    //Information of the socket at the central server.
    sockaddr_in hintc;
    memset(&hintc, 0, sizeof(hintc));
    socklen_t hintc_size = sizeof(hintc);

    cout<<"The ServerT is up and running using UDP on port 21998."<<endl;


    //Reading the edgelist and putting it in a structure.

    ifstream edgelist("edgelist.txt");

    lines=0;
    while(edgelist>>edges[lines].first>>edges[lines].second)
    lines++;

	read_nodes(); //Creating a structure of distinct nodes.

    //Converting String to corresponding numbers for DFS.
    for(int i=0; i<lines; i++)
        for(int j=0; j<number_of_nodes; j++)
        {
            if(edges[i].first==nodes[j].n)
                edges_num[i].first=j;
            else if (edges[i].second==nodes[j].n)
                edges_num[i].second=j;
        }
    
    //Creating a graph.
    Graph g(number_of_nodes);
	for(int i=0; i<lines; i++)
		{	    
				g.addEdge(edges_num[i].first, edges_num[i].second);
		}
    
    char buffer[512];

    //Always On Server
    while (true)
    {   
        username[0] = "";
        username[1] = "";

        for(int i=0; i<2; i++)
        {
            memset(buffer, 0, 512);
            if(recvfrom(tsock, buffer, 512, 0, (sockaddr*)&hintc, &hintc_size)==-1)
            {
                cerr<<"Error in receiving usernames from central"<<endl;
                return -1;
            }
            username[i]=buffer;
        }

        cout<<"The ServerT received a request from Central to get the topology."<<endl;
    

        string username_a = username[0];
        int username_a_num = 0;
        string username_b = username[1];
        int username_b_num = 0;

        //Converting the received username to integer.
        for(int j=0; j<number_of_nodes; j++)
        {
            if(nodes[j].n==username_a)
                username_a_num=j;
            
            if(nodes[j].n==username_b)
                username_b_num=j;
        }
                
        reduced_edgelist.open("reduced_edgelist.txt");
        
        //Calculating the paths between the received usernames.
        g.path_calculation(username_a_num, username_b_num);

        reduced_edgelist.close();


        ifstream sendfile("reduced_edgelist.txt");
        string send_to_c;
        int count = 0;

        while(getline(sendfile, send_to_c))
        {
            count++;
        }
        sendfile.close();


        string line_count = "";
        line_count = to_string(count);

        if(sendto(tsock, line_count.c_str(), line_count.size() + 1, 0, (sockaddr*)&hintc, sizeof(hintc))==-1)
        {
            cerr<<"Error in sending line count to the central server"<<endl;
            return -1;
        }

        sendfile.open ("reduced_edgelist.txt");
        while(getline(sendfile, send_to_c))
        {
            if(sendto(tsock, send_to_c.c_str(), send_to_c.size() + 1, 0, (sockaddr*)&hintc, sizeof(hintc))==-1)
                {
                    cerr<<"Error in sending paths to the central server"<<endl;
                    return -1;
                }
            
            send_to_c = "";
            usleep(1);
        }

        cout<<"The ServerT finished sending the topology to Central."<<endl;
        cout<<endl;

        remove("reduced_edgelist.txt");
    }

return 0;

}