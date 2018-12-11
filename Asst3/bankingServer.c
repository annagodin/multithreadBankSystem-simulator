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
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <signal.h>
#include "bankingServer.h"


#define PORT 9382
#define _GNU_SOURCE 
#define MAX 500
#define SA struct sockaddr
//9382
typedef int bool;
#define true 1
#define false 0


void sigint_handler(int sig); /*prototype*/
int numAccounts; //counter for number accounts created during session
pthread_mutex_t openAccLock;
pthread_mutex_t clientInfoLock;

bankAccount * frontNode = NULL;
clientInfo * clientInfoFront = NULL;

sem_t sem;
int pshared;
int ret;
int value;



//create temp array of structs to hold accounts for that session
//don't forget to free before disconnecting curr sessions
//struct account *bankAccounts = (struct account *)malloc(sizeof(struct account));
//struct account bankAccounts[20]; //test bc lazy




int isNumeric(char*str){

    int i;
    int isDec = 0;
    int numDec =0;
    for(i = 0; i < strlen(str); i++){
        
        if((!isdigit(str[i])) && str[i] != '.'){
            //printf("the string is not a number. \n");
            return 0;       //string is a word
        }
        
        if(str[i] == '.'){
            numDec+=1;
            //printf("the string has a period in it. \n");
            isDec = 1;
        }

        if(isdigit(str[i])){
            //printf("str[%d] val is: %c\n", i, str[i]);
            continue;
        }
    }
    
    if(isDec == 1 && numDec==1){
        //printf("the string is a decimal. \n");
        return 1;       //string is decimal value
    }
    else if(isDec == 0){
        //printf("the string is an int. \n");
        return 1;       //integer
    } else {
        return 0;
    }
}


//account linked list node creation method
bankAccount* createNode(char * token){
	bankAccount *temp;
	temp = malloc(sizeof(bankAccount));
	temp->next = NULL;
    temp->accountName = (char*)malloc(sizeof(char)*(strlen(token)+1));
    strcpy(temp->accountName, token);
    temp->balance = 0;
    temp->inSessionFlag = 0;
    pthread_mutex_init(&temp->lock, NULL);
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
}


void addClientNode(clientInfo** head, clientInfo * node){
    clientInfo *p = *head;
    if(*head == NULL){
        *head = node;
    }else{
        
        while(p->next != NULL){
            p = p->next;
        }
        p->next = node;
    }
}

//checks if the client is in a current session
int isInSession(int inSession, int sockfd){
    if (inSession==0){
        char w_buff[MAX];
        bzero(w_buff, MAX); 
        strcpy(w_buff,"* * * You are not currently in a service session * * *\n");
        write(sockfd, w_buff, sizeof(w_buff));
        return 0;
    } else {
        return 1;
    }
}

//writes given message to the client
void writeToClient(int sockfd, char* message){
    // printf("hey, were here\n");
    char w_buff[MAX];
    bzero(w_buff, MAX); 
    strcpy(w_buff,message);
    write(sockfd, w_buff, sizeof(w_buff));


    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if(sigaction(SIGINT, &sa, NULL) == -1){
	   perror("Sigaction Error");
	   exit(1);
    }
}

int acctExists(char* acctName){
    bankAccount *ptr = frontNode;
    while(ptr!=NULL){
        if(strcmp(ptr->accountName,acctName)==0){
            return 1;
        }
        ptr=ptr->next;
    }
    return 0;
}

bankAccount* findAcct(char* acctName){
    bankAccount *ptr= frontNode;
    while(ptr!=NULL){
        if(strcmp(ptr->accountName,acctName)==0){
            return ptr;
        }
        ptr=ptr->next;
    }
    return NULL;
}

//boolean method to check if account exists already in linked list
int alreadyExists(char *token){
	bankAccount *temp = frontNode;

	while(temp != NULL){
		int result = strcmp(temp->accountName, token);
		
		if(result == 0){
			// printf("Account name already exists!\n");
			return 1;
		}else{
			temp = temp->next;
		}
	}
	// printf("Account does not exist, continue creation!\n");
	return 0;	//account not found, continue to create new account
}

//print bankAccount structs as a test
void printBankAccounts(){
    // printf("NAME\tBALANCE\t\tSESSION FLAG\n");
    bankAccount *temp = frontNode;
	while(temp != NULL){
        if(temp->inSessionFlag==1)
            printf("%s\t$%lf\tIN SERVICE\n", temp->accountName, temp->balance);
		else
            printf("%s\t$%lf\n", temp->accountName, temp->balance);
        temp = temp->next;
	}
}

void printClientInfo(){
    printf("\n");
    printf("Client ID's and stuff:\n");
    clientInfo *temp = clientInfoFront;
    while(temp != NULL){
        printf("sockfd: %d\nthreadID: %lu\n\n", temp->sockfd, temp->threadID);
        temp = temp->next;
    }
}

//prints note in client, for debugging
void printNode(bankAccount *node){
    printf("\nAccount Name: %s\nBalance: $%lf\ninSessionFlag: %d\n\n", node->accountName, node->balance, node->inSessionFlag);      
}

int serveAcct(int sockfd, char * acctName, int inSession){
    // printf("Hey, in serve function\n");
    //first check if the given account exists, have a function that does that to be clean
    if(acctExists(acctName)==0){
        char buff[MAX];
        sprintf(buff,"* * * Error, cannot serve account that does not exist: [%s] * * *\n",acctName);
        writeToClient(sockfd, buff);
        return 0;
    } else {
        bankAccount *acct = findAcct(acctName);
        if( pthread_mutex_trylock(&(acct->lock)) != 0 ){ 
            writeToClient(sockfd, "* * * Error, this account is already being served * * *\n");
            return 0;
        }

        if(acct->inSessionFlag==1){
            writeToClient(sockfd,"* * * Error, this account is already in session * * *\n");
            return 0;
        } else {
            acct->inSessionFlag=1;
            writeToClient(sockfd,"* * * Successfully ENTERED service session * * *\n");
            printNode(acct);
            return 1;
        }
    }
}
void deposit(int sockfd, char* currAccount, int inSession, char * amount){
    // printf("Hey, in deposit function\n");
    if(isNumeric(amount)==0){
         writeToClient(sockfd,"* * * Error, amount entered must be NUMERIC * * *\n");
         return;
    } else { //number is numeric
        double amt = atof(amount);
        if (amount<0){
            writeToClient(sockfd,"* * * Error, amount entered must be POSITIVE * * *\n");
            return;
        }
        bankAccount *acct = findAcct(currAccount);
        acct->balance+=amt;
        char buff[MAX];
        sprintf(buff,"* * * Successfully deposited [$%lf] into your account * * *\n",amt);
        writeToClient(sockfd, buff);
    }
}
void withdraw(int sockfd, char* currAccount, int inSession, char * amount){
    // printf("Hey, in withdraw function\n");
    bankAccount *acct = findAcct(currAccount);
    if(isNumeric(amount)==0){
         writeToClient(sockfd,"* * * Error, amount entered must be NUMERIC * * *\n");
         return;
    } else { //number is numeric
        double amt = atof(amount);
        if (amount<0){
            writeToClient(sockfd,"* * * Error, amount entered must be POSITIVE * * *\n");
            return;
        } else if (amt > acct->balance){
            writeToClient(sockfd,"* * * Error, amount entered must LESS THAN OR EQUAL TO current balance * * *\n");
            return;
        }
        acct->balance-=amt;
        char buff[MAX];
        sprintf(buff,"* * * Successfully withdrew [$%lf] into your account * * *\n",amt);
        writeToClient(sockfd, buff);
    }
}
void query(int sockfd,  char* currAccount, int inSession){
    // printf("Hey, in query function\n");
    char buff[MAX];
    bankAccount *acct = findAcct(currAccount);
    sprintf(buff,"Balance: $%lf\n", acct->balance);
    writeToClient(sockfd,buff);
}
int end(int sockfd,  char* currAccount, int inSession){
    // printf("Hey, in end function\n");
    bankAccount *acct = findAcct(currAccount);
    acct->inSessionFlag=0;
    pthread_mutex_unlock(&(acct->lock));
    writeToClient(sockfd,"* * * Successfully ENDED service session * * *\n");
    
    return 0;   
}
void sigint_handler(int sig) {
	char buff[MAX];
	bzero(buff, MAX);
	strcpy(buff, "CtrlC");
    clientInfo * ptr = clientInfoFront;
    while (ptr!=NULL){
        write(ptr->sockfd, buff, sizeof(buff));
        ptr=ptr->next;
    }
	exit(0);
}

//Function designed for chat between client and server
void * func(void* args) { 
    // printf("pthread id : %lu\n", pthread_self());
    int sockfd;
    sockfd = *(int*)args;
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
    strcpy(currAccount," ");
   
    // writeToClient(sockfd, "");

    for (;;) { 
        bzero(r_buff, MAX); 

        // read the message from client and copy it in buffer
	    int status;
        status = read(sockfd, r_buff, sizeof(r_buff));
        // printf("status is: %d\n", status);
        if(status<=0){
            printf("Error reading from the Client, aborting this connection\n");
            if(isInSession(inSession, sockfd) == 1){
                writeToClient(sockfd, "* * * Implicitly closing the active account session * * *\n");
                inSession = end(sockfd, currAccount, inSession);
                strcpy(currAccount, " ");
            }
            printf("Client with threadID [%lu] has disconnected\n", pthread_self());
            writeToClient(sockfd, "* * * Closing this client connection * * *\n");
            close(sockfd);
            return;
        }

        int blah;
        ret = sem_wait(&sem);
        sem_getvalue(&sem, &blah);
        // printf("semval: %d\n", blah);

        // print buffer which contains the client contents
        printf("From client [%lu]: %s\n", sockfd, r_buff); 
         
        sscanf(r_buff, "%s %s",command, value);
        // printf("----------------\ncommand: %s\n", command);
        // printf("value: %s\n---------------\n\n", value );
    	
    	//if msg contains "create" then server will create an account
    	if (strcmp("create", command) == 0){
            
            if(isInSession(inSession, sockfd) == 1){
                 writeToClient(sockfd, "* * * Error, you cannot create an account while in a service session! * * *\n");
               ret = sem_post(&sem);
                continue;
            }
            pthread_mutex_lock(&openAccLock);
    		//handle create account
    		//ERR check: if account already created
    		//if <accountname> input = acct_name 
            if(strcmp(value, " ")==0||strcmp(value,"")==0){
                writeToClient(sockfd, "* * * Error, you must specify a name for your account! * * *\n");
                ret = sem_post(&sem);
                continue;
            }
    		if(numAccounts == 0){
                // printf("0 Accts: will create a NEW account!\n");
        	    bankAccount *node = createNode(value);
                addNode(&frontNode,node);    
    		    numAccounts++;
    		    // printf("Number of Accounts: %d\n", numAccounts);
                bzero(w_buff, MAX); 
                sprintf(w_buff,"* * * Account [%s] successfully added * * *\n", value);
                write(sockfd, w_buff, sizeof(w_buff));
                pthread_mutex_unlock(&openAccLock);
		    } else {
                if(alreadyExists(value) == 1) {
                    bzero(w_buff, MAX); 
                    strcpy(w_buff,"* * * Error, this account already exists! * * *\n");
                    write(sockfd, w_buff, sizeof(w_buff));
                    pthread_mutex_unlock(&openAccLock);
                    // printf("Error, this account already exists!\n");
                } else {
                	// printf("will create NEW account!!\n");
                    bankAccount *node = createNode(value);
            		addNode(&frontNode, node);
    				numAccounts++;	
    				// printf("Number of Accounts: %d\n", numAccounts);
                    bzero(w_buff, MAX); 
                    sprintf(w_buff,"* * * Account [%s] successfully added * * *\n", value);
                    write(sockfd, w_buff, sizeof(w_buff));
                    pthread_mutex_unlock(&openAccLock);
		    	}
                	
            }	     
        } else if (strcmp("serve", command) == 0){
             // ret = sem_wait(&sem);
             if(strcmp(value, " ")==0||strcmp(value,"")==0){
                writeToClient(sockfd, "* * * Error, you must specify SOME ACCOUNT * * *\n");
                ret = sem_post(&sem);
                continue;
            }
            // printf("will SERVE\n");
            if (strcmp(currAccount," ")!=0){
                writeToClient(sockfd, "* * * Error, you already have an account in session * * *\n");
            } else {
                inSession = serveAcct(sockfd,  value, inSession);
                if (inSession==1){
                    strcpy(currAccount,value);
                    printf("Successfully served, current acct serving is: [%s]\n",currAccount);
                } else if (inSession==0){
                    printf("Account not successully served\n");
                }
                
            }
        } else if (strcmp("deposit", command) == 0){
            // ret = sem_wait(&sem);
            if(strcmp(value, " ")==0||strcmp(value,"")==0){
                writeToClient(sockfd, "* * * Error, you must specify SOME AMOUNT * * *\n");
                ret = sem_post(&sem);
                continue;
            }
            if(isInSession(inSession, sockfd) == 1){
                // printf("will DEPOSIT\n");
                deposit(sockfd,  currAccount, inSession, value);
            }
        } else if (strcmp("withdraw", command) == 0){
              // ret = sem_wait(&sem);
              if(strcmp(value, " ")==0||strcmp(value,"")==0){
                writeToClient(sockfd, "* * * Error, you must specify SOME AMOUNT * * *\n");
                ret = sem_post(&sem);
                continue;
            }
             if(isInSession(inSession, sockfd) == 1){
                // printf("will WITHDRAW\n");
                withdraw(sockfd, currAccount, inSession, value);
            }
        } else if (strcmp("query", command) == 0){
            // ret = sem_wait(&sem); 
            if(isInSession(inSession, sockfd) == 1){
                // printf("will QUERY\n");
                query(sockfd,  currAccount, inSession);
            }
        } else if (strcmp("end", command) == 0){
            // ret = sem_wait(&sem);
            if(isInSession(inSession, sockfd) == 1){
                inSession = end(sockfd, currAccount, inSession);
                strcpy(currAccount, " ");
            }
        } else if(strcmp("quit", command) == 0){
            // printf("will QUIT\n");
    	   //printf("client quit, so check if other clients there\n");
            if(isInSession(inSession, sockfd) == 1){
                writeToClient(sockfd, "* * * Implicitly closing the active account session * * *\n");
                inSession = end(sockfd, currAccount, inSession);
                strcpy(currAccount, " ");
            }
            printf("Client with threadID [%lu] has disconnected\n", pthread_self());
            writeToClient(sockfd, "* * * Closing this client connection * * *\n");
            close(sockfd);
            ret = sem_post(&sem);
            return;
    	    // break;
	    } 

        memset(r_buff, 0, sizeof(r_buff));
        memset(w_buff, 0, sizeof(w_buff));
        memset(command, 0, sizeof(command));
        memset(value, 0, sizeof(value));
        ret = sem_post(&sem);
    }

}

//i think this is right???
void * printDiagnostics(void* args){
    
    while(1){
        ret = sem_wait(&sem); 
        if(numAccounts==0){
            printf("There are no accounts in the system yet\n");
        }else {
            printBankAccounts();
        }
        ret = sem_post(&sem); 
        sleep(15);
    }
    
}

//Driver function
int main(int argc, char *argv[]) { 
    numAccounts=0;
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

    getaddrinfo(0, argv[1] , &request, &result);

    if ((servSockFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol) ) < 0 ){
        fprintf(stderr, "ERROR: Server socket could not be created: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("** Socket successfully created\n");
    if(bind(servSockFD, result->ai_addr, result->ai_addrlen) < 0){
        printf("ERROR: Bind failed: %s\n", strerror(errno));
        exit(1);
    }

    printf("** Socket successfully binded\n");
    int optval = 1;
    setsockopt(servSockFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    //setsockopt(servSockFD, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)); //based on his ex in class
										   //should fix time lockout with binding
    pthread_mutex_init(&openAccLock, NULL);
    pthread_mutex_init(&clientInfoLock, NULL);


    /* initialize a private semaphore */
    pshared = 0;
    value = 1;
    ret = sem_init(&sem, pshared, value); 


    listen(servSockFD,10); // ten connections can be queued
   

    printf("** Begin listening for connections\n");
    void* clientSocketThreadArg;
    pthread_t clientID;
    pthread_t printDiag;


    //thread to do all da diagnostic shiz
    if (pthread_create(&printDiag, NULL, &printDiagnostics, NULL) != 0){
            printf("ERROR: Can't create new diagnostic thread:%s\n", strerror(errno));
            exit(EXIT_FAILURE);
    }

    //infinite loop to accept connections and spawn threads
    while(1){
        clientInfo * node = malloc(sizeof(clientInfo));

        if( (clientSockFD = accept(servSockFD, NULL, NULL)) < 0){
            printf("ERROR: failed to accept socket: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        node->sockfd = clientSockFD;

        clientSocketThreadArg = (void*)malloc(sizeof(int));
        memcpy(clientSocketThreadArg, &clientSockFD, sizeof(int));

        // create thread for client - service
        if (pthread_create(&clientID, NULL, &func, clientSocketThreadArg) != 0){
            printf("ERROR: Can't create new client thread: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        node->threadID = clientID;
        if (pthread_detach(clientID) != 0){
            printf("ERROR: Could not detach new client thread: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        addClientNode(&clientInfoFront,node);  
        printf("**Accepted a connection and detached a thread, sockfd: [%lu]\n", clientSocketThreadArg);
        // printClientInfo();
    }

    return 0;

}
