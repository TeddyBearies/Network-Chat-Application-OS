#include <iostream>      // for cout and cin
#include <thread>        // for thread
#include <cstring>       // for memset
#include <unistd.h>      // for close()
#include <arpa/inet.h>   // for sockaddr_in, htons
#include <sys/socket.h>  // for socket functions

using namespace std;

// fixed port number for the chat
const int port_number = 5000;

// size of message buffer
const int buffer_size = 1024;

// this function keeps receiving messages from the client
void receive_messages(int client_socket) {
    char buffer[buffer_size];

    while (true) {
        // clear old data from buffer before receiving new message
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        // if recv returns 0 or less, the client disconnected or an error happened
        if (bytes_received <= 0) {
            cout << "\nclient disconnected\n";
            break;
        }

        // show the received message on the server screen
        cout << "\nclient: " << buffer << endl;
        cout << "you: ";
        cout.flush();
    }
}

int main() {
    // create server socket using tcp
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cout << "could not create socket\n";
        return 1;
    }

    sockaddr_in server_address;

    // set server address details
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;  // accept connections from any ip

    // bind socket to port
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cout << "bind failed\n";
        close(server_socket);
        return 1;
    }

    // start listening for one client
    if (listen(server_socket, 1) < 0) {
        cout << "listen failed\n";
        close(server_socket);
        return 1;
    }

    cout << "server is waiting on port " << port_number << "...\n";

    sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    // accept incoming client connection
    int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
    if (client_socket < 0) {
        cout << "accept failed\n";
        close(server_socket);
        return 1;
    }

    cout << "client connected\n";

    // start a thread for receiving messages
    thread receive_thread(receive_messages, client_socket);

    string message;

    // main loop for sending messages to the client
    while (true) {
        cout << "you: ";
        getline(cin, message);

        // send exit and stop chat
        if (message == "exit") {
            send(client_socket, message.c_str(), message.size(), 0);
            break;
        }

        // send typed message to client
        if (send(client_socket, message.c_str(), message.size(), 0) < 0) {
            cout << "send failed\n";
            break;
        }
    }

    // close sockets after chat ends
    close(client_socket);
    close(server_socket);

    // wait for receiving thread to finish
    if (receive_thread.joinable()) {
        receive_thread.join();
    }

    return 0;
}