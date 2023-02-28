#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

using namespace std;

enum State {IDLE, ONBOARDING, ENGAGING, OFFBOARDING, ERROR};
enum Status {SUCCESS, FAIL};

class Client {
public:
    int id;
    Client(int socket, int id) : m_socket(socket), id(id) {}
    Client(int id, string address, int port) : id(id), m_address(address), m_port(port) {}
    //Client(int sock, int id) : m_socket(sock), id(id) {}

    bool connectToServer();
    void changeStateRequest(bool err); //send request to server
    void setState(State newState) {curr_state = newState;};
    State getState() {return curr_state;}
    Status printStatus() {return status;}
    void disconnect();
    void receiveMessage();
    void stateNotice();

    int getSocket() {return m_socket;}

private:
    int m_socket;
    string m_address;
    int m_port;
    State curr_state;
    Status status;
};

#endif