/**
 * @name        des_display.cpp
 * @description Prompts the user for inputs to the controller. This
 *              program is simulating all of the input events from the devices; e.g., card readers, door latches,
 *              scale.
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


void send_message(send_msg_request_t& msg_req, response_msg_t* response_message){

	/* Establish a connection */
	int  coid;

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

    memcpy(buffer, &msg_req, sizeof(send_msg_request_t));

    /* Send the message */

	/*
     * @params
     * the connection ID of the target server (coid),
     * a pointer to the send message (smsg),
     * the size of the send message (sbytes),
     * a pointer to the reply message (rmsg), and
     * the size of the reply message (rbytes).
     */
    int return_code = -1;
    return_code = MsgSend(coid, &msg_req, sizeof(send_msg_request_t), NULL, 0);
	if (return_code == -1)
	{
		// TODO: For future - Check for other error return codes
		std::cout << "Failed to send the message to controller" << std::endl;
		std::cout.flush();
		exit(EXIT_FAILURE);
	}
	else if (return_code == 0)  /* Check that the message was transferred properly */
	{
		/* If controller received a termination request terminate input */

		if (msg_req.instruction == Input::EXT)
		{
			terminate = 1;
		}
	}
}

void process_input(std::string in){

	/* Prepare the message */
	send_msg_request_t msg_req;    /* Struct to be sent to the server */
	/* Clear the memory for the message */
	memset( &msg_req, 0, sizeof(send_msg_request_t));

	response_msg_t* response_message;

	int inst_code = -1;

	if (in == "ls" || in == "rs"){
		std::cout << "Enter the Person's ID:" << std::endl;
		std::cout.flush();
		std::cin >> msg_req.person_id;
		if (in == "ls"){
			inst_code = Input::LS;
		} else {
			inst_code = Input::RS;
		}
	}else if(in == "ws"){
		std::cout << "Enter the Person's weight:" << std::endl;
		std::cout.flush();
		std::cin >> msg_req.extras;
		inst_code = Input::WS;
	}else if(in == "lo"){
		inst_code = Input::LO;
	}else if(in == "ro"){
		inst_code = Input::RO;
	}else if(in == "lc"){
		inst_code = Input::LC;
	}else if(in == "rc"){
		inst_code = Input::RC;
	}else if(in == "glu"){
		inst_code = Input::GLU;
	}else if(in == "gru"){
		inst_code = Input::GRU;
	}else if(in == "grl"){
		inst_code = Input::GRL;
	}else if(in == "gll"){
		inst_code = Input::GLL;
	}else if(in == "exit" || in == "EXIT"){
		inst_code = Input::EXT;
	}

	msg_req.instruction = inst_code;

	send_message(msg_req, response_message);
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

	while (!terminate) {
		std::cout << "Enter the event type: (ls = left scan, rs = right scan, "
				"ws = weight scale, lo = left open, ro = right open, "
				"lc = left closed, rc = right closed, "
				"gru = guard right unlock, grl = guard right lock, "
				"gll = guard left lock, "
				"glu = guard Left unlock, exit = exit programs)" << std::endl;

		std::cout.flush();

		std::string input;
		std::cin >> input;

		process_input(input);
	}
	return 0;
}

