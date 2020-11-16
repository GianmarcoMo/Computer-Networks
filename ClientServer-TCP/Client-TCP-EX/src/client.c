/*
 * client.c
 *
 *  Created on: Nov 10, 2020
 *      Author: Gianmarco
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

int comunicazione(int);
void esci(int);
void ClearWinSock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(int argc, char *argv[]) {
	#if defined WIN32 // initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		puts("Error at WSAStartup()\n");
		return 0;
	}
	#endif

	//	Creazione socket client
	int socketClient;
	socketClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socketClient < 0){
		puts("Creazione socket fallita!");
		system("pause");
		closesocket(socketClient);
		WSACleanup();
		return -1;
	}
	//	--------------------------------

	struct sockaddr_in socClientAdr;
	socClientAdr.sin_family = AF_INET;
	socClientAdr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	socClientAdr.sin_port = htons( 27015 );

	//	Connessione
	if (connect(socketClient,(struct sockaddr *) &socClientAdr, sizeof(socClientAdr)) < 0){
		puts( "Connessione fallita.\n" );
		system("pause");
		ClearWinSock();
		return -1;
	}

	while(1)
		if(comunicazione(socketClient) != 0)
			break;

	return(0);
}

int comunicazione(int socketClient){
	//	FUNZIONE DI INVIO DEL MESSAGGIO
	printf("Digita messaggio: ");
	char inputClient[50];
	scanf("%s", inputClient);

	int stringLen = strlen(inputClient);
	if (send(socketClient, inputClient, stringLen, 0) != stringLen) {
		puts("Messaggio troppo grande");
		system("pause");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}
	if(strcmp(inputClient,"esci") == 0){
		esci(socketClient);
		return -1;
	}
	puts("");

	//--------------------------------------------
	// FUNZIONE DI RICEZIONE DEL MESSAGGIO
	int bytesRcvd;
	int totalBytesRcvd = 0;
	char buf[512];
	if ((bytesRcvd = recv(socketClient, buf, 1024 - 1, 0)) <= 0) {
		puts("Connessione chiusa o ricezione fallita");
		system("pause");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}
	totalBytesRcvd += bytesRcvd;
	buf[bytesRcvd] = '\0';
	if(strcmp(buf,"esci") == 0){
		esci(socketClient);
		printf("Server disconnesso.\n\n");
		system("pause");
		return -1;
	}
	printf("Server: %s\n", buf);
	//--------------------------------------------
	return 0;
}

void esci(int socketClient){
	// CHIUSURA DELLA CONNESSIONE
	closesocket(socketClient);
	ClearWinSock();
}
