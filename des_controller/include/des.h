#pragma once

#define SRVR_OK                  0
#define SRVR_UNDEFINED           1
#define SRVR_INVALID_INSTRUCTION 2

/**
 * Stores the system status information
 */
struct system_status {

	//TODO: Define what information we need to track
	//TODO: e.g. Someone inside of the building, in what state is the system at, etc

} typedef system_status_t;

/**
 * Holds the information needed by the controller
 */
struct send_msg_request {

	//TODO: Define what information we need to send
	//TODO: e.g. Who is sending the instruction? What Instruction (ls, rs, etc.)
	int person_id;
	char* instruction;

} typedef send_msg_request_t;

/**
 *
 */
struct response_msg {

	//TODO: Define what information we need to send - example below
	int status_code; // [OK, UNDEFINED, INVALID_INSTRUCTION]
	char message[128]; /* Message to be displayed to the console */


} typedef response_msg_t;



class des_controller{

public:

private:

	int coid;
	int person_id;

	/* States */
	void *start();
	void *scanning();
	void *locked();
	void *unlocked();
	void *opened();
	void *weight_scan();
	void *closed();

};
