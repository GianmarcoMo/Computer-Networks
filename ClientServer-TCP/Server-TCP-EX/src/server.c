/*
 ============================================================================
 Name        : TCP-Client-Server.c
 Author      : Gianmarco Moresi
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


int comunicazione(int, int );
void esci(int);

void ClearWinSock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(int argc, char *argv[]) {
	#if defined WIN32 // initialize Winsock
	WORD wVersionRequested = MAKEWORD(2,2);
	WSADATA wsaData;
	int wsastartup;
	wsastartup = WSAStartup(wVersionRequested, &wsaData);
	if (wsastartup != 0) printf("Errore WSAStartup()\n");
	#endif

	int socketServer;
	socketServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socketServer < 0){
		printf("Creazione socket fallita!");
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}else{
		puts("Creazione socket eseguita");
	}

	//	BIND FUNCTION
	struct sockaddr_in sockAdd;
	sockAdd.sin_family = AF_INET;
	sockAdd.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	sockAdd.sin_port = htons( 27015 );

	if (bind(socketServer, (struct sockaddr*) &sockAdd, sizeof(sockAdd)) <0) {
		puts("Binding fallito!\n");
		closesocket(socketServer);
		ClearWinSock();
		return -1;
	}else{
		puts("binding avviato!");
	}
	//	----------------------------------------------------

	//	FUNZIONE ASCOLTO
	int qlen = 6;
	if (listen (socketServer, qlen) < 0) {
		puts("Ascolto fallito\n");
		closesocket(socketServer);
		ClearWinSock();
		return -1;
	}else{
		puts("In attesa di un client");
	}
	//	----------------------------------------------------

	//	FUNZIONE ACCETTAZIONE CLIENT
	struct sockaddr_in cad;
	int clientSocket;
	int clientLen;
	while (1) {
		puts( "Aspettando connessione client...");
		clientLen = sizeof(cad);
		if ( (clientSocket=accept(socketServer, (struct sockaddr *)&cad, &clientLen)) < 0) {
			puts("Accettazione fallita\n");
			closesocket(socketServer);
			ClearWinSock();
			return -1;
		}
		printf( "Connesione client: %s\n", inet_ntoa(cad.sin_addr) );

		while(1){
			if(comunicazione(clientSocket, socketServer) != 0)
				break;
		}

	}// end of the while loop
}


int comunicazione(int clientSocket, int socketServer ){
	//--------------------------------------------
	// FUNZIONE DI RICEZIONE DEL MESSAGGIO
	int bytesRcvd;
	int totalBytesRcvd = 0;
	char buf[512];

	if ((bytesRcvd = recv(clientSocket, buf, 1024 - 1, 0)) <= 0) {
		puts("Connessione chiusa dal client o ricezione fallita");
		return -1;
	}else{
		if(strcmp(buf,"esci") == 0){
			printf("Client disconnesso.\n");
			return -1;
		}else{
			totalBytesRcvd += bytesRcvd;
			buf[bytesRcvd] = '\0';
			printf("Client: %s\n", buf);
		}
	}

	//--------------------------------------------
	//	FUNZIONE DI INVIO DEL MESSAGGIO
	printf("Digita messaggio: ");
	char inputServer[50];
	scanf("%s", inputServer);

	int stringLen = strlen(inputServer);
	if (send(clientSocket, inputServer, stringLen, 0) != stringLen) {
		puts("Messaggio troppo grande");
		system("pause");
		closesocket(socketServer);
		ClearWinSock();
		return -1;
	}
	if(strcmp(inputServer,"esci") == 0){
		esci(socketServer);
	}
	puts("");
	//--------------------------------------------
	return 0;
}

void esci(int socketServer){
	// CHIUSURA DELLA CONNESSIONE
	closesocket(socketServer);
	ClearWinSock();
}
