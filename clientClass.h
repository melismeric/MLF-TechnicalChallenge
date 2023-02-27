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

class Client {
public:
    Client(int id, string address, int port) : id(id), m_address(address), m_port(port), state(IDLE) {}
    Client(int sock, int id) : m_socket(sock), id(id), state(IDLE) {}

    int getId() {return id;}
    int getSocket() {return m_socket;}
    void setState(State new_state) {state = new_state;}
    void setStateByNum(int stateNum);
    void printState();  // for debug purposes make the states readable
    string getState(); // get Client's current state as string 
    bool connectToServer();
    void disconnect();
    void statusNotice();
    void deviceTrigger();
    void receiveMessage();

private:
    int id;
    string m_address;
    int m_port;
    State state;
    int m_socket;

    void state_change(bool error);
};

#endif