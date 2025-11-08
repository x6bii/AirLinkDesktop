#include "Handle_Client.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {

  // initialize winSock
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    std::cout << "[ WINSOCK2 ] : WsaStartup Failed: " << result << std::endl;
    WSACleanup();
    return 1;
  }

  // Create server socket
  SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket == INVALID_SOCKET) {
    std::cout << "[ WINSOCK2 ] : Socket creation failed: " << WSAGetLastError()
              << std::endl;
    WSACleanup();
    return 1;
  }

  // Create server address
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(54000);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  // Bind address
  if (bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) ==
      SOCKET_ERROR) {
    std::cout << "[ SERVER ] : Binding socket error: " << WSAGetLastError()
              << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    return 1;
  }

  // Listen to clients
  if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
    std::cout << "[ SERVER ] : Error listening to: " << WSAGetLastError()
              << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    return 1;
  } else {
    std::cout << "[ SERVER ] : Server listening on port 54000\n";
  }

  // Accept a client connection
  while (true) {
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
      std::cout << "[ CLIENT ] : Accept failed: " << WSAGetLastError()
                << std::endl;
      continue;
    }
    {
      std::lock_guard<std::mutex> lock(clientsMutex);
      clients.push_back(clientSocket);
    }
    std::thread(handleClient, clientSocket).detach();
    std::cout << "[ CLIENT ] : New client connected!\n";
  }

  closesocket(serverSocket);
  WSACleanup();
  return 0;
};