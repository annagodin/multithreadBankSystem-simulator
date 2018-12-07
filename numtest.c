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


int main(int argc, char *argv[]) { 
    int isNum = isNumeric(argv[1]);
    if(isNum==1){
        printf("woooooo its a number!!\n");
    } else {
        printf("not a number bitch\n");
    }
    return 0;
}