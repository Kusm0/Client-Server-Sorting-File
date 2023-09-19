#ifdef _WIN32

#include <iostream>
#include <winsock2.h>
#include <string>
#include "filesort_win.h"

#pragma comment(lib, "ws2_32.lib")

int main() {

    // Ініціалізація Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Створення сокету
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Задання параметрів сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Прив'язка сокету до порту
    result = bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to bind: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Прослуховування вхідних з'єднань
    result = listen(serverSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // Прийом вхідних з'єднань та обробка клієнтів
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    char buffer[4096];
    std::string response;

    // Надіслати інструкцію клієнту
    const char *instructions = "Welcome to the server! You can send commands:\n"
                               "Choose sorting option: \n1 - by name,\n2 - by creation date,\n3 - by file type\n"
                               "exit - to quit\n";
    result = send(clientSocket, instructions, strlen(instructions), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
    }

    std::string sortResponse;

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // Прийом даних від клієнта
        result = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            break;
        }

        std::cout << "Received message from client: " << buffer << std::endl;

        // Перевірка, чи клієнт відправив "exit"
        if (strcmp(buffer, "exit") == 0) {
            std::cout << "Client requested to exit." << std::endl;
            break;
        }

        // Запит на сортування

        if (strcmp(buffer, "1") == 0) {
            std::cout << "Client want to sort BY NAME" << std::endl;
            sortResponse = execute(1); // Функція сортування з filesort
        }
        if (strcmp(buffer, "2") == 0) {
            std::cout << "Client want to sort BY CREATION DATE" << std::endl;
            sortResponse = execute(2);
        }
        if (strcmp(buffer, "3") == 0) {
            std::cout << "Client want to sort BY FILE TYPE" << std::endl;
            sortResponse = execute(3);
        }
        std::cout << sortResponse;
        result = send(clientSocket, sortResponse.c_str(), sortResponse.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }


    }

    // Закриття сокетів та очищення Winsock

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

#endif