#include <fstream>
#include <iostream>
#include <network.h>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "- WSAStartup failed" << std::endl;
    return 1;
  }
  SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket == INVALID_SOCKET) {
    std::cerr << "- Socket creation failed : " << WSAGetLastError()
              << std::endl;
    WSACleanup();
    return 1;
  }
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(54000);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
  while (true) {
    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {
      std::cout << "- Server not available, retrying in 1 second...\n";
      Sleep(1000);
      continue;
    }
    break;
  }
  std::cout << "- Connected to server" << std::endl;
  std::string msg;
  while (true) {
    std::getline(std::cin, msg);
    if (msg == "exit") {
      std::cout << "- Client closed sucessfully" << std::endl;
      break;
    } else if (msg.rfind("receive ", 0) == 0) {
      std::string path = msg.substr(8);
      receiveFiles(clientSocket, path);
    } else if (msg.rfind("send ", 0) == 0) {
      std::string path = msg.substr(5);
      sendFiles(clientSocket, path);
    } else {
      std::cout << "- Invalid command please use (send, receive or exit)"
                << std::endl;
    }
  }
  closesocket(clientSocket);
  WSACleanup();
  return 0;
}