#include <iostream>
#include <winsock2.h>

int main() {

  // initialize winSock
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    std::cout << "WsaStartup Failed: " << result << std::endl;
    WSACleanup();
    return 1;
  }

  // Create server socket
  SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket == INVALID_SOCKET) {
    std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return 1;
  }

  // Create server adress
};