DEAD CODE



//wrote createAccount() as a linked list method addNode()
create account function
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




	Just seeing what happens when commenting this out
      //  I personally don't think server needs to have the ability to write back?
      //  we can discuss this!
	
	printf("Write a message to client: ");
        bzero(buff, MAX); 
        n = 0;
         // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');
        buff[strlen(buff)]='\0';
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff));
        printf("Sent message to client: '%s'\n",buff);

