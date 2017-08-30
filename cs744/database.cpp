#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <map>
#include <sstream>
using namespace std;

void service (int sock);
std::map<string,int> hash;
void error(char *msg)
{
    perror(msg);
    exit(1);
}

void serialize(map<string,int> hash,string filename)
{
	ofstream file(filename.c_str());
	for (std::map<string,int>::iterator it=hash.begin(); it!=hash.end(); ++it)
    	file << it->first << " " << it->second<<endl;
  	file.close();
}

void deserialize(map<string,int>& hash,string filename)
{
	std::ifstream myfile(filename.c_str());
	string line="";
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{	
			size_t index=-1;
			index=line.find (' ');
			string key=line.substr(0,index);
			line=line.substr(index+1);
			cout << key<<" " << line<<" ";
			hash[key]=atoi(line.c_str());
		}
		myfile.close();
		cout<<endl;
	}  	
}

string insert(map<string,int> &hash,string filename,string key, int value)
{
	hash[key]=value;
	serialize(hash,filename);
}

string update(map<string,int> &hash,string filename,string key, int value)
{
	insert(hash,filename,key,value);
}


string del(map<string,int> &hash,string filename,string key)
{
	map<string,int>::iterator it;
	it = hash.find (key);            
    hash.erase (it);
	serialize(hash,filename);
	return "Success";
}



int search(map<string,int> hash,string key)
{
	for (std::map<string,int>::iterator it=hash.begin(); it!=hash.end(); ++it)
    	cout << it->first << " " << it->second<<endl;
	map<string,int>::iterator it;
	it = hash.find (key);            
   	return it->second;
}



int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, pid;
	struct sockaddr_in serv_addr, cli_addr;

	
 	deserialize(hash,"database.db");
	
	
    if (argc < 2) 
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
       error((char *)"ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error((char *)"ERROR on binding");

    listen(sockfd,100);

    clilen = sizeof(cli_addr);

    while (1) 
	{
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,(socklen_t*)&clilen);
        if (newsockfd < 0) 
            error((char *)"ERROR on accept");

        pid = fork();

        if (pid < 0)
            error((char *)"ERROR on fork");
        else if (pid == 0)  {
            close(sockfd);
            service(newsockfd);
            exit(0);
        }
        else close(newsockfd);
    } 
    return 0; 
}

void service(int sock)
{
   int n;
   char buffer[256];
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error((char *)"ERROR reading from socket");
   string str(buffer);
   //printf("Here is the message: %s\n",buffer);
   //n = write(sock,"I got your message",18);
	stringstream ss;
	ss << search(hash,str);
	string s = ss.str();
	
   write(sock,s.c_str(),255);
   if (n < 0) error((char*)"ERROR writing to socket");
}
