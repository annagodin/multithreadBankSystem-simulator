#ifndef BANKINGSERVER_H
#define BANKINGSERVER_H


typedef struct bankAccount {
    char *accountName;
    double balance;   
    int inSessionFlag;
    struct bankAccount *next;
    pthread_mutex_t lock;

} bankAccount;


//method prototypes in Server
bankAccount* createNode();
void createAccount(char* token);
void addNode(bankAccount **head, bankAccount * node);
int alreadyExists(char *token);
void printLL();
void func(int sockfd);


int serveAcct(int sockfd, bankAccount *frontNode, char * acctName, int inSession);
void deposit(int sockfd, bankAccount *frontNode, char* currAccount, int inSession);
void withdraw(int sockfd, bankAccount *frontNode, char* currAccount, int inSession);
void query(int sockfd, bankAccount *frontNode, char* currAccount, int inSession);
int end(int sockfd, bankAccount *frontNode, char* currAccount, int inSession);


#endif










