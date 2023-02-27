// client.cpp
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

#include "clientClass.h"

using namespace std;

// function to search for an object in a vector of object pointers based on its id
Client* GetClientById(vector<Client*>& objects, int id) {
    for (int i = 0; i != objects.size(); ++i) {
        if (objects[i]->getId() == id) { // replace getId() with the getter function for your object's id
            return objects[i];
        }
    }
    return nullptr; // return nullptr if object not found
}

Client* CreateClientIfNotExist(vector<Client*>& objects, int id, string address, int port){
    Client* c = GetClientById(objects, id);
                
    if (c != nullptr) {
        c->deviceTrigger();
        return c;
    }
    else {
        cout << "Client Create with id: " << id << endl;
        Client *cl = new Client(id, address, port);
        if (!cl->connectToServer()) {
            return nullptr; // todo error
        } else {
            objects.push_back(cl);
            //each client listens to the server using a thread
            thread receive_thread(&Client::receiveMessage, cl);
            receive_thread.detach();
            return cl;
        }

    }
}


int main() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds

    string address = "127.0.0.1";
    int port = 8080;
    
    cout << "client <id> <request> OR simulate OR quit" << endl;
    cout << "Requests can be: status_check, status_change" << endl;

    //clientList
    vector<Client*> clients;

    
    while (true) {
        string message;
        cout << "> ";
        getline(cin, message);

        istringstream iss(message);
        vector<string> words;
        string word;
        // putting input words into a list to seperate them
        while (iss >> word) {        
            words.push_back(word);
        }

        if(message == "quit"){
            cout << "Quit Application" << endl;
            break;
        } else if (message == "simulate") {
            cout << "Simulation" << endl;
            // TODO simulate 35 clients
            int maxClients = 3;
            for(int i=0 ; i < maxClients ; i++){
                sleep_for(nanoseconds(10));
                sleep_until(system_clock::now() + seconds(1));

                Client* c = CreateClientIfNotExist(clients, i, address, port);
            }

            for(int i=0 ; i < maxClients ; i++){
                sleep_for(nanoseconds(10));
                sleep_until(system_clock::now() + seconds(1));

                Client* c = GetClientById(clients, i);

                c->deviceTrigger();
            }
        } else if (words[0] == "client" && isdigit(message[7])) { // device trigger
            int inputId = static_cast<int>(message[7]) - 48;

            if (words[2] == "disconnect") {
                Client *c = GetClientById(clients, inputId);
                if(c != nullptr){
                    c->disconnect();
                }
            } else if (words[2] == "" || words[2] == "trigger"){
                Client* c = CreateClientIfNotExist(clients, inputId, address, port);
                c->deviceTrigger();
            }
        
        } else {
            cout << "Invalid input" << endl;

        }
    }

    return 0;
}