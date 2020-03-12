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
#include <sys/uio.h>
#include <limits.h>
#include "./des.h"

/**
 * This function sends a message to the display process.
 * The display returns a pass or fail response.
 */
void send_message(std::string msg){

	char message[400];     /* Message object to send data from the display */
	memset(&message, 0, sizeof(message));
	strcpy(message, msg.c_str());

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

	/*
     * @params
     * the connection ID of the target server (coid),
     * a pointer to the send message (smsg),
     * the size of the send message (sbytes),
     * a pointer to the reply message - Set to null if no need of a response message,
     * the size of the reply message  - Set to 0 if not expecting response message.
     */
	int return_code = -1;
	return_code = MsgSend(coid, &message, sizeof(message), NULL, 0);
    if (return_code == -1)
	{
		std::cout << "Failed to send the message." << std::endl;
		std::cout.flush();
		exit(EXIT_FAILURE);
	}
	else if (return_code == 0)  /* Check that the message was transferred properly */
	{
		/* If display received a termination request terminate controller */

		if (terminate)
		{
			std::cout << "Exiting Controller" << std::endl;
			std::cout.flush();
			exit(EXIT_SUCCESS);
		}
	}

	/* Disconnect from the channel */
	ConnectDetach(coid);
}

int main(int argc, char* argv[]) {
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

	system_status_t     system_message;    /* Structure to hold the system information  */
	send_msg_request_t* message_request;

	/* Clear the memory for the message and the response */
	memset(&system_message, 0, sizeof(system_status_t));
	memset(&message_request, 0, sizeof(send_msg_request_t));

	char message[400]; /* Message object to receive and send data to client */
	memset(&message, 0, sizeof(message));

	/* Initialize system status */
	system_message.system_state = -1;
	system_message.current_step = 0;

    /* Create a channel for the client to connect to */
    chid = ChannelCreate(0);
    if (chid == -1)
    {
        perror("failed to create the channel.");
        exit(EXIT_FAILURE);
    }

    /* Declare function pointer for handling states */
    void (*funcPointer)(system_status_t&);

	/* Assign pointer to initial state */
	funcPointer = &start;

    /* Program is active and ready to listen for instructions */
	funcPointer(system_message); /* Update state to 'Start' */

    /* Send message to display to inform we are ready */
	std::string msg = system_message.message;
    send_message(msg);

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

        /* Respond to input */
        switch(message_request->instruction){
        	case Input::LS:
        	case Input::RS:
				funcPointer = &scanning;
				system_message.person_id = message_request->person_id;
				break;

        	case Input::WS:
				funcPointer = &weight_scan;
				system_message.person_weight = message_request->extras;
				break;

        	case Input::LO:
        	case Input::RO:
				std::cout << "assigning function pointer to open" << std::endl;
				std::cout.flush();
				funcPointer = &opened;
				break;

        	case Input::LC:
        	case Input::RC:
				std::cout << "assigning function pointer to closed" << std::endl;
				std::cout.flush();
				funcPointer = &closed;
				break;

        	case Input::GLU:
        	case Input::GRU:
				std::cout << "assigning function pointer to guard unlocked" << std::endl;
				std::cout.flush();
				funcPointer = &unlocked;
				break;

        	case Input::GRL:
        	case Input::GLL:
				std::cout << "assigning function pointer to guard locked" << std::endl;
				std::cout.flush();
				funcPointer = &locked;
				break;
        	case Input::EXT:
        		funcPointer = &exit_program;
        		break;
        }

        /* Update state */
        funcPointer(system_message);

        /*
         * No need to send any data back to the input. Respond to unblock.
         * This unblocks the client (but doesn't return any data) and returns the EOK “success” indication.
         */
        MsgError(rcvid, EOK);

        /* Alternatively, you can unblock with regular reply and choose to not send
         * data back. Standard shown in QNX documentation encourages MsgError for pass/fail responses */
        //MsgReply (rcvid, EOK, NULL, 0);

        /* Send message to display */
        send_message(system_message.message);
    }

								/* Phase 3 */

    /* Destroy the channel when done */
    ChannelDestroy(chid);

	return 0;
}

// TODO: Get messages from a header structure container all messages
void start(system_status_t& ss){

	std::string msg = "Waiting for person...";
	strcpy(ss.message, msg.c_str());
	ss.system_state = State::START_STATE;
}

void scanning(system_status_t& ss){

	if(ss.current_step != 0){
		std::cout << "You need to be on step 0 to use this command, exiting";
		exit(0);
	}
	std::string msg = "Person scanned ID. ID = " + std::to_string(ss.person_id);
	strcpy(ss.message, msg.c_str());
	ss.system_state = State::SCANNING_STATE;
}

void locked(system_status_t& ss){
	if(ss.current_step == 0){
			std::cout << "You need to be on step 1 or 2 to use this command, exiting";
			exit(0);
		}
	std::cout << "Locked function called\n";
	std::cout.flush();
	ss.system_state = State::LOCKED_STATE;
}

void unlocked(system_status_t& ss){
	std::cout << "Unlocked function called\n";
	std::cout.flush();
	ss.system_state = State::UNLOCKED_STATE;
}

void opened(system_status_t& ss){
	std::cout << "Opened function called" << std::endl;
	std::cout.flush();
	ss.system_state = State::OPENED_STATE;
}

void weight_scan(system_status_t& ss){
    std::string msg = "Person weighed, Weight = " + std::to_string(ss.person_weight);
	strcpy(ss.message, msg.c_str());
	ss.system_state = State::WEIGHT_SCAN_STATE;
}

void closed(system_status_t& ss){
	std::cout << "Closed function called\n";
	std::cout.flush();
	ss.system_state = State::CLOSED_STATE;
}

void exit_program(system_status_t& ss){
	std::string msg = "Exit Display";
	terminate = 1;
	strcpy(ss.message, msg.c_str());
	ss.system_state = State::EXIT_STATE;
}
