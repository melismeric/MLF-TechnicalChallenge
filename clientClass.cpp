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

//Helper function

State stringToState(string i){
    if(i == "0") {
        return IDLE;
    } else if(i == "1") {
        return ONBOARDING;
    } else if(i == "2") {
        return ENGAGING;
    } else if(i == "3") {
        return OFFBOARDING;
    } else {
        return ERROR;
    }
}

//

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
        string m = to_string(id) + " connect ";
        send(m_socket, m.c_str(), m.length(), 0);
    }
    return true;
}

void Client::disconnect(){
    close(m_socket);
    cout << "Device " + to_string(id) + " disconnected." << endl;
}

void Client::changeStateRequest(bool err){
    string t = to_string(id) + " state_change";
    if (err) t = to_string(id) + " state_error";
    send(m_socket, t.c_str(), t.length(), 0);
}

void Client::stateNotice(){
    //<id> <status> <state>
    string notice = to_string(id) + " " + to_string(status) + " " + to_string(curr_state);
    cout << notice << endl;
    send(getSocket(), notice.c_str(), notice.length(), 0);
}

void Client::receiveMessage() {
    while (true) {
        char buffer[BUFFER_SIZE] = {0};
        int valread = read(m_socket, buffer, 1024);
        string s = buffer;
        
        istringstream iss(s);
        vector<string> words;
        string word;
        // putting input words into a list to seperate them
        while (iss >> word) {        
            words.push_back(word);
        }

        if (valread > 0 && s.find("state_check") != string::npos){
            stateNotice();
        } else if (words.size() == 3){
            if (valread > 0 && stoi(words[1]) == 0){
                cout << "State changed successfully." << endl;
                curr_state = stringToState(words[2]);
            } else if (valread > 0 && stoi(words[1]) == 1){
                cout << "State change failed or error happened" << endl;
                curr_state = ERROR;
                disconnect();
            }
        }else {
            cout << "Disconnected from server" << endl;
            //disconnect();
            break;
        }
        
        memset(buffer, 0, BUFFER_SIZE);
    }
}