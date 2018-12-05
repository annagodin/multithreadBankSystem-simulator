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
#include "bankingServer.h"

#define PORT 9382
#define _GNU_SOURCE
#define MAX 500
#define SA struct sockaddr
//9382
typedef int bool;
#define true 1
#define false 0


int numAccounts = 0; //counter for number accounts created during session
// TRYING TO DEFINE STRUCT IN HEADER
struct account
{

    char accountName[266];
    double balance;   
    int inSessionFlag;
    struct account *next;
};

typedef struct account *bankAccount;
bankAccount head = NULL;

/* making the linked list of bank account structs
struct bankAccounts
{
	struct bankAccounts *next;
	struct account account;


};

bankAccounts *accountList = NULL;
*/
//struct account *bankAccounts;

//create temp array of structs to hold accounts for that session
//don't forget to free before disconnecting curr sessions
//struct account *bankAccounts = (struct account *)malloc(sizeof(struct account));
//struct account bankAccounts[20]; //test bc lazy


//account linked list node creation method
bankAccount createNode(){

	bankAccount temp;
	temp = (bankAccount)malloc(sizeof(bankAccount));
	temp->next = NULL;
	return temp;

}

//add new bank account node to linked list of account structs
bankAccount addNode(bankAccount head, char* token){

	bankAccount temp, p;
	temp = createNode();

	memset(temp->accountName, '\0', sizeof(temp->accountName));
	strcpy(temp->accountName, token);

	temp->balance = 0;
	temp->inSessionFlag = 0;

	if(head == NULL){
		head = temp;

	}else{

		p = head;
		
		while(p->next != NULL){
			p = p->next;
		}
		p->next = temp;
	}

	printLL(head);	//test just to print
	return head;

}

//boolean method to check if account exists already in linked list
int alreadyExists(char *token){


	bankAccount temp;
	temp = head;

	while(temp != NULL){
		int result = strcmp(temp->accountName, token);
		
		if(result == 0){
			printf("account name already exists!\n");
			return 1;
		}else{
			
			temp = temp->next;

		}
	}
	printf("account does not exist, continue creation!\n");
	return 0;	//account not found, continue to create new account

}

//print bankAccount structs as a test
printLL(bankAccount head){

	bankAccount temp;
	temp = head;

	while(temp != NULL){
		printf("Account Name: %s\n Balance: $%d\n inSessionFlag: %ld\n", temp->accountName, temp->balance, temp->inSessionFlag);
		temp = temp->next;
	}
	


}








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
    		if(numAccounts == 0){
                    printf("0: will create a NEW account!\n");
        	    addNode(head,token);    
		    numAccounts++;
		    printf("Number of Accounts: %d\n", numAccounts);
		    printLL(head);
		}else {
                
                    if(alreadyExists(token) == 1)
                    	{
                        	printf("error, this account already exists!\n");
        
                    	}else{
                        	printf("will create NEW account!!\n");
                    		addNode(head, token);
				printLL(head);
				numAccounts++;	
				printf("Number of Accounts: %d\n", numAccounts);
		    	}
                	
            	}	     
    	}
	else if(strncmp("quit", cmd, 4) == 0){
		
		printf("client quit, so check if other clients there\n");
		break;
	} 
    	// if msg contains "Exit" then server exit and chat ended.
    	if (strncmp("exit", cmd, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 


	// Just seeing what happens when commenting this out
        // I personally don't think server needs to have the ability to write back?
        // we can discuss this!
	/*
	printf("Write a message to client: ");
        bzero(buff, MAX); 
        n = 0;
         // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');
        buff[strlen(buff)]='\0';
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff));
        printf("Sent message to client: '%s'\n",buff);

	*/
    } 
} 



//wrote createAccount() as a linked list method addNode()
/*create account function
void createAccount(char* token){

	account newAccount;
				
	memset(newAccount.accountName, '\0', sizeof(newAccount.accountName));
	strcpy(newAccount.accountName,token);

	newAccount.balance = 0.0;
	newAccount.inSessionFlag= 0;

	printf("\nContents of new account struct: name = %s\n balance = $%d\n inSession = %d\n", newAccount.accountName, newAccount.balance, newAccount.inSessionFlag);


	 
	//Attach new account to linked list of structs holding all bank accounts
	
	
	
	numAccounts++;
	printf("number of accounts now: %d\n", numAccounts);

	



}
*/









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
    setsockopt(servSockFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    //setsockopt(servSockFD, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)); //based on his ex in class
										   //should fix time lockout with binding

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
