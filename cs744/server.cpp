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
using namespace std;

void callServer(int); 

struct conf 
{
 string ip_add;
 int port;
 char start;
 char end; 
};

vector<struct conf> servers;
void readconfigurations(string filename)
{
	string s=;
	std::ifstream myfile(filename.c_str());
	string line="";
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{	size_t index=-1;
			struct conf temp; 
		        index=line.find (' ');
			temp.ip_add=line.substr(0,index);
			cout << temp.ip_add;
			line=line.substr(index+1);
			
			index=line.find (' ');
			temp.port=atoi(line.substr(0,index).c_str());
			cout << temp.port;
			line=line.substr(index+1);

			index=line.find (' ');
			temp.start=(line.substr(0,index))[0];
			cout << temp.start;
			line=line.substr(index+1);
			
			temp.end=line[0];
			cout << temp.end;

			cout<<endl;
			servers.push_back(temp);
		}
		cout<< servers.size();
		myfile.close();
  	}
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;
	readconfigurations("server.conf");
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
            callServer(newsockfd);
            exit(0);
        }
        else close(newsockfd);
    } 
    return 0; 
}

void callServer (int sock)
{
   int n;
   char buffer[256];
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error((char *)"ERROR reading from socket");
   //printf("Here is the message: %s\n",buffer);
   //n = write(sock,"I got your message",18);
   //if (n < 0) error("ERROR writing to socket");
}
