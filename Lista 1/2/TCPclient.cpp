#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <bits/stdc++.h>
#include <unistd.h>

#define SERVER_PORT 5432 
#define MAX_LINE 256 

using namespace std;

int main(int argc, char * argv[]) { 
  FILE *fp; 
  struct hostent *hp; 
  struct sockaddr_in sin; 
  char *host; 
  char buf[MAX_LINE]; 
  int s; 
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

  char nome[MAX_LINE], cargo[MAX_LINE];
  char response[MAX_LINE];
  int salario;
  cout << "Digite o nome: ";
  cin >> nome;
  cout << "Digite o genero: ";
  cin >> cargo;
  cout << "Digite a idade: ";
  cin >> salario;
  sprintf(buf, "%s %s %d\n", nome, cargo, salario);
  buf[MAX_LINE-1] = '\0';
  len = strlen(buf) + 1;
  send(s, buf, len, 0);
  int len2 = recv(s, response, sizeof(response), 0);
  printf("%s\n", response);
  close(s);
  printf("**Connection closed\n");
  
  return 0;
} 
