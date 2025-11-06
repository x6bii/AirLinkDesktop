#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void reciveMessages(SOCKET serverSocket) {
  char buffer[512];
  while (true) {
    int bytesRecived = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRecived <= 0) {
      std::cout << "Disconnected from server" << std::endl;
      break;
    };
    buffer[bytesRecived] = '\0';
    std::cout << "Server says: " << buffer << std::endl;
  };
};

int main() {
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "WSAStartup failed" << std::endl;
    return 1;
  }
  SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket == INVALID_SOCKET) {
    std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return 1;
  }
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(54000);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
  if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
      SOCKET_ERROR) {
    std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
    closesocket(clientSocket);
    WSACleanup();
    return 1;
  }
  std::cout << "Connected to server" << std::endl;
  std::thread reciveThread(reciveMessages, clientSocket);
  reciveThread.detach();
  std::string msg;
  while (true) {
    std::getline(std::cin, msg);
    if (msg == "exit") {
      break;
    }
    send(clientSocket, msg.c_str(), msg.size(), 0);
  }
  closesocket(clientSocket);
  WSACleanup();
  return 0;
}