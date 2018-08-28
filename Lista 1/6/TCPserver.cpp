//#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
//#include <stdlib.h>
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

  /* wait for connection, then receive and print text */ 
  //while(1) { 
    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) { 
      perror("simplex-talk: accept"); 
      exit(1); 
    } 

    printf("**Connection accepted\n");
	/*
    while (len = recv(new_s, buf, sizeof(buf), 0))
      fputs(buf, stdout);*/
    len = recv(new_s, buf, sizeof(buf), 0);
	char nome[MAX_LINE], nivel;
	char output[MAX_LINE];
	float salario;
	int dependentes;
	sscanf(buf, "%s%c%f%d", nome, nivel, &salario, &dependentes);
	
	if(nivel == 'A'){
		if(dependentes == 0)
			sprintf(output, "%f", salario * 0.97);
		else
			sprintf(output, "%f", salario * 0.92);
	}
	else if(nivel == 'B'){
		if(dependentes == 0)
			sprintf(output, "%f", salario * 0.95);
		else
			sprintf(output, "%f", salario * 0.90);
	}
	else if(nivel == 'C'){
		if(dependentes == 0)
			sprintf(output, "%f", salario * 0.92);
		else
			sprintf(output, "%f", salario * 0.85);
	}
	else{
		if(dependentes == 0)
			sprintf(output, "%f", salario * 0.90);
		else
			sprintf(output, "%f", salario * 0.83);
	}
	int len2 = strlen(output) + 1;
	send(new_s, output, len2, 0);
    close(new_s);
    printf("**Connection closed\n");
  //} 
}
