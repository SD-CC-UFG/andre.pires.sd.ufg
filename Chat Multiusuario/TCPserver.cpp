//#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
//#include <stdlib.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_PORT 5432 
#define MAX_PENDING 30 
#define MAX_LINE 256 

using namespace std;

vector<int> sockets;
//vector<*pthread_t> threads;

void *connectionHandler(void *sock){
	int socket = *((int*)sock);
	char input[MAX_LINE]; 
	while(1){
		recv(socket, input, sizeof(input), 0);
		for(int cont = 0 ; cont < sockets.size() ; cont++){
			if(sockets[cont] == socket)
				continue;
			send(sockets[cont], input, (strlen(input)+1), 0);
		}
	}
}

void acceptNewConnection(){
	struct sockaddr_in sin; 
	int len; 
	int s, new_s; 

	/* build address data structure */ 
	bzero((char *)&sin, sizeof(sin)); 
	sin.sin_family = AF_INET; 
	sin.sin_addr.s_addr = INADDR_ANY; 
	sin.sin_port = htons(SERVER_PORT); 

	/* create new socket */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) { 
		perror("simplex-talk: socket"); 
		exit(1); 
	} 

	printf("**Socket created\n");

	/* setup for passive open */ 
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) { 
		perror("simplex-talk: bind"); 
		exit(1); 
	} 

	printf("**Socket bound to local address\n");
	listen(s, MAX_PENDING); 
	while(1){
		/* wait for connection, then receive and print text */ 
		if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) { 
			perror("simplex-talk: accept"); 
			continue; 
		} 

		printf("**Connection accepted\n");
		sockets.push_back(new_s);
		pthread_t threadid;
		pthread_create(&threadid, NULL, connectionHandler, (void *)&new_s);
		//threads.push_back(&threadid);
		//threadid.
	}
}



int main(){ 
  
	acceptNewConnection();
    
	
    //close(new_s);
    
}
