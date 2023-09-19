#ifdef __unix__

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "filesort_linux.h"

int main() {
    // Створення сокету
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Задання параметрів сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Прив'язка сокету до порту
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Прослуховування вхідних з'єднань
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // Прийом вхідних з'єднань та обробка клієнтів
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == -1) {
        std::cerr << "Accept failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    char buffer[4096];
    std::string response;

    // Надіслати інструкцію клієнту
    const char* instructions = "Welcome to the server! You can send commands:\n"
                               "Choose sorting option: \n1 - by name,\n2 - by creation date,\n3 - by file type\n"
                               "exit - to quit\n";
    ssize_t result = send(clientSocket, instructions, strlen(instructions), 0);
    if (result == -1) {
        std::cerr << "Send failed" << std::endl;
    }

    std::string sortResponse;

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // Прийом даних від клієнта
        result = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (result == -1) {
            std::cerr << "Receive failed" << std::endl;
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
            sortResponse = execute(1);
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
        if (result == -1) {
            std::cerr << "Send failed" << std::endl;
            break;
        }
    }

    // Закриття сокетів
    close(clientSocket);
    close(serverSocket);

    return 0;
}
#endif