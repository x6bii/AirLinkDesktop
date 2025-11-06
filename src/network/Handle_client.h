#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

std::vector<SOCKET> clients;
std::mutex clientsMutex;

void handleClient(SOCKET client) {
  char buffer[512];
  while (true) {
    int bytesReceived = recv(client, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
      std::cout << "Client disconnected" << std::endl;
      break;
    }
    buffer[bytesReceived] = '\0';
    std::cout << "Client says: " << buffer << std::endl;
  }

  {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client),
                  clients.end());
  }
  closesocket(client);
}