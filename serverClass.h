#ifndef SERVER_H
#define SERVER_H
#include <sys/socket.h>
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

#include "clientClass.h"
#pragma once

class Server {
    public:
        Server(int port) : m_port(port), running(false), s_shutdown(false) {}

        int server_fd, valread;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        std::vector<std::thread> threads;

        void start(); 
        void changeState(int id, bool error);
        void saveState(int id, int status, State state);
        
        void listActiveClients();

        void listenUserInput();
        void stop();
        void disconnect();
        void handleClient(int client_socket, sockaddr_in client_address, vector<Client*>& active_clients);
        void handleClientInput(string cmd, int socket, sockaddr_in client_address);




    private:
        int new_socket;
        int m_port;
        bool running;
        bool s_shutdown;
        std::vector<Client*> active_clients;
};

#endif