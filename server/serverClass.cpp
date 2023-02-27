#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <sstream>
#include <cstring>
#include <atomic>
#include <chrono>
#include <thread>

#include "serverClass.h"
#include "clientClass.h"

#define PORT 8080
#define BUFFER_SIZE 1024
// maximum number of clients that can connect to the server
#define MAX_CLIENTS 35

using namespace std;   
      
void Server::listen_user() {
    string input;
    while (true) {
        // Read user input from stdin
        input = ""; //clean input
        getline(std::cin, input);
        std::cout << "user command: " << input << std::endl;
        handle_user_input(input);
        
    }
}

// function to search for an object in a vector of object pointers based on its id
Client* Server::GetClientById(vector<Client*>& objects, int id) {
    for (int i = 0; i != objects.size(); ++i) {
        if (objects[i]->getId() == id) { // replace getId() with the getter function for your object's id
            return objects[i];
        }
    }
    return nullptr; // return nullptr if object not found
}

// function to search for an object in a vector of object pointers based on its id
Client* Server::GetClientBySocket(vector<Client*>& objects, int socket) {
    for (int i = 0; i != objects.size(); ++i) {
        if (objects[i]->getSocket() == socket) { // replace getId() with the getter function for your object's id
            return objects[i];
        }
    }
    return nullptr; // return nullptr if object not found
}

void Server::commandToDevice(int id, string req) { // command status change or check to a specific device
// if id exist
// else invalid
    Client* c = GetClientById(active_clients, id);
    if(c != nullptr){
        // Format should be: client <id> <request>
        string s = "client " + to_string(id) + " status_" + req;
        const int length = s.length();
        char* cmd = new char[length + 1];
        strcpy(cmd, s.c_str());
        
        write(c->getSocket(), cmd, strlen(cmd));   
        delete[] cmd;

    } else {
        cout << "Error: Client does not exits!" << endl;
    }

}

Client* Server::CreateClientIfNotExist(int id, int socket, sockaddr_in client_address, vector<Client*>& objects){
    Client* c = GetClientById(objects, id);
                
    if (c != nullptr) {
        return c;
    } else {
        cout << "New client added to list" << endl;
        // if client does not exist 
        Client* client = new Client(socket, id);
        active_clients.push_back(client);
        return client;
    }
}

void Server::removeClientUsingId(vector<Client*>& objects, int id){
    Client *c = GetClientById(objects, id);
    // remove client socket from list of active clients
    objects.erase(
        remove(objects.begin(), objects.end(), c),
        objects.end()
    );
}

void Server::removeClientUsingSocket(vector<Client*>& objects, int socket){
    Client *c = GetClientBySocket(objects, socket);
    // remove client socket from list of active clients
    if (c != nullptr) {
        objects.erase(
            remove(objects.begin(), objects.end(), c),
            objects.end()
        );
    }

}

void Server::handleClientInput(string cmd, int socket, sockaddr_in client_address){
    transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    istringstream iss(cmd); //Todo: make all input lower case
    vector<string> words;
    string word;
    // putting input words into a list to seperate them
    while (iss >> word) {        
        words.push_back(word);
    }

    if(words[0] == "client" ){//if cmd == client <id> <req> Option 1: trigger notice
        int id = stoi(words[1]);
        Client *c = CreateClientIfNotExist(id, socket, client_address, active_clients);
        commandToDevice(id, "change"); 
    } else if(cmd.find(",") != string::npos){  //if <id>,<state> Otion 2: status notice
        int currId = stoi(cmd.substr(0, cmd.find(',')));
        int currState = stoi(cmd.substr(cmd.find(',') + 1));

        Client *c= GetClientById(active_clients, currId);
        c->setStateByNum(currState);
        cout << "Device id: "<< currId << " State: " << c->getState() << endl;
        
        // state notice is idle
        if (currState == 0) {removeClientUsingId(active_clients, currId);}

    } else { //else: invalid client input
        cout << "Invalid Client request/notice" << endl;
    }     
}

void Server::handle_client(int client_socket, sockaddr_in client_address, vector<Client*>& active_clients) {
    // add new client to the active clients list
    // Create a new client object and add it to the list of clients
    
    cout << "New client connected: " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << endl;

    // handle the client request here
    char buffer[BUFFER_SIZE] = {0};
    memset(buffer, 0, BUFFER_SIZE);
    while(true){
        int message_size = read(client_socket, buffer, sizeof(buffer));

        if (message_size <= 0) {
            //client disconnected -> TODO: state_change?
            cout << " DISCONNECTING: "<< inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << endl;
            removeClientUsingSocket(active_clients, client_socket);
            break;
        } 
        cout << "Received message from client: " << buffer << endl;
        // Message from client should be serialized format
        string s = buffer;
        handleClientInput(s, client_socket, client_address);
        memset(buffer, 0, BUFFER_SIZE);
    }
    

    cout << "Client disconnected, socket id: " << client_socket << endl;
    removeClientUsingSocket(active_clients, client_socket);

    // close the client socket
    close(client_socket);
}

void Server::handle_user_input(string input){
    transform(input.begin(), input.end(), input.begin(), ::tolower);

    istringstream iss(input); //Todo: make all input lower case
    vector<string> words;
    string word;
    // putting input words into a list to seperate them
    while (iss >> word) {        
        words.push_back(word);
    }

    if(words[0] == "client" ){//if input == client <id> <req> -> send request to client
        cout << "Send command to client " << words[1] << endl;

        int id = stoi(words[1]);
        Client* c = GetClientById(active_clients, id);
        if(c != nullptr) {  // check if client exist
            if(words[2].find("status_check") != string::npos){ // status check expected status notice from client
                cout << ">>> Sending status_check command to Client " << id << endl;
                commandToDevice(id, "check");
            } else if (words[2].find("status_change") != string::npos) {  // status chnage 
                cout << ">>> Sending status_change command to Client " << id << endl;
                commandToDevice(id, "change");
            } else if (words[2].find("status_error") != string::npos) {  // status chnage 
                cout << ">>> Sending status_change_error command to Client " << id << endl;
                commandToDevice(id, "error");
            }
        } else {
            cout << "Client " << id << " does not exist in the active_clients list." << endl;
        }

    } else if(input == "list clients"){ // Debug
        cout << ">> Listing active clients: " << endl;
        for (auto& c : active_clients) {
            cout << "Id: " << c->getId() << " Socket: " << c->getSocket() << " State: " << c->getState() << endl;
        }
    } else if(input == "quit") {
        cout << "quit!!" << endl;
        stop();
    } else {
        cout << "User said: " << endl; 
    }

    input = ""; //clean input
}

void Server::start(){
    std::vector<std::thread> threads;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                SO_REUSEADDR, &opt,
                sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

    // Forcefully attaching socket to the port 8080
    if (::bind(server_fd, (struct sockaddr*)&address,
            sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Server is now running
    running = true;

    // accept incoming client connections and handle them in separate threads
    while (running) {
        std::cout << "Running" << std::endl;

        // Check for shutdown command from user input
        if (s_shutdown) {
            std::cout << "Shutting down server\n";
            running = false;
            break;
        }

        sockaddr_in client_address;
        int client_socket = accept(server_fd, (sockaddr*) &client_address, (socklen_t*) &client_address);
        if (client_socket < 0) {
            std::cerr << "Error accepting client connection." << std::endl;
            continue;
        } 

        if (threads.size() >= MAX_CLIENTS) {
            std::cerr << "Maximum number of clients reached, closing connection" << std::endl;
            close(client_socket);
            continue;
        }
        
        // Create a new thread to handle the client
        threads.push_back(std::thread(&Server::handle_client, this, client_socket, client_address, ref(active_clients)));
        

        threads.push_back(std::thread(&Server::listen_user, this));

        //std::cout << "Main thread ID: " << std::this_thread::get_id() << std::endl;

        //OR detach thread after creating it

        /*std::thread client_thread(handle_client, client_socket, client_address, ref(active_clients));
        client_thread.detach();*/
    }

    // Join all threads before exiting
    for (auto& thread : threads) {
        thread.join();
        std::cout << "Joined thread ID: " << thread.get_id() << std::endl;
    }

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
}


void Server::stop(){
    s_shutdown = true;
}

void Server::test(){
    std::cout << "test" << std::endl;
}