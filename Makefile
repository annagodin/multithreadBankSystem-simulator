all: bankingClient bankingServer bankingClient.c bankingServer.c

bankingClient: bankingClient.c bankingClient.o
	gcc -pthread -Wall -Werror=implicit-function-declaration -o bankingClient bankingClient.o
bankingServer: bankingServer.c bankingServer.o
	gcc -pthread -Wall -Werror=implicit-function-declaration -o bankingServer bankingServer.o
clean:
	rm -f bankingServer bankingClient
