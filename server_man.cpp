//server arch   |   client - server

#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main() {

	// setup
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	// check for startup proper
	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0) { // bad start
		cerr << "Can't initialize WINSOCK! See ya" << endl;;
		return 0;
	}


	// socket creation. all hail the socket lord
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) { // bad listener
		cerr << "Can't create no socket. See ya" << endl;
		return 0;
	}


	// bind the socket. like a leather book
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); // Host TO Network Short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint)); // binding the socket


	// tell winsock the socket is for listen
	listen(listening, SOMAXCONN); // max connection ~ 15


	// wait for connection
	sockaddr_in client;
	int clientsize = sizeof(client);


	// can check for invalid socket
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize); // accept connection

	char host[NI_MAXHOST]; // client's remotoe name
	char service[NI_MAXSERV]; // service client1 is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}


	// close listening socket
	closesocket(listening);


	// message echoing
	char buff[4096]; // up to 4096 characters

	while (true) {

		ZeroMemory(buff, 4096); // reset the message

		int bytesRecieved = recv(clientSocket, buff, 4096, 0);

		if (bytesRecieved == SOCKET_ERROR) { // too much or something
			cerr << "Error in recv(). See ya" << endl;
			break;
		}

		if (bytesRecieved == 0) { // no more data transmitting
			cout << "Client disconnected." << endl;
			break;
		}


		// handling client1 messages
		
		if (bytesRecieved != 2) { // 2 bytes == nothing sent
			cout << "Client: " << buff << endl; // repeat message to server
			//cout << "   |   Bytes Received = " << bytesRecieved << endl;

			send(clientSocket, buff, bytesRecieved, 0); // acknowledge & repeat message
		}
	}


	// close socket
	closesocket(clientSocket);
	WSACleanup();


	return 0;
}
