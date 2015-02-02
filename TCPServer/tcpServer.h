#ifndef POINT_H
#define POINT_H

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <string>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1234"
#define ACTIVATION_BOUTON "activation"
#define RETOUR_BOUTON "retour"

using namespace std;

class TcpServer {
public:
	// Constructeur
	TcpServer();
	bool TcpServer::createServer();
	SOCKET TcpServer::waitForClient();
	void TcpServer::listenClient(SOCKET clientSocket);
	bool TcpServer::sendMessage(SOCKET clientSocket, int identifiant, char* action);
	void TcpServer::deconnecterClient(SOCKET clientSocket);
	void TcpServer::shutdownServer();
private:
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket;

	struct addrinfo *result;
	struct addrinfo hints;

	int iSendResult;

};


#endif