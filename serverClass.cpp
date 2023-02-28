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

// Helper functions for active clients list
// function to search for an object in a vector of object pointers based on its id
Client* GetClientById(vector<Client*>& objects, int id) {
    for (int i = 0; i != objects.size(); ++i) {
        if (objects[i]->id == id) { // replace getId() with the getter function for your object's id
            return objects[i];
        }
    }
    return nullptr; // return nullptr if object not found
}

// function to search for an object in a vector of object pointers based on its id
Client* GetClientBySocket(vector<Client*>& objects, int socket) {
    for (int i = 0; i != objects.size(); ++i) {
        if (objects[i]->getSocket() == socket) { // replace getId() with the getter function for your object's id
            return objects[i];
        }
    }
    return nullptr; // return nullptr if object not found
}

Client* AddClientIfNotExist(int id, int socket ,vector<Client*>& objects){
    Client* c = GetClientById(objects, id);
                
    if (c != nullptr) {
        return c;
    } else {
        cout << "New client " + to_string(id) + " added to list" << endl;
        // if client does not exist 
        Client* client = new Client(socket, id);
        objects.push_back(client);
        return client;
    }
}

void removeClientUsingSocket(vector<Client*>& objects, int socket){
    Client *c = GetClientBySocket(objects, socket);
    // remove client socket from list of active clients
    if (c != nullptr) {
        objects.erase(
            remove(objects.begin(), objects.end(), c),
            objects.end()
        );
    }

}

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

string stateToString(State state){
    if(state == IDLE) {
        return "IDLE";
    } else if(state == ONBOARDING) {
        return "ONBOARDING";
    } else if(state == ENGAGING) {
        return "ENGAGING";
    } else if(state == OFFBOARDING) {
        return "OFFBOARDING";
    } else {
        return "ERROR";
    }
}

//

void Server::listActiveClients(){
    cout << ">> Listing active clients: " << endl;
    for (auto& c : active_clients) {
        cout << "Id: " << c->id << " State: " << stateToString(c->getState()) << endl;
    }
}

void Server::listenUserInput(){
    string input;
    while (true) {
        
        // Read user input from stdin
        input = ""; //clean input
        getline(std::cin, input);

        istringstream iss(input);
        vector<string> words;
        string word;
        // putting input words into a list to seperate them
        while (iss >> word) {        
            words.push_back(word);
        }


        if(input == "quit"){
            break;
        } else if (input == "list") {
            listActiveClients();
        } else if (input == "random state_change") {
            int i = rand() % active_clients.size();
            changeState(active_clients[i]->id, false);

        } else if (input == "random state_check") {
            int i = rand() % active_clients.size();
            int id = active_clients[i]->id;

            string s = to_string(id) + " state_check";
            const int length = s.length();
            char* cmd = new char[length + 1];
            strcpy(cmd, s.c_str());
            
            write(active_clients[i]->getSocket(), cmd, strlen(cmd));   
            delete[] cmd;
        } else if (input.find("state_check") != string::npos) {
            // send status check to devices
            
            int id = stoi(words[0]);
            Client* c = GetClientById(active_clients, id);
            if(c != nullptr){
                // Format should be: <id> <request>
                string s = to_string(id) + " state_check";
                const int length = s.length();
                char* cmd = new char[length + 1];
                strcpy(cmd, s.c_str());
                
                write(c->getSocket(), cmd, strlen(cmd));   
                delete[] cmd;

            } else {
                cout << "Error: Client does not exits!" << endl;
            }
            
        } else if (input.find("state_change") != string::npos) {

            // send status check to devices
            int id = stoi(words[0]);
            Client* c = GetClientById(active_clients, id);
            if(c != nullptr){
                cout << id << " requires state change.";
                changeState(id, false);
            } else {
                cout << "Error: Client does not exits!" << endl;
            }
        } else if (input.find("state_error") != string::npos) {

            // send status check to devices
            int id = stoi(words[0]);
            Client* c = GetClientById(active_clients, id);
            if(c != nullptr){
                cout << id << " requires state change.";
                changeState(id, true);
            } else {
                cout << "Error: Client does not exits!" << endl;
            }
        }
        
        
    }
    disconnect();
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
  
    //<id> <req> OR <id> <status> <state>
    if (is_number(words[0])) {

        if(is_number(words[1])) {
            if (stoi(words[1]) == 0) {
                cout << "Device State Notice:" << endl;
                cout << "Device " + words[0] + " state: " + words[2] << endl;
            } else if (stoi(words[1]) == 1) {
                cout << "Device Status is Error:" << endl;
                cout << "Device " + words[0] + " state: " + words[2] << endl;
            }

        } else {
            if (words[1] == "connect") {
                cout << words[0] << " requires connection.";
                int inputId = stoi(words[0]);
                Client *c = AddClientIfNotExist(inputId, socket ,active_clients);
                changeState(inputId, false);
            } else if (words[1] == "state_change") {
                int inputId = stoi(words[0]);
                cout << words[0] << " requires state change.";
                changeState(inputId, false);
            } else if (words[1] == "state_error") {
                int inputId = stoi(words[0]);
                cout << words[0] << " requires state change with error.";
                changeState(inputId, true);
            } 
        }
        
    } else {
        cout << "Invalid message from device" << endl;
    }
  
}

void Server::handleClient(int client_socket, sockaddr_in client_address, vector<Client*>& active_clients) {
    // add new client to the active clients list
    // Create a new client object and add it to the list of clients
    
    cout << "New device connected: "<< endl;

    // handle the client request here
    char buffer[BUFFER_SIZE] = {0};
    memset(buffer, 0, BUFFER_SIZE);
    while (true) {
        
        // Check for shutdown command from user input
        if (s_shutdown) {
            std::cout << "Inside handle_client Shutting down server\n";
            running = false;
            disconnect();
            break;
        }
        int message_size = read(client_socket, buffer, sizeof(buffer));

        if (message_size <= 0) {
            //client disconnected -> TODO: state_change?
            cout << "Device disconnecting: "<< inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << endl;
            removeClientUsingSocket(active_clients, client_socket);
            break;
        } 
        
        // Message from client should be serialized format
        string s = buffer;
        handleClientInput(s, client_socket, client_address);
        memset(buffer, 0, BUFFER_SIZE);
    }

    // close the client socket
    close(client_socket);
}

void Server::changeState(int id, bool error){
    Client *c = GetClientById(active_clients, id);
  
    if (error) {
        cout << " state changing to error" << endl;
        saveState(id,error, ERROR); // status should be fail
        removeClientUsingSocket(active_clients, c->getSocket());
    } else if (c->getState() == 0) {
        cout << "Idle state changing to onboarding" << endl;
        saveState(id,error, ONBOARDING);
    } else if (c->getState()  == 1) {
        cout << "Onboarding state changing to engaging" << endl;
        saveState(id,error, ENGAGING);
    } else if (c->getState() == 2) {
        cout << "Engaging state changing to offboarding" << endl;
        saveState(id,error, OFFBOARDING);
    } else if (c->getState() == 3) {
        cout << "Offboarding state changing to idle" << endl;
        saveState(id,error, IDLE);
        removeClientUsingSocket(active_clients, c->getSocket());
    }  
}

void Server::saveState(int id, int status, State state){
    Client *c = GetClientById(active_clients, id);
    c->setState(state);
    
    string s = to_string(id) + " " + to_string(status) + " " + to_string(state);

    const int length = s.length();
    char* cmd = new char[length + 1];
    strcpy(cmd, s.c_str());
    

    write(c->getSocket(), cmd, strlen(cmd));   
    delete[] cmd;
}
      
void Server::start(){
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

    cout << "Server listening on port " << PORT << endl;
    cout << "Command List:" << endl;
    cout << "   list: list active devices" << endl;
    cout << "   quit: Close server" << endl;
    cout << "   <id> <state_check>: Check device state by id." << endl;
    cout << "   <id> <state_change>: Change device state by id." << endl;
    cout << "   <id> <state_error>: Send error" << endl;


    // Server is now running
    running = true;

    //threads.push_back(std::thread(&Server::listen_user, this));
    std::thread user_thread(&Server::listenUserInput, this);
    user_thread.detach();

    
    // accept incoming client connections and handle them in separate threads
    while (running) {
        std::cout << "Running" << std::endl;

        // Check for shutdown command from user input
        if (s_shutdown) {
            std::cout << "Shutting down server\n";
            running = false;
            disconnect();
            break;
        }

        sockaddr_in client_address;
        int client_socket = accept(server_fd, (sockaddr*) &client_address, (socklen_t*) &client_address);
        if (client_socket < 0) {
            std::cerr << "Error accepting client connection." << std::endl;
            continue;
        } 

        if (threads.size() >= MAX_CLIENTS) {
            std::cerr << "Maximum number of clients reached, closing last connection" << std::endl;
            close(client_socket);
            continue;;
        }
        
        // Create a new thread to handle the client
        threads.push_back(std::thread(&Server::handleClient, this, client_socket, client_address, ref(active_clients)));
        

        //std::cout << "Main thread ID: " << std::this_thread::get_id() << std::endl;

        //OR detach thread after creating it

        //std::thread client_thread(&Server::handleClient, this, client_socket, client_address, ref(active_clients));
        //client_thread.detach();
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

void Server::disconnect(){
    // Join all threads before exiting
    /*for (auto& thread : threads) {
        thread.join();
        std::cout << "Joined thread ID: " << thread.get_id() << std::endl;
    }*/

    cout << "Closing the server" << endl;

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    exit(0);
}