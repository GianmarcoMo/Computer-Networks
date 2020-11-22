/*
 * client.c
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
	struct sockaddr_in fromAddr;
	int fromSize;
	char echoString[ECHOMAX];
	char echoBuffer[ECHOMAX];
	int echoStringLen;
	int respStringLen;

	printf("Digita il messaggio da inviare al server: ");
	scanf("%s", echoString);

	while((echoStringLen = strlen(echoString)) > ECHOMAX){
		errore("Messaggio troppo lungo.");
		printf("\n Digita il messaggio: ");
		scanf("%s", echoString);
	}


	// Creazione socket
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		errore("Creazione socket fallita! ");
	}

	// Costruzione dell'indirizzo
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_port = htons(PORT);
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// RINVIA LA STRINGA ECHO AL CLIENT
	if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != echoStringLen)
		errore("Messaggio con bytes superiore di quelli aspettati.");

	// RITORNO DELLA STRINGA ECHO
	fromSize = sizeof(fromAddr);
	respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr){
		fprintf(stderr, "Error: received a packet from unknown source.\n");
		exit(EXIT_FAILURE);
	}

	echoBuffer[respStringLen] = '\0';

	printf("Messaggio server: %s\n", echoBuffer);
	closesocket(sock);
	ClearWinSock();

	system("pause");

	return EXIT_SUCCESS;
}
