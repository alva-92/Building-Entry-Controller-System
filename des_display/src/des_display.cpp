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

/**
 * Prints out status information to the console each time a status update is sent
 * to it using a message from the des_controller program
 *
 */
void print_message(std::string msg){

	std::cout << "Display: " << msg << std::endl;
	std::cout.flush();
}

int main() {

	std::cout << "The display is running as PID: " << getpid() << std::endl;
	std::cout.flush();

	char message[400];     /* Message object to receive data from the controller */

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

        // TODO: Remove the 'Display tag' once we are good/verified with provided screenshots
        std::cout << "Display: " << message << std::endl;
        std::cout.flush();

        /*
         * No need to send any data back to the controller. Respond to unblock.
         * This unblocks the client (but doesn't return any data) and returns the EOK “success” indication.
         */
        MsgError(rcvid, EOK);

        /* Alternatively, you can unblock with regular reply and choose to not send
         * data back. Standard shown in QNX documentation encourages MsgError for pass/fail responses */
        //MsgReply (rcvid, EOK, NULL, 0);
    }

										/* Phase 3 */

	/* Destroy the channel when done */
	ChannelDestroy(chid);

	return 0;
}
