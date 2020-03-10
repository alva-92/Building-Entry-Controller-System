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


void send_message(int input){

	/* Prepare the message */
	send_msg_request_t msg_req;    /* Struct to be sent to the server */
	response_msg_t* response_message;

	/* Clear the memory for the message and the response */
	memset( &msg_req, 0, sizeof(send_msg_request_t));
	memset( &response_message, 0, sizeof(response_msg_t));


	/* Establish a connection */
	int  coid;
	char resp_msg [200]; 		     /* Response message buffer */
	memset( &resp_msg, 0, sizeof(resp_msg));

	msg_req.instruction = 7;
	/*
	 * @params
	 * nd The node descriptor of the node (e.g. ND_LOCAL_NODE for the local node) on which the process that owns the channel is running; see “Node descriptors,” below.
	 * pid The process ID of the owner of the channel. If pid is zero, the calling process is assumed.
	 * chid The channel ID, returned by ChannelCreate(), of the channel to connect to the process.
	 * index The lowest acceptable connection ID.
	 * flags If flags contains _NTO_COF_CLOEXEC, the connection is closed when your process calls an exec
	 */
	std::cout << "Connecting to: " <<  serverpid << " Sending\n ID:" << msg_req.person_id << " Instruction: " << msg_req.instruction << std::endl;
	std::cout.flush();
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
	if (MsgSend(coid, &msg_req, sizeof(send_msg_request_t), resp_msg, sizeof(response_msg_t)) == -1)
	{
		std::cout << "Failed to send the message" << std::endl;
		std::cout.flush();
		exit(EXIT_FAILURE);
	}

	response_message = (response_msg_t*) resp_msg;

	std::cout << "Input got response: " << response_message->status_code << std::endl;
	std::cout.flush();

}

void process_input(std::string in){

	int inst_code = -1;

	// TODO: Update all other status codes e.g. rc, ws and validate data
	if (in == "ls" || in == "rs"){
		std::cout << "Enter the Person's ID:" << std::endl;
		std::cout.flush();
		if (in == "ls"){
			inst_code = Input::LS;
		} else {
			inst_code = Input::RS;
		}
	}

	//message_request.instruction = inst_code;

	send_message(inst_code);
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
	std::cout << "Enter the type of event: e (ls= left scan, rs= right scan, "
			"ws= weight scale, lo =left open, ro=right open, "
			"lc =left closed, rc = right closed , "
			"gru = guard right unlock, grl = guard right lock, "
			"gll=guard left lock, "
			"glu = guard Left unlock)" << std::endl;
	std::cout.flush();

	std::string input;
	std::cin >> input;

	process_input(input);
	return 0;
}

