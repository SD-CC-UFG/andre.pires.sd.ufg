//#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>
//#include <stdlib.h>
//#include <string.h>

#define SERVER_PORT 5432 
#define MAX_LINE 256
 
int s; 

using namespace std;

void *receiveMessage(void *arg){
	while(1){
		char response[MAX_LINE];
		recv(s, response, sizeof(response), 0);
		printf("%s", response);
	}
}

void *sendMessage(void *name){
	while(1){
		int len;
		char buf[MAX_LINE], output[MAX_LINE]; 
		fgets(buf, MAX_LINE-strlen((char*)name)-4, stdin);
		sprintf(output, "%s: %s", (char*)name, buf);
		output[MAX_LINE-1] = '\0';
		len = strlen(output) + 1;
		send(s, output, len, 0);
	}
}

int main(int argc, char * argv[]) { 
  FILE *fp; 
  struct hostent *hp; 
  struct sockaddr_in sin; 
  char *host; 
  char buf[MAX_LINE]; 
  
  int len; 

  if (argc==2) { 
    host = argv[1]; 
  } 
  else { 
    fprintf(stderr, "usage: simplex-talk host\n"); 
    exit(1); 
  } 

  /* translate host name into peer’s IP address */ 
  hp = gethostbyname(host); 
  if (!hp) { 
    fprintf(stderr, "simplex-talk: unknown host: %s\n", host); 
    exit(1);
  } 

  /* build address data structure */ 
  bzero((char *)&sin, sizeof(sin)); 
  sin.sin_family = AF_INET; 
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length); 
  sin.sin_port = htons(SERVER_PORT); 

  /* active open */ 
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) { 
    perror("simplex-talk: socket"); 
    exit(1); 
  } 

  printf("**Socket created\n");

  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) { 
    perror("simplex-talk: connect"); 
    close(s); 
    exit(1); 
  } 

  printf("**Socket connected\n");
  string myname;
  printf("Digite seu nome: ");
  cin >> myname;
  char *mynamec = (char*)myname.c_str();

  pthread_t sendm, receivem;
  pthread_create(&sendm, NULL, sendMessage, (void *)mynamec);
  pthread_create(&receivem, NULL, receiveMessage, NULL);
  pthread_join(sendm, NULL);
  pthread_join(receivem, NULL);
  
  return 0;
} 
