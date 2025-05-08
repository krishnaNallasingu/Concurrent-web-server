#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

const int PORT = 8060;

// Function to handle each client connection
void handle_client(int client_socket) {
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);

    std::cout << "Received request:\n" << buffer << std::endl;

    // Simple HTTP response
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello World!..";

    send(client_socket, response, strlen(response), 0);
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to port
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    // Accept clients in a loop
    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        // Create a new thread to handle the client
        std::thread(handle_client, client_socket).detach();
    }

    return 0;
}
