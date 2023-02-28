#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>

#include "serverClass.h"
#include "clientClass.h"

#define PORT 8080
#define MAX_CLIENTS 36

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

// function to search for an object in a vector of object pointers based on its id
Client* GetClient(vector<Client*>& objects, int id) {
    for (int i = 0; i != objects.size(); ++i) {
        if (objects[i]->id == id) { // replace getId() with the getter function for your object's id
            return objects[i];
        }
    }
    return nullptr; // return nullptr if object not found
}

Client* CreateClientIfNotExist(vector<Client*>& objects, int id, string address, int port){
    Client* c = GetClient(objects, id);
                
    if (c != nullptr) {
        //c->deviceTrigger();
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


TEST(ClientTest, ClientConnection) {  
  int id = 1;
  string address = "127.0.0.1";
  int port = 8080; 

  Client client(id, address, port);
  ASSERT_TRUE(client.connectToServer());
  client.disconnect();
}

TEST(ClientTest2, ClientStateOnboarding) {  
  int id = 2;
  string address = "127.0.0.1";
  int port = 8080; 

  Client client(id, address, port);
  client.connectToServer();
  
  // State should be ongoing
    EXPECT_EQ(client.getState(),1);
    client.disconnect();
}

TEST(ClientTest3, changeStateRequest) {  
  int id = 3;
  string address = "127.0.0.1";
  int port = 8080; 

  Client client(id, address, port);
  client.connectToServer();
  std::chrono::seconds(20);
  client.changeStateRequest(false);
  
  // State should be engaging
  EXPECT_EQ(client.getState(),1);
  client.disconnect();
}



TEST(LimitTest, limit){
    //clientList
    vector<Client*> clients;
    string address = "127.0.0.1";
    int port = 8080; 
    for(int i = 0; i < MAX_CLIENTS; i++){
        Client* c = CreateClientIfNotExist(clients, i, address, port);
    }
    Client *lastClient = GetClient(clients, 35);
    // expected state is idle for the last device
    EXPECT_EQ(lastClient->getState(),0);
}

/*
// State Request w/ error
TEST(ClientTest4, changeStateRequestError) {  
  int id = 4;
  string address = "127.0.0.1";
  int port = 8080; 

  Client client(id, address, port);
  client.connectToServer();
  std::chrono::seconds(20);
  client.changeStateRequest(true);
  std::chrono::seconds(100);
  // State should be engaging
  EXPECT_EQ(client.getState(),4);
  client.disconnect();
} 

TEST(ClientTest5, disconnect) { 
  vector<Client*> clients; 
  int id = 4;
  string address = "127.0.0.1";
  int port = 8080; 

  Client* c = CreateClientIfNotExist(clients, id, address, port);
   c->changeStateRequest(true);
  
  // State should be idle
  //c->disconnect();
  //std::chrono::seconds(100);
  EXPECT_EQ(c->getState(),0);
}*/