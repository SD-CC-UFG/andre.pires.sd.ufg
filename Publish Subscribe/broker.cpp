#include <bits/stdc++.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#define MAX_PENDING 20
#define MAX_LINE 256

using namespace std;

vector<int> sockets;

pthread_mutex_t pubMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t subMutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------
map<string, string> newsTable;
map<string, vector<int> > interestTable;


//------------------------------

void acceptNewConnections(int port);

void *connectionHandler(void *sock);

void publish(void *input);

void subscribe(int socketID, char* subject_c);

void notify(string subject, string newsContent, int socketID);

void sendInterestList(int socketID);

int main(){
	int port;
	printf("Digite o numero da porta a ser utilizada: ");
	scanf("%d", &port);
	acceptNewConnections(port);
	return 0;
}

void notify(string subject, string newsContent, int socketID){
	vector<int> clientsToNotify = interestTable[subject];
	string message = "Assunto: " + subject + ", Noticia: " + newsContent;
	char *message_c = (char *)message.c_str();
	if(socketID == -1){
		for(int cont = 0 ; cont < clientsToNotify.size() ; cont++){
			if(clientsToNotify[cont] != -1 && send(clientsToNotify[cont], message_c, strlen(message_c), 0) < 0){
				perror("Failed to send message, removing client from list");
				pthread_mutex_lock(&subMutex);
				interestTable[subject][cont] = -1;
				pthread_mutex_unlock(&subMutex);
			}
		}	
	}
	else{
		if(send(socketID, message_c, strlen(message_c), 0) < 0){
			perror("Failed to send message");
		}
	}
}


void publish(void *input){
	char *subject_c, *newsContent_c;
	string subject, newsContent;
	int inputLength = strlen((char *)input);
	subject_c = (char *)malloc(inputLength * sizeof(char));
	newsContent_c = (char *)malloc(inputLength * sizeof(char));
	sscanf((char *)input, "%s %s", subject_c, newsContent_c);
	subject = string(subject_c);
	newsContent = string(newsContent_c);
	free(subject_c);
	free(newsContent_c);
	
	//MUTEX LOCKED
	pthread_mutex_lock(&pubMutex);
	newsTable[subject] = newsContent;
	pthread_mutex_unlock(&pubMutex);
	//MUTEX UNLOCKED
	
	notify(subject, newsContent, -1);
	
}

void sendInterestList(int socketID){
	char message[MAX_LINE];
	memset(message, '\0', sizeof(message));
	map<string, vector<int> >::iterator it;
	
	pthread_mutex_lock(&subMutex);
	for(it = interestTable.begin() ; it != interestTable.end(); it++){
		char *value = (char *)(it->first).c_str();
		if(strlen(value) + strlen(message) + 2 < MAX_LINE){
			sprintf(message, "%s\n", value);
		}
		else
			break;
	}
	pthread_mutex_unlock(&subMutex);
	
	if(send(socketID, message, strlen(message), 0) < 0){
		perror("Failed to send message");
	}
}

void subscribe(int socketID, char* subject_c){
	bool aux = false;
	string subject = string(subject_c);
	pthread_mutex_lock(&subMutex);
	vector<int>::iterator it = find(interestTable[subject].begin(), interestTable[subject].end(), socketID);
	if(it == interestTable[subject].end()){
		interestTable[subject].push_back(socketID);
		aux = true;
	}
	pthread_mutex_unlock(&subMutex);
	if(aux)
		notify(subject, newsTable[subject], socketID);
}

void *connectionHandler(void *sock){
	int socketID = *((int*)sock);
	char input[MAX_LINE]; 
	while(1){
		memset(input, '\0', sizeof(input));
		int callType;
		if(recv(socketID, input, sizeof(input), 0) < 0){
			perror("Failed to receive message");
			break;
		}
		sscanf(input, "%d", &callType);
		//pthread_t newThread;
		if(callType == 0){ //Publish
			//pthread_create(&newThread, NULL, publish, (void *)(input+2));
			publish((void *)(input+2));
		}
		else if(callType == 1){ //Subscribe
			//pthread_create(&newThread, NULL, subscribe, (void *)(input+2));
			subscribe(socketID, (input+2)); 
		}
		else if(callType == 2){ //Request Interest List
			sendInterestList(socketID);
		}
	}
}

void acceptNewConnections(int port){
	struct sockaddr_in myAddress, clientAddress;
	int socketID, newSocketID;
	socklen_t clientAddressLength = sizeof(clientAddress);
	bzero((char *)&myAddress, sizeof(myAddress)); 
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = INADDR_ANY;
	myAddress.sin_port = htons(port);
	socketID = socket(AF_INET, SOCK_STREAM, 0);
	if(socketID < 0){
		perror("Failed to create a socket");
		return;
	}
	if(bind(socketID, (struct sockaddr *)&myAddress, sizeof(myAddress)) < 0){
		perror("Failed to bind socket");
		return;
	}
	listen(socketID, MAX_PENDING);
	while(1){
		newSocketID = accept(socketID, (struct sockaddr *)&clientAddress, &clientAddressLength);
		if(newSocketID < 0){
			perror("Failed to create socket for incoming client");
			return;
		}
		printf("Connection successful!\n");
		sockets.push_back(newSocketID);
		pthread_t newThread;
		pthread_create(&newThread, NULL, connectionHandler, (void *)&newSocketID);
	}
}
