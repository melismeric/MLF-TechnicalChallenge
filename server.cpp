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

using namespace std;
#define PORT 8080

int main(int argc, char const* argv[])
{
	Server s(PORT);
    s.start();   
	return 0;
}