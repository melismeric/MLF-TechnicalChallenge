# MLF-TechnicalChallenge

### Build&Run client code: 
	g++ client.cpp clientClass.cpp -o client -std=c++11 -pthread
	./client
  
  
  
### Build&Run server code: 
	 g++ server.cpp clientClass.cpp serverClass.cpp -o server -std=c++11 -pthread
	 ./server


## Server commands to Client:

* Status Change
* Status Check
* Error Case

## Client commands to Server:

* Status Notice
* Device Trigger/ Status Change Request (which will cause status change after getting command from the server)


## CLIENT Listening to user inputs and server messages:

### THREAD 1: 
	User inputs to test:
        - client <id> <request>
	if request == connect
		creates client if not exist
   	Else if request == state_change // DEVICE TRIGGER
		send request to server 
	Else if request == disconnect // DEVICE TRIGGER
		Client Disconnects

        - Simulate
        - QuiT

### THREAD 2:
	Server Inputs:
		- client <id> <request>
			if request == status_check
				send statusNotice() to server
			else if request == status_change
				change status of the client
				


### SERVER 
Listening to each client (thread for each client) & user input

### Handling State Machine:
IDLE -> ONBOARDING -> ENGAGING -> OFFBOARDING -> IDLE
 |          |             |            |          | 
                        ERROR 

Server deals with the device states, when a device is triggered (either by server or client) it changes the device state and sends it to device class.

### THREAD 1: 
	User inputs to test:

        - client <id> <request>
			if request == status_check
				send command to client
                		get notice
			else if request == status_change
				send command to client
			else if request == status_error
				send command to client to change the status to error

        - List clients
        - Quit // todo

### THREAD 2+: 
	Client Messages:
		- client <id> status_change Device Trigger
			send status_change command to client
		- <id>,<state> // Status Notice 
			print out status




TODO:
Test cases
