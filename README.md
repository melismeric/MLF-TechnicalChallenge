# MLF-TechnicalChallenge

### Run client code: 
	g++ client.cpp clientClass.cpp -o client -std=c++11 -pthread
  
  
  
### Run server code: 
	 g++ server.cpp clientClass.cpp serverClass.cpp -o server -std=c++11 -pthread


## Server commands to Client:

* Status Change
* Status Check

## Client commands to Server:

* Status Notice
* Device Trigger (which will cause status change after getting command from the server)


## CLIENT Listening to user inputs and server messages:

### THREAD 1: 
	User inputs to test:
        - client <id> <request>
	if request == null
		creates client if not exist
   Else if request == trigger // DEVICE TRIGGER
		send message to server!! // TODO

        - Simulate
        - QuiT

### THREAD 2:
	Server Inputs:
		- client <id> <request>
			if request == status_check
				send statusNotice() to server
			else if request == status_change
				change status of the client
				


### SERVER Listening to each client (thread for each client) & user input

### THREAD 1: 
	User inputs to test:

        - client <id> <request>
			if request == status_check
				send command to client
                		get notice
			else if request == status_change
				send command to client

        - List clients
        - Quit // todo

### THREAD 2+: 
	Client Messages:
		- client <id> status_change Device Trigger
			send status_change command to client
		- <id>,<state> // Status Notice 
			print out status



### Device State Machine:
IDLE, ONBOARDING, ENGAGING, OFFBOARDING, ERROR

TODO:
Test cases
