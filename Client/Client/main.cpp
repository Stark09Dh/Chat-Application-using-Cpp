#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<thread>
#include<string>


using namespace std;

#pragma comment(lib , "ws2_32.lib")

/*
 -> initialize winsock
 -> create socket 
 -> connect to socket
 ->send / recieve
 ->close socket


*/
bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s) {
	cout << " Enter Your Chat Name : ";
	string name;
	cin >> name;
	string message;

	while (true) {
		getline(cin, message);
		string msg = name + " : " + message;
		int byteSend = send(s, msg.c_str(), msg.length(), 0);
		if (byteSend == SOCKET_ERROR) {
			cout << "Error Sending Message" << endl;
			break;
		}
		if (message == "exit") {
			cout << "Stopping the Application" << endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}

void RecieveMsg(SOCKET s) {
	char buffer[4096];
	int recvlength;
	string msg = "";
	while (true) {
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			cout << "Disconnected from the Server" << endl;
			break;
		}
		else {
			msg = string(buffer, recvlength);
			cout << msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();
	
}


int main() {
	if (!initialize()) {
		cout << "Initialize winsock Failed" << endl;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "invalid socket created" << endl;
		return 1;
	}
	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &serveraddr.sin_addr);

	
	

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Server not able to connect to server" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connected to server" << endl;

	thread senderThread(SendMsg, s);
	thread reciever(RecieveMsg, s);

	senderThread.join();
	reciever.join();


	cout << "Press Enter to exit..." << endl;
	cin.get();
	return 0;
}