/**
 * @name        des_display.cpp
 * @description Runs in the background, operates the state machine
 *              for the controller, and directly maintains the persistent data and status for the controller. It
 *              should have separate functions for the state handler for each state. Each state handler should
 *              perform the actions required for that state, send a message to the des_display program to
 *              update the display (as required), and then check the exit conditions for that state. When an
 *              exit condition is met, the state handler should return the function pointer for the next state
 *              handler. The des_controller program should print out its process id when it first starts up
 * @author(s):  Gerardo Enrique Alvarenga
 *              Chris Bishop
 */

#include <iostream>
#include <process.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "./des.h"

void send_message(std::string msg){

	std::cout << "Sending message to display proces: " << msg << std::endl;
	std::cout.flush();
	system_status_t system_message;    /* Struct to be sent to the server */
	char resp_msg [200]; 		     /* Response message buffer */
	int  coid;
	response_msg_t* response_message;


	/* Clear the memory for the message and the response */
	memset( &system_message, 0, sizeof(system_message));
	memset( &resp_msg, 0, sizeof(resp_msg));

	system_message.person_id = 123456;
	strcpy(system_message.message , msg.c_str());

	/* Establish a connection */

	/*
	 * @params
	 * nd The node descriptor of the node (e.g. ND_LOCAL_NODE for the local node) on which the process that owns the channel is running; see “Node descriptors,” below.
	 * pid The process ID of the owner of the channel. If pid is zero, the calling process is assumed.
	 * chid The channel ID, returned by ChannelCreate(), of the channel to connect to the process.
	 * index The lowest acceptable connection ID.
	 * flags If flags contains _NTO_COF_CLOEXEC, the connection is closed when your process calls an exec
	 */
	coid = ConnectAttach(ND_LOCAL_NODE, serverpid , 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1)
	{
		printf("\n Could not attach to channel running on: %d", serverpid);
		exit(EXIT_FAILURE);
	}

    /* Send the message */
    int size = sizeof(send_msg_request_t);
    char buffer[size]; /* For testing, sending the struct as a buffer to the server */

    memcpy(buffer, &system_message, sizeof(response_msg_t));

    /* Send the message */

	/*
     * @params
     * the connection ID of the target server (coid),
     * a pointer to the send message (smsg),
     * the size of the send message (sbytes),
     * a pointer to the reply message (rmsg), and
     * the size of the reply message (rbytes).
     */
	if (MsgSend(coid, &system_message, sizeof(system_message), resp_msg, sizeof(response_msg_t)) == -1)
	{
		std::cout << "Failed to send the message" << std::endl;
		std::cout.flush();
		exit(EXIT_FAILURE);
	}

	response_message = (response_msg_t*) resp_msg;

	/* Disconnect from the channel */
	ConnectDetach(coid);
}

int main(int argc, char* argv[]) {

    /* Set the function pointer to the function you want */

	void (*funcPointer)();
	funcPointer = start;
    std::cout << "Calling function pointer\n";
    /* Call the function pointer */
    funcPointer();

	/* Validate the number of command line arguments as per requirements */
	if (argc != 2)
	{
		// TODO: Check that argument is a process PID
		std::cout << "Server PID is required" << std::endl;
		std::cout.flush();
		exit(EXIT_FAILURE);
	}

	serverpid = atoi(argv[1]);
	std::cout << "The controller is running as process_id: " << getpid() << std::endl;
	std::cout.flush();

	send_msg_request_t* message_request;
	response_msg_t      response_message;
	char message[200]; /* Message object to receive and send data to client */

    /* Create a channel for the client to connect to */
    chid = ChannelCreate(0);
    if (chid == -1)
    {
        perror("failed to create the channel.");
        exit(EXIT_FAILURE);
    }
    /* Put server in an endless listening state */
    while (1)
    {
    	/*
    	 * Retrieve the message
    	 * @params
    	 * int chid
    	 * void* rmsg - Pointer to received message object
    	 * size_t rbytes
    	 * struct _msg_info *info
    	 */
    	rcvid = MsgReceive(chid, (void*) message, sizeof(message), NULL);

        if (rcvid == -1)
        {
        	printf("Could not parse message");
        }

        message_request = (send_msg_request_t*) message;

        std::cout << "Controller Got: " << message_request->person_id << " - " <<  message_request->instruction << " test "<< std::endl;
        std::cout.flush();

        /* Respond to input */
        /* GET FUNCTION POINTERS GOIN */
        if(strcmp(message_request->instruction,"ls") == 0){
        	std::cout << "assigning function pointer to scanning" << std::endl;
        	std::cout.flush();
        	funcPointer = scanning;
        }
        funcPointer();

        response_message.status_code = SRVR_OK;
        /*
         * rcvid  - The receive ID that MsgReceive*() returned when you received the message.
         * status - The status to use when unblocking the MsgSend*() call in the rcvid thread.
         * msg    - A pointer to a buffer that contains the message that you want to reply with.
         * size   - The size of the message, in bytes.
         */
        MsgReply(rcvid, EOK, (void*) &response_message, sizeof(response_msg_t));

        /* Send message to display */
        // TODO: Pre-define the messages and map to them based on the instruction e.g. inst: 'ls' mapped to 'left door scan' for example
        std::string message = "User sent: " + std::string(message_request->instruction);
        send_message(message);

    }

								/* Phase 3 */

    /* Destroy the channel when done */
    ChannelDestroy(chid);

	return 0;
}

void start(){
	std::cout << "Start function called\n";
	std::cout.flush();
}

void scanning(){
	std::cout << "Scanning function called\n";
	std::cout.flush();
}

void locked(){
	std::cout << "Locked function called\n";
	std::cout.flush();
}

void unlocked(){
	std::cout << "Unlocked function called\n";
	std::cout.flush();
}

void opened(){
	std::cout << "Opened function called" << std::endl;
	std::cout.flush();
}

void weight_scan(){
	std::cout << "Weight Scan function called\n";
	std::cout.flush();
}

void closed(){
	std::cout << "Closed function called\n";
	std::cout.flush();
}

