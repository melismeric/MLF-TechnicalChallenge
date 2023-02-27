#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

#include "clientClass.h"
#define BUFFER_SIZE 1024


using namespace std;


// for debug purposes make the states readable
void Client::printState() { 
    switch (state) {
        case 0:
            cout << "Idle" << endl;
            break;
        case 1:
            cout << "Onboarding" << endl;
            break;
        case 2:
            cout << "Engaging" << endl;
            break;
        case 3:
            cout << "Offboarding" << endl;
            break;
        case 4:
            cout << "Error" << endl;
            break;
    }
}

string Client::getState(){
    if( state == IDLE) {
        return "IDLE";
    } else if ( state == ONBOARDING) {
        return "ONBOARDING";
    } else if ( state == ENGAGING) {
        return "ENGAGING";
    } else if ( state == OFFBOARDING) {
        return "OFFBOARDING";
    } else {
        return "ERROR";
    }
}

void Client::setStateByNum(int stateNum){
    if(stateNum == 0){
        setState(IDLE);
    } else if (stateNum == 1){
        setState(ONBOARDING);
    } else if (stateNum == 2){
        setState(ENGAGING);
    } else if (stateNum == 3){
        setState(OFFBOARDING);
    }
}

bool Client::connectToServer() {
    // Create socket
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Client Socket: " << m_socket << endl;
    if (m_socket == -1) {
        cerr << "Failed to create socket." << endl;
        return false;
    }

    // Set server address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(m_port);
    if (inet_pton(AF_INET, m_address.c_str(), &serverAddress.sin_addr) <= 0) {
        cerr << "Invalid address or address not supported." << endl;
        return false;
    }

    // Connect to server
    if (connect(m_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Connection failed." << endl;
        return false;
    } else {
        string m = "client " + to_string(id) + " created";
        send(m_socket, m.c_str(), m.length(), 0);
    }
    return true;
}

void Client::disconnect(){
    close(m_socket);
    cout << "Client disconnected" << endl;
}

void Client::statusNotice(){
    string notice = to_string(id) + "," + to_string(state);
    send(m_socket, notice.c_str(), notice.length(), 0);
}

void Client::deviceTrigger(){
    string t = "Client " + to_string(id) + " trigger";
    cout << ">>>" + t << endl;
    send(m_socket, t.c_str(), t.length(), 0);
}

void Client::receiveMessage() {
    while (true) {
        char buffer[BUFFER_SIZE] = {0};
        int valread = read(m_socket, buffer, 1024);
        string s = buffer;
        if (valread > 0 && s.find("status_change") != string::npos){
            state_change(false); //change status without error;

        } else if (valread > 0 && s.find("status_error") != string::npos){
            state_change(true); //change status  error;

        } else if (valread > 0 && s.find("status_check") != string::npos){
            statusNotice();

        } else {
            cout << "Disconnected from server" << endl;
            break;
        }
            memset(buffer, 0, BUFFER_SIZE);
    }
}

void Client::state_change(bool error) {
    
    if (error) {
        cout << " state changing to error" << endl;
        state = ERROR;
    } else if (state == IDLE) {
        cout << "Idle state changing to onboarding" << endl;
        state = ONBOARDING;
    } else if (state == ONBOARDING) {
        cout << "Onboarding state changing to engaging" << endl;
        state = ENGAGING;      
    } else if (state == ENGAGING) {
        cout << "Engaging state changing to offboarding" << endl;
        state = OFFBOARDING;
    } else if (state == OFFBOARDING) {
        cout << "Offboarding state changing to idle" << endl;
        state = IDLE;
    }
    // TODO: What happens if state is ERROR

}