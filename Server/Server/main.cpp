#include<iostream>
#include<WinSock2.h>
#include <ws2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

/*
 -> initialize winsock library 
 -> create the Socket 
 -> get ip and port
 -> bind the ip/port wiht the socket
 -> listen on the socket
 -> accept
 -> recieve and send

*/
bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}


void InteractWithClient(SOCKET clientSocket, vector<SOCKET>&clients, int &number) {
	// send - recv client

	cout << " Client "<<number <<" Connected " << endl;

	char buffer[4096];

	while (true) {
		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesrecvd <= 0) {
			cout << "Client Disconnected" << endl;
			break;
		}

		string msg(buffer, bytesrecvd);

		cout << "Message from : " << msg << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, msg.c_str(), msg.length(), 0);
			}
		}
		auto it = find(clients.begin(), clients.end(), clientSocket);

		if (it != clients.end()) {
			clients.erase(it);
		}
	}

	closesocket(clientSocket);
}

int main() {

	if (!initialize()) {
		cout << "winsock initialization Failed" << endl;
	}

	cout << "Server Program" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET) {
		cout << " Socket creation failed" << endl;
		return 1;
	}

	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	// convert IP address to binary form and put it in sim_family
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "Setting address structure Failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Bind Failed";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << " Listen failed";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server has started listening on port : " << port << endl;
	vector<SOCKET>clients;
	int number = 0;
	while (true) {
		// accept 
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			cout << "Invalid Client Socket" << endl;
		}
		clients.push_back(clientSocket);
		number++;
		

		thread t1(InteractWithClient, clientSocket , std::ref(clients),std::ref(number));
		t1.detach();
	}

	closesocket(listenSocket);

	WSACleanup();

	cout << "Press Enter to exit..." << endl;
	cin.get();
	return 0;
}