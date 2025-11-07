#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

bool reciveAll(SOCKET sock, char *data, int length) {
  int totalReceived = 0;
  while (totalReceived < length) {
    int receive = recv(sock, data + totalReceived, length - totalReceived, 0);
    if (receive <= 0)
      return false; // disconnected or error
    totalReceived += receive;
  }
  return true;
}

void reciveFiles(SOCKET serverSocket) {

  std::cout << "- Waiting for clients to send..." << std::endl;

  // Receive file name length
  int netLength = 0;
  if (!reciveAll(serverSocket, (char *)&netLength, sizeof(netLength))) {
    std::cout << "- Receiving file name length failed" << std::endl;
    return;
  }
  int fileNameLength = ntohl(netLength); // convert to host byte order

  // Receive file name
  if (fileNameLength <= 0 || fileNameLength >= 512) {
    std::cout << "- Invalid file name length: " << fileNameLength << std::endl;
    return;
  }

  char fileName[512];
  if (!reciveAll(serverSocket, fileName, fileNameLength)) {
    std::cout << "- Receiving file name failed" << std::endl;
    return;
  }
  fileName[fileNameLength] = '\0';

  std::cout << "[ FILE ]: \"" << fileName << "\" received successfully"
            << std::endl;
}

bool sendAll(SOCKET sock, const char *data, int length) {
  int totalSent = 0;
  while (totalSent < length) {
    int sent = send(sock, data + totalSent, length - totalSent, 0);
    if (sent <= 0)
      return false; // disconnected or error
    totalSent += sent;
  }
  return true;
}

void sendFiles(SOCKET serverSocket) {

  // Ask for file path
  std::string path;
  std::cout << "- Please enter file path: ";
  std::getline(std::cin, path);

  // Extract file name
  size_t fileNameIndex = path.find_last_of("\\/");
  std::string fileName = path.substr(fileNameIndex + 1);

  // Send file name length in network byte order
  int fileNameLength = fileName.size();
  int netLength = htonl(fileNameLength);
  if (!sendAll(serverSocket, (char *)&netLength, sizeof(netLength))) {
    std::cout << "- Failed to send file name length.\n";
    return;
  }

  // Send the file name itself
  if (!sendAll(serverSocket, fileName.c_str(), fileNameLength)) {
    std::cout << "- Failed to send file name.\n";
    return;
  }

  std::cout << "[ FILE ]: \"" << fileName << "\" sent successfully.\n";
}

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
  std::cout << "- Connected to server" << std::endl;
  std::string msg;
  while (true) {
    std::getline(std::cin, msg);
    if (msg == "exit") {
      std::cout << "- Client closed sucessfully" << std::endl;
      break;
    } else if (msg == "receive") {
      reciveFiles(clientSocket);
    } else if (msg == "send") {
      sendFiles(clientSocket);
    }
  }
  closesocket(clientSocket);
  WSACleanup();
  return 0;
}