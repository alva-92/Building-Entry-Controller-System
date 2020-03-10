/**
 * @name        des_display.cpp
 * @description Displays the status of the system – which door is open /
 *              closed, if there is a user waiting to enter from the left or right, etc. The program should run in
 *              the background and print out status information to the console each time a status update is sent
 *              to it using a message from the des_controller program. The des_display program can print out
 *              its process id when it first starts up
 * @author(s):  Gerardo Enrique Alvarenga
 *              Chris Bishop
 */

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
#include <sys/dispatch.h>

#include "../../des_controller/src/des.h"

int main() {

	std::cout << "The display is running as PID: " << getpid() << std::endl;
	std::cout.flush();

	system_status_t*    message_request;
	response_msg_t      response_message;
	char message[400]; /* Message object to receive and send data to client */

    /* Create a channel for the controller to connect to */
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

        message_request = (system_status_t*) message;

        std::cout << "Display: " << message_request->person_id << std::endl;
        std::cout.flush();

        response_message.status_code = SRVR_OK;

        /*
         * rcvid  - The receive ID that MsgReceive*() returned when you received the message.
         * status - The status to use when unblocking the MsgSend*() call in the rcvid thread.
         * msg    - A pointer to a buffer that contains the message that you want to reply with.
         * size   - The size of the message, in bytes.
         */
        MsgReply(rcvid, EOK, (void*) &response_message, sizeof(response_msg_t));
    }

	/* Phase 3 */

	/* Destroy the channel when done */
    std::cout << "Destroying display channel"<< std::endl;
    std::cout.flush();
	ChannelDestroy(chid);

	return 0;
}


/**
 * Prints out status information to the console each time a status update is sent
 * to it using a message from the des_controller program
 *
 *TODO: Validate that cout is working on QNX - There has been some issues
 * with the buffer not flushing properly. If that is the case, switch to printf
 */
void print_message(std::string message){
	std::cout << "Message received: " << message << std::endl;
	std::cout.flush();
	//TODO: Implementation
}
