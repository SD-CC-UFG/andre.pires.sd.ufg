#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <bits/stdc++.h>
#include <unistd.h>

#define SERVER_PORT 5432 
#define MAX_PENDING 5 
#define MAX_LINE 256 

using namespace std;

int main() 
{ 
  struct sockaddr_in sin; 
  char buf[MAX_LINE]; 
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

    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) { 
      perror("simplex-talk: accept"); 
      exit(1); 
    } 

    printf("**Connection accepted\n");

    len = recv(new_s, buf, sizeof(buf), 0);
	char output[MAX_LINE];
	int salario;
	sscanf(buf, "%d", &salario);
	
	if(salario < 5){
		sprintf(output, "nenhuma");
	}
	else if(salario <= 7){
		sprintf(output, "infantil A");
	}
	else if(salario <= 10){
		sprintf(output, "infantil B");
	}
	else if(salario <= 13){
		sprintf(output, "juvenil A");
	}
	else if(salario <= 17){
		sprintf(output, "juvenil B");
	}
	else{
		sprintf(output, "adulto");
	}
	int len2 = strlen(output) + 1;
	send(new_s, output, len2, 0);
    close(new_s);
    printf("**Connection closed\n");
}
