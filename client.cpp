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
        if (objects[i]->id == id) { // replace getId() with the getter function for your object's id
            return objects[i];
        }
    }
    return nullptr; // return nullptr if object not found
}

Client* CreateClientIfNotExist(vector<Client*>& objects, int id, string address, int port){
    Client* c = GetClientById(objects, id);
                
    if (c != nullptr) {
        if (!c->connectToServer()) {
            return nullptr; // todo error
        }
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
    
    cout << "<id> <request> OR simulate OR quit" << endl;
    cout << "Requests can be: connect, state_change, disconnect, state_error" << endl;

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
            int maxClients = 35;
            for(int i=0 ; i < maxClients ; i++){
                sleep_for(nanoseconds(10));
                sleep_until(system_clock::now() + seconds(1));

                Client* c = CreateClientIfNotExist(clients, i, address, port);
                
                sleep_for(nanoseconds(15));
                sleep_until(system_clock::now() + seconds(1));
                c->changeStateRequest(false);
            }


        } else if (isdigit(message[0])) { // device trigger
            int inputId = stoi(words[0]);

            if (words[1] == "disconnect") {
                Client *c = GetClientById(clients, inputId);
                if(c != nullptr){
                    c->disconnect();
                }
            } else if (words[1] == "connect"){
                Client* c = CreateClientIfNotExist(clients, inputId, address, port);
                cout << "connect" << endl;
            } else if (words[1] == "state_change"){
                Client* c = CreateClientIfNotExist(clients, inputId, address, port);
                cout << "state_change" << endl;
                c->changeStateRequest(false);
            } else if (words[1] == "state_error"){
                Client* c = CreateClientIfNotExist(clients, inputId, address, port);
                cout << "state_change_errorr" << endl;
                c->changeStateRequest(true);
            }
        
        } else {
            cout << "Invalid input" << endl;

        }
    }

    return 0;
}