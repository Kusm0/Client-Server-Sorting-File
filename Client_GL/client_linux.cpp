#ifdef __unix__

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Створення сокету
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Задання параметрів сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr)) != 1) {
        std::cerr << "Invalid address" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Підключення до сервера
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect" << std::endl;
        close(clientSocket);
        return 1;
    }

    char buffer[4096];
    std::string userInput;

    // Очікування інструкцій від сервера
    memset(buffer, 0, sizeof(buffer));
    ssize_t result = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (result == -1) {
        std::cerr << "Receive failed" << std::endl;
    } else {
        std::cout << "Server instructions: " << buffer << std::endl;
    }

    std::cout << "You can now enter commands." << std::endl;

    while (true) {
        std::cout << "Enter a command (or 'exit' to quit): ";
        std::getline(std::cin, userInput);

        // Відправлення команди на сервер
        result = send(clientSocket, userInput.c_str(), userInput.size(), 0);
        if (result == -1) {
            std::cerr << "Failed to send data" << std::endl;
            break;
        }

        // Перевірка, чи клієнт ввів "exit" для виходу
        if (userInput == "exit") {
            std::cout << "Exiting..." << std::endl;
            break;
        }

        // Очищення буфера перед прийомом відповіді від сервера
        memset(buffer, 0, sizeof(buffer));

        // Прийом відповіді від сервера
        result = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (result == -1) {
            std::cerr << "Receive failed" << std::endl;
            break;
        }

        std::cout << "Server response: " << buffer << std::endl;
    }

    // Закриття сокету
    close(clientSocket);

    return 0;
}
#endif