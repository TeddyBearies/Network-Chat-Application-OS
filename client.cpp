#include <iostream>      // for cout and cin
#include <thread>        // for thread
#include <cstring>       // for memset
#include <unistd.h>      // for close()
#include <arpa/inet.h>   // for inet_pton, sockaddr_in
#include <sys/socket.h>  // for socket functions

using namespace std;

// same port number as the server
const int port_number = 5000;

// same buffer size as the server
const int buffer_size = 1024;

// this function keeps receiving messages from the server
void receive_messages(int server_socket) {
    char buffer[buffer_size];

    while (true) {
        // clear old buffer content
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(server_socket, buffer, sizeof(buffer) - 1, 0);

        // stop if server disconnects or an error happens
        if (bytes_received <= 0) {
            cout << "\nserver disconnected\n";
            break;
        }

        // print message from server
        cout << "\nserver: " << buffer << endl;
        cout << "you: ";
        cout.flush();
    }
}

int main() {
    // create client socket using tcp
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        cout << "could not create socket\n";
        return 1;
    }

    sockaddr_in server_address;

    // set server ip and port
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);  // localhost

    // connect client to server
    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cout << "connection failed\n";
        close(client_socket);
        return 1;
    }

    cout << "connected to server\n";

    // start a thread for receiving messages
    thread receive_thread(receive_messages, client_socket);

    string message;

    // main loop for sending messages to the server
    while (true) {
        cout << "you: ";
        getline(cin, message);

        // send exit and stop client
        if (message == "exit") {
            send(client_socket, message.c_str(), message.size(), 0);
            break;
        }

        // send typed message to server
        if (send(client_socket, message.c_str(), message.size(), 0) < 0) {
            cout << "send failed\n";
            break;
        }
    }

    // close socket after finishing
    close(client_socket);

    // wait for receiving thread to finish
    if (receive_thread.joinable()) {
        receive_thread.join();
    }

    return 0;
}