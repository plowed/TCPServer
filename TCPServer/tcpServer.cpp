//============================================================================
// Name        : tcpServ.cpp
// Author      : Damien Druel
// Version     : 1.1
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "tcpServer.h"

TcpServer::TcpServer(){

	ListenSocket = INVALID_SOCKET;
	result = NULL;

}

bool TcpServer::createServer(){

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

}

SOCKET TcpServer::waitForClient(){

	printf("en attente d'un client");

	// Accept a client socket
	SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return NULL;
	}
	printf("\n client connecté");

	return ClientSocket;

}


void TcpServer::listenClient(SOCKET clientSocket){

	string reset("reset");

	// Receive until the peer shuts down the connection
	do {

		printf("\n ---- en attente d'un message ---- \n");
		vector<char> recvData(DEFAULT_BUFLEN);

		iResult = recv(clientSocket, recvData.data(), recvData.size() - 1, 0);

		if (iResult > 0) {
			printf("Message reçu : ");
			cout << recvData.data() << endl;


			if (reset.compare(recvData.data()) == 0){


				if (!sendMessage(clientSocket, 2, RETOUR_BOUTON))
					return;
				if (!sendMessage(clientSocket, 1, RETOUR_BOUTON))
					return;
				if (!sendMessage(clientSocket, 0, RETOUR_BOUTON))
					return;


			}
			else{
				int identifiant = atoi(recvData.data());

				//activer edit mode pour 'identifiant'

				if (!sendMessage(clientSocket, identifiant, ACTIVATION_BOUTON))
					return;
			}

		}
		else if (iResult == 0){
			printf("Connection closing...\n");
			deconnecterClient(clientSocket);
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			deconnecterClient(clientSocket);
		}

	} while (iResult > 0);


}

void TcpServer::deconnecterClient(SOCKET clientSocket){

	printf("\n on déconnecte le client, fin de transmission\n");

	// shutdown the connection since we're done
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return;
	}
	// cleanup
	closesocket(clientSocket);
	WSACleanup();

}



bool TcpServer::sendMessage(SOCKET clientSocket, int identifiant, char* action){

	char message[DEFAULT_BUFLEN];

	wsprintf(message, "%d/%s\n", identifiant, action);


	// Echo the buffer back to the sender
	iSendResult = send(clientSocket, message, (int)strlen(message), 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		deconnecterClient(clientSocket);
		return false;
	}
	printf("On envoi : %s", message);
	return true;

}

void TcpServer::shutdownServer(){

	closesocket(ListenSocket);
	ListenSocket = INVALID_SOCKET;
	result = NULL;

}

int main(void)
{

	TcpServer serveur;
	if (serveur.createServer()){
		SOCKET client = serveur.waitForClient();
		if (client != NULL){
			serveur.listenClient(client);
		}
	}

	return 0;
}
