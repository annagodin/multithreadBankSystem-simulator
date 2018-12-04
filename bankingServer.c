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
#define MAX 500
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
void func(int sockfd){ 
    char buff[MAX]; 
    int n; 
    int i;
    // infinite loop for chat

    for (;;) { 
        bzero(buff, MAX); 

        // read the message from client and copy it in buffer
	    read(sockfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents
        printf("From client: %s\n", buff); 
         
    	//get substring of msg
        char* token = (char*)malloc(sizeof(char)*(strlen(buff))+2);
        strcpy(token,buff);
        //printf("token now: %s\n", token);
    	
        int ret;
    	char *cmd;
    	
        //ANNA: use strsep!!!!!
        cmd = strsep(&token, " ");
    	// token = strtok(buff, tab);
    	// token = strtok(NULL, "	\n");
    	//test after: token = strtok(NULL, tab);
    	printf("command: %s\n", cmd);
        printf("value: %s\n", token);
       
    	//if msg contains "create" then server will create an account
    	if (strncmp("create", cmd, 6) == 0){
    		//handle create account
    		
    		//ERR check: if account already created
    		//if <accountname> input = acct_name 
    		if(numAccounts == 0)
                    printf("0: will create a NEW account!\n");
            else {
                for(i = 0; i < numAccounts; i++){
                //struct *temp = bankAccounts;
                    
                    if(numAccounts != 0 && (strcmp(token, bankAccounts[i].accountname) == 0)){
                        printf("error, this account already exists!\n");
                        return;
                        //return some message to the client!!
                    }else{
                        printf("will create NEW account!!\n");
                    }
                }
            }     
    	} 
    	// if msg contains "Exit" then server exit and chat ended.
    	if (strncmp("exit", cmd, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 

        printf("Write a message to client: ");
        bzero(buff, MAX); 
        n = 0;
         // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');
        buff[strlen(buff)]='\0';
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff));
        printf("Sent message to client: '%s'\n",buff);
    } 
} 


//Driver function
int main() { 

//-------------TIFF TINGS--------------------
    // int sockfd, connfd, len; 
    // struct sockaddr_in servaddr, cli; 
  
    // // socket create and verification
    // sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    // if (sockfd == -1) { 
    //     printf("socket creation failed...\n"); 
    //     exit(0); 
    // } 
    // else
    //     printf("Socket successfully created..\n"); 
    // bzero(&servaddr, sizeof(servaddr)); 
  
    // // assign IP, PORT 
    // servaddr.sin_family = AF_INET; 
    // servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    // servaddr.sin_port = htons(PORT); 
  
    // // Binding newly created socket to given IP and verification 
    // if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
    //     printf("socket bind failed...\n"); 
    //     exit(0); 
    // } 
    // else
    //     printf("Socket successfully binded..\n"); 
  
    // // Now server is ready to listen and verification 
    // if ((listen(sockfd, 5)) != 0) { 
    //     printf("Listen failed...\n"); 
    //     exit(0); 
    // } 
    // else
    //     printf("Server listening..\n"); 
    // len = sizeof(cli); 
  
    // // Accept the data packet from client and verification 
    // connfd = accept(sockfd, (SA*)&cli, &len); 
    // if (connfd < 0) { 
    //     printf("server acccept failed...\n"); 
    //     exit(0); 
    // } 
    // else
    //     printf("server acccept the client...\n"); 
//--------------END TIFF TINGS-------------------------


//------------------ANNAS NEW SHIT--------------------
    int servSockFD;
    int clientSockFD;
    struct addrinfo request;
    request.ai_flags = AI_PASSIVE;
    request.ai_family = AF_INET;
    request.ai_socktype = SOCK_STREAM;
    request.ai_protocol = 0;
    request.ai_addrlen = 0;
    request.ai_canonname = NULL;
    request.ai_next = NULL;
    struct addrinfo *result;

    getaddrinfo(0,"9382", &request, &result);

    if ((servSockFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol) ) < 0 ){
        fprintf(stderr, "ERROR: Server socket could not be created: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("**Socket successfully created\n");
    if(bind(servSockFD, result->ai_addr, result->ai_addrlen) < 0){
        printf("ERROR: Bind failed: %s\n", strerror(errno));
        exit(1);
    }

    printf("**Socket successfully binded\n");
    int optval = 1;
    setsockopt(servSockFD, SOL_SOCKET, SO_REUSEADDR , &optval, sizeof(int));


    listen(servSockFD,5); // five connections can be queued
    printf("**Listening for connection\n");
    if((clientSockFD= accept(servSockFD, NULL, NULL)) < 0){
        fprintf(stderr, "ERROR: failed to accept: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("**Socket successfully accepted, shoudld be good to go\n");

//------------------END ANNAS NEW SHIT----------------



    // Function for chatting between client and server
    func(clientSockFD); 
  
    // After chatting close the socket 
    close(clientSockFD); 
    close(servSockFD);
} 
