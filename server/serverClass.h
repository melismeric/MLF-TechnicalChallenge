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

        void listen_user();
        void commandToDevice(int id, std::string req);
        Client* GetClientById(std::vector<Client*>& objects, int id);
        Client* GetClientBySocket(std::vector<Client*>& objects, int socket);
        Client* CreateClientIfNotExist(int id, int socket, sockaddr_in client_address, std::vector<Client*>& objects);
        void removeClientUsingId(std::vector<Client*>& objects, int id);
        void removeClientUsingSocket(std::vector<Client*>& objects, int socket);
        void handleClientInput(std::string cmd, int socket, sockaddr_in client_address);
        void handle_client(int client_socket, sockaddr_in client_address, std::vector<Client*>& active_clients);
        void handle_user_input(std::string input);
        void stop();
        void start();
        void test();   

    private:
        int new_socket;
        int m_port;
        bool running;
        bool s_shutdown;
        std::vector<Client*> active_clients;
};

#endif