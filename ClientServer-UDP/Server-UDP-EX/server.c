/*
 * server.c
 *
 *      Author: Gianmarco Moresi
 */

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa.inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ECHOMAX 255
#define PORT 48000

void errore(char * messaggioInput){
	printf(messaggioInput);
}

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}

int main(int argc, char *argv[])  {
	#if defined WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
		if (iResult != 0) {
			printf ("Errore con WSAStartup\n");
			return EXIT_FAILURE;
		}
	#endif

	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
    int cliAddrLen;
	char echoBuffer[ECHOMAX];
	int recvMsgSize;

	// Creazione socket
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		errore("Creazione socket fallita! ");
	}

	// Costruzione dell'indirizzo
	memset(&echoServAddr, 0, sizeof(echoServAddr));

	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_port = htons(PORT);
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// Binding del socket
	if ((bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
		errore("Errore nel binding");


	// Ricezione del messaggio da parte del client
	while(1) {
		puts("In attessa della connessione del client...");
		cliAddrLen = sizeof(echoClntAddr);
		recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&echoClntAddr, &cliAddrLen);

		printf("Connessione del client: %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("Client invia: %s\n", echoBuffer);

		// RINVIA LA STRINGA ECHO AL CLIENT
		if (sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
			errore("Messaggio con bytes superiore di quelli aspettati.");
	}
}
