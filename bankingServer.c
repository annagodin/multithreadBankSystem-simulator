#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <netinet/in.h>
#define PORT 9382
#define _GNU_SOURCE
#define MAX 80
#define SA struct sockaddr
//9382
typedef int bool;
#define true 1
#define false 0


int numAccounts = 0; //counter for number accounts created during session

struct account{

    char* accountname;
    double balance;   
    bool inSession;

};

//struct account *bankAccounts;

//create temp array of structs to hold accounts for that session
//don't forget to free before disconnecting curr sessions
//struct account *bankAccounts = (struct account *)malloc(sizeof(struct account));
struct account bankAccounts[20]; //test bc lazy

/*-------------GEEKS FOR GEEKS EXAMPLE ------------------*/

//Function designed for chat between client and server
void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    int i;
    // infinite loop for chat

    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer
	read(sockfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
  
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff));


	//get substring of msg
	int ret;
	char *token;
	char *tab = " ";

	token = strtok(buff, tab);
	token = strtok(NULL, "	\n");
	//test after: token = strtok(NULL, tab);


	//if msg contains "create" then server will create an account
	if (strncmp("create", buff, 6) == 0){
		//handle create account
		
		//ERR check: if account already created
		//if <accountname> input = acct_name 
		for(i = 0; i < numAccounts; i++){
			if(numAccounts == 0)
				printf("will create a NEW account!\n");
			
			if(strcmp(token, bankAccounts[i]->accountname) == 0){
				printf("error, this account exists!\n");
				return;
			}else{
				printf("create NEW account!!\n");
			}
		}
	//need to debug in the morning, im falling asleep haha
	} 
	
	
	// if msg contains "Exit" then server exit and chat ended.
	if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        }else{
		printf("not valid command!!!! \n");
		exit(-1)	//for test purposes lol;
	} 
    } 
} 


//Driver function
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 

    // Function for chatting between client and server
    func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 
