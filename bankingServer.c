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


bankAccount * frontNode = NULL;

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
bankAccount* createNode(char * token){
	bankAccount *temp;
	temp = malloc(sizeof(bankAccount));
	temp->next = NULL;
    temp->accountName = (char*)malloc(sizeof(char)*(strlen(token)+1));
    strcpy(temp->accountName, token);
    temp->balance = 0;
    temp->inSessionFlag = 0;
	return temp;
}

//add new bank account node to linked list of account structs
void addNode(bankAccount** head, bankAccount * node){
	
    bankAccount *p = *head;
	
	if(*head == NULL){
		*head = node;
	}else{
		
		while(p->next != NULL){
			p = p->next;
		}
		p->next = node;
	}

	// printLL(head);	//test just to print
	// return head;
}


//boolean method to check if account exists already in linked list
int alreadyExists(char *token){
	bankAccount *temp = frontNode;

	while(temp != NULL){
		int result = strcmp(temp->accountName, token);
		
		if(result == 0){
			printf("Account name already exists!\n");
			return 1;
		}else{
			temp = temp->next;
		}
	}
	printf("Account does not exist, continue creation!\n");
	return 0;	//account not found, continue to create new account
}

//print bankAccount structs as a test
void printLL(){
	printf("\n");
    bankAccount *temp = frontNode;
	while(temp != NULL){
		printf("Account Name: %s\nBalance: $%lf\ninSessionFlag: %d\n\n", temp->accountName, temp->balance, temp->inSessionFlag);
		temp = temp->next;
	}
}

void printNode(bankAccount *node){
    printf("\nAccount Name: %s\nBalance: $%lf\ninSessionFlag: %d\n\n", node->accountName, node->balance, node->inSessionFlag);      
}

//Function designed for chat between client and server
void func(int sockfd){
    int inSession=0; 
    char r_buff[MAX]; 
    char w_buff[MAX];
    char command[20];
    char value[200];
    int n; 
    int i;
    memset(command, 0, sizeof(command));
    memset(value, 0, sizeof(value));
    char currAccount[260];

    // infinite loop for chat
    for (;;) { 
        bzero(r_buff, MAX); 

        // read the message from client and copy it in buffer
	    read(sockfd, r_buff, sizeof(r_buff));

        // print buffer which contains the client contents
        printf("From client: %s\n", r_buff); 
         
        sscanf(r_buff, "%s %s",command, value);
        printf("\n\n----------------\ncommand: %s\n", command);
        printf("value: %s\n---------------\n\n", value );
    	
       
    	//if msg contains "create" then server will create an account
    	if (strncmp("create", command, 6) == 0){
    		//handle create account
    		//ERR check: if account already created
    		//if <accountname> input = acct_name 
    		if(numAccounts == 0){
                printf("0 Accts: will create a NEW account!\n");
        	    bankAccount *node = createNode(value);
                addNode(&frontNode,node);    
    		    numAccounts++;
    		    // printf("Number of Accounts: %d\n", numAccounts);
                bzero(w_buff, MAX); 
                strcpy(w_buff,"Account successfully added\n");
                write(sockfd, w_buff, sizeof(w_buff));
		    } else {
                if(alreadyExists(value) == 1) {
                    bzero(w_buff, MAX); 
                    strcpy(w_buff,"Error, this account already exists!\n");
                    write(sockfd, w_buff, sizeof(w_buff));
                    // printf("Error, this account already exists!\n");
                } else {
                	printf("will create NEW account!!\n");
                    bankAccount *node = createNode(value);
            		addNode(&frontNode, node);
    				numAccounts++;	
    				// printf("Number of Accounts: %d\n", numAccounts);
                    bzero(w_buff, MAX); 
                    strcpy(w_buff,"Account successfully added\n");
                    write(sockfd, w_buff, sizeof(w_buff));
		    	}
                	
            }	     
    	   
        } else if (strncmp("serve", command, 5) == 0){
            inSession = serveAcct(sockfd, frontNode, value, inSession);
            if (inSession==1){
                strcpy(currAccount,value);
            }
        } else if (strncmp("deposit", command, 7) == 0){
            deposit(sockfd, frontNode, currAccount, inSession);
        } else if (strncmp("withdraw", command, 8) == 0){
            withdraw(sockfd, frontNode, currAccount, inSession);
        } else if (strncmp("query", command, 5) == 0){
            query(sockfd, frontNode, currAccount, inSession);
        } else if (strncmp("end", command, 3) == 0){
            inSession = end(sockfd, frontNode, currAccount, inSession);
        } else if(strncmp("quit", command, 4) == 0){
    	   printf("client quit, so check if other clients there\n");
    	   break;
	   } 
    	
        memset(r_buff, 0, sizeof(r_buff));
        memset(w_buff, 0, sizeof(w_buff));
        memset(command, 0, sizeof(command));
        memset(value, 0, sizeof(value));

    } 
} 


int serveAcct(int sockfd, bankAccount *frontNode, char * acctName, int inSession){
	
}
void deposit(int sockfd, bankAccount *frontNode, char* currAccount, int inSession){

}
void withdraw(int sockfd, bankAccount *frontNode, char* currAccount, int inSession){

}
void query(int sockfd, bankAccount *frontNode, char* currAccount, int inSession){

}
int end(int sockfd, bankAccount *frontNode, char* currAccount, int inSession){
    
}





//Driver function
int main() { 

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
