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
* Error Case


## CLIENT Listening to user inputs and server messages:

### THREAD 1: 
	User inputs to test: (simulating device trigger with user input)
        - <id> <request>
	if request == connect
		creates client if not exist
   	Else if request == state_change
		send request to server 
	Else if request == state_error 
		Client Disconnects

        - simulate
        - quit

### THREAD 2: 
	Server Inputs:
	- <id> <request>
		if request == state_check
			send state notice to server
		else if request == state_change
			save new state of the client
				


### SERVER 
Listening to each client (thread for each client) & user input

### Handling State Machine:
IDLE -> ONBOARDING -> ENGAGING -> OFFBOARDING -> IDLE
 |          |             |            |          | 
                        ERROR 

Server deals with the device states, when a device is triggered (either by server or client) it changes the device state and sends it to device class.

### THREAD 1: 
	User inputs to simulate:
	Command List:
	   list: list active devices
	   quit: Close server
	   <id> <state_check>: Check device state by id.
	   <id> <state_change>: Change device state by id.
	   <id> <state_error>: Send error
	   random <state_check>: Random state check of one of the active devices
	   random <state_change>: Random state change of one of the active devices

        -> <id> <request>
		if request == status_check
			send command to client
			get notice
		else if request == status_change
			send command to client
		else if request == status_error
			send command to client to change the status to error & client disconnects



### THREAD 2+: 
	Client Messages:
		- client <id> state_change Device Trigger
			send state_change command to client
		- <id> <state> // State Notice 
			print out state





## Test Cases:

### Active Clients list:
This list keeps active devices in the server. 
The active devices means the device state is transitioning with user interaction. Meaning; active clients list can be ONBOARDING -> ENGAGING -> OFFBOARDING. When a device connects to server the state changes from idle to onboarding and it is added to the list. And when it goes idle from offboarding it state gets to idle and it gets out of the list.

<img width="300" alt="Screen Shot 2023-02-28 at 10 46 10" src="https://user-images.githubusercontent.com/37816087/221831666-c64c15ff-024f-434b-bea7-90c43d0ed737.png">
<img width="300" alt="Screen Shot 2023-02-28 at 10 46 21" src="https://user-images.githubusercontent.com/37816087/221831700-fa1327ea-fdb5-43ba-9902-2da0d1366c61.png">

### Normal flow:
Idle -> Onboarding -> Engaging -> Offboarding -> Idle
Device should be in the active list then get out of the list when offboarded then should be added again via user interaction.

<img width="400" alt="Screen Shot 2023-02-28 at 10 17 19" src="https://user-images.githubusercontent.com/37816087/221824872-993bbd4c-30d4-4af8-9e75-1e2d01f582ac.png"> <img width="400" alt="Screen Shot 2023-02-28 at 10 17 38" src="https://user-images.githubusercontent.com/37816087/221824937-833fe3eb-169a-416c-b332-2cf5447477c0.png">

### Disconnection
Disconnection means an error case where user interaction causes sudden disconnection/ error state. 
This can be tested in client side with these commands:
> 1 connect
> 1 disconnect
> 1 connect 

<img width="400" alt="Screen Shot 2023-02-28 at 10 24 17" src="https://user-images.githubusercontent.com/37816087/221826530-437a67c3-64c9-4ea7-acfd-0092fa7b3554.png"> <img width="400" alt="Screen Shot 2023-02-28 at 10 24 28" src="https://user-images.githubusercontent.com/37816087/221826586-8c4152c3-46c4-4753-9638-08774c269a53.png">




