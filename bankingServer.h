#ifndef BANKINGSERVER_H
#define BANKINGSERVER_H


typedef struct bankAccount {
    char *accountName;
    double balance;   
    int inSessionFlag;
    struct bankAccount *next;
    pthread_mutex_t lock;
} bankAccount;



int isNumeric(char *str);
bankAccount* createNode();
void addNode(bankAccount **head, bankAccount * node);
int isInSession(int inSession, int sockfd);
void writeToClient(int sockfd, char* message);
int acctExists(char* acctName);
int alreadyExists(char *token);
void printLL();
void printNode(bankAccount *node);

int serveAcct(int sockfd, char * acctName, int inSession);
void deposit(int sockfd, char* currAccount, int inSession, char *amount);
void withdraw(int sockfd,  char* currAccount, int inSession, char *amount);
void query(int sockfd,  char* currAccount, int inSession);
int end(int sockfd, char* currAccount, int inSession);
//method prototypes in Server


void *func(void* args);




#endif










