#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

const int port_number = 5000;
const int buffer_size = 1024;

void receive_messages(int client_socket) {
    char buffer[buffer_size];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            cout << "\nclient disconnected\n";
            break;
        }

        cout << "\nclient: " << buffer << endl;
        cout << "you: ";
        cout.flush();
    }
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cout << "could not create socket\n";
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cout << "bind failed\n";
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 1) < 0) {
        cout << "listen failed\n";
        close(server_socket);
        return 1;
    }

    cout << "server is waiting on port " << port_number << "...\n";

    sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
    if (client_socket < 0) {
        cout << "accept failed\n";
        close(server_socket);
        return 1;
    }

    cout << "client connected\n";

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
    close(server_socket);

    if (receive_thread.joinable()) {
        receive_thread.join();
    }

    return 0;
}