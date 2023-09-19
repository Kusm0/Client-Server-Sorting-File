#ifdef _WIN32

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <string>

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
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Задання параметрів сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    if (InetPton(AF_INET, _T("192.168.0.113"), &serverAddr.sin_addr) != 1) {
        std::cerr << "Invalid address: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Підключення до сервера
    result = connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to connect: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    char buffer[4096];
    std::string userInput;

    // Очікування інструкцій від сервера
    memset(buffer, 0, sizeof(buffer));
    result = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "Server instructions: " << buffer << std::endl;
    }

    std::cout << "You can now enter commands." << std::endl;

    while (true) {
        std::cout << "Enter a command (or 'exit' to quit): ";
        std::getline(std::cin, userInput);

        // Відправлення команди на сервер
        result = send(clientSocket, userInput.c_str(), userInput.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Failed to send data: " << WSAGetLastError() << std::endl;
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
        if (result == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            break;
        }

        std::cout << "Server response: " << buffer << std::endl;
    }

    // Закриття сокету та очищення Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
#endif