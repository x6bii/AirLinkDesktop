#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void receiveFiles(SOCKET serverSocket) {

  std::cout << "- Waiting for clients to send..." << std::endl;

  // Receive file name length
  int netLength = 0;
  if (!recv(serverSocket, (char *)&netLength, sizeof(netLength), 0)) {
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
  if (!recv(serverSocket, fileName, fileNameLength, 0)) {
    std::cout << "- Receiving file name failed" << std::endl;
    return;
  }
  fileName[fileNameLength] = '\0';

  std::cout << "[ FILE ]: \"" << fileName << "\" received successfully"
            << std::endl;
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
  if (!send(serverSocket, (char *)&fileNameLength, sizeof(fileNameLength), 0)) {
    std::cout << "- Failed to send file name length.\n";
    return;
  }

  // Send the file name itself
  if (!send(serverSocket, fileName.c_str(), fileNameLength, 0)) {
    std::cout << "- Failed to send file name.\n";
    return;
  }

  // Open the file and reads it in binary mode
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::cout << "- Failed to open the file " << fileName << std::endl;
    return;
  }

  // Send file size (in bytes)
  file.seekg(0, std::ios::end);
  int fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  send(serverSocket, (char *)&fileSize, sizeof(fileSize), 0);

  // Send file to server in binary
  char buffer[4096];
  while (!file.eof()) {
    file.read(buffer, sizeof(buffer));
    int bytesRead = file.gcount();
    send(serverSocket, buffer, bytesRead, 0);
  }
  std::cout << "[ FILE ]: \"" << fileName << "\" sent successfully to server\n";
  file.close();
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
      receiveFiles(clientSocket);
    } else if (msg == "send") {
      sendFiles(clientSocket);
    } else {
      std::cout << "- Invalid command please use (send, receive or exit)"
                << std::endl;
    }
  }
  closesocket(clientSocket);
  WSACleanup();
  return 0;
}