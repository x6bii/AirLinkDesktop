#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

std::vector<SOCKET> clients;
std::mutex clientsMutex;

int recvAll(SOCKET sock, char *buffer, int length) {
  int total = 0;
  while (total < length) {
    int bytes = recv(sock, buffer + total, length - total, 0);
    if (bytes <= 0)
      return bytes; // disconnected or error
    total += bytes;
  }
  return total;
}

void handleClient(SOCKET client) {
  while (true) {
    // Receive file name length
    int netLength;
    if (recvAll(client, (char *)&netLength, sizeof(netLength)) <= 0)
      break;
    int fileNameLength = ntohl(netLength);

    // Receive file name
    char *fileName = new char[fileNameLength + 1];
    if (recvAll(client, fileName, fileNameLength) <= 0) {
      delete[] fileName;
      break;
    }
    fileName[fileNameLength] = '\0';

    std::cout << "[ FILE ]: " << fileName << " received" << std::endl;

    // Broadcast to other clients
    int netLengthToSend = htonl(fileNameLength);
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (SOCKET c : clients) {
      if (c != client) {
        send(c, (char *)&netLengthToSend, sizeof(netLengthToSend), 0);
        send(c, fileName, fileNameLength, 0);
      }
    }

    delete[] fileName;
  }

  // Remove client
  std::lock_guard<std::mutex> lock(clientsMutex);
  clients.erase(std::remove(clients.begin(), clients.end(), client),
                clients.end());
  closesocket(client);
}
