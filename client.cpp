#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

const int port_number = 5000;
const int buffer_size = 1024;

void receive_messages(int server_socket) {
    char buffer[buffer_size];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(server_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            cout << "\nserver disconnected\n";
            break;
        }

        cout << "\nserver: " << buffer << endl;
        cout << "you: ";
        cout.flush();
    }
}

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        cout << "could not create socket\n";
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cout << "connection failed\n";
        close(client_socket);
        return 1;
    }

    cout << "connected to server\n";

    thread receive_thread(receive_messages, client_socket);

    string message;
    while (true) {
        cout << "you: ";
        getline(cin, message);

        if (message == "exit") {
            send(client_socket, message.c_str(), message.size(), 0);
            break;
        }

        if (send(client_socket, message.c_str(), message.size(), 0) < 0) {
            cout << "send failed\n";
            break;
        }
    }

    close(client_socket);

    if (receive_thread.joinable()) {
        receive_thread.join();
    }

    return 0;
}