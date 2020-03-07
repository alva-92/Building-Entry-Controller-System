#ifndef DES_H_
#define DES_H_

#define SRVR_OK                  0
#define SRVR_UNDEFINED           1
#define SRVR_INVALID_INSTRUCTION 2


// TODO: Might need to create a person object to fulfill requirements
#define NUM_STATES 7

typedef enum
{
	START_STATE = 0,
	SCANNING_STATE = 1,
	LOCKED_STATE = 2,
	UNLOCKED_STATE = 3,
	OPENED_STATE = 4,
	WEIGHT_SCAN_STATE = 5,
	CLOSED_STATE = 6,

} State;

/**
 * Stores the system status information
 */
struct system_status {

	int person_id;
	int system_state;
	int current_step; /* The current step where the person is in the process */
	char message[128]; /* Message to be displayed to the console */

} typedef system_status_t;

/**
 * Holds the information needed by the controller
 */
struct send_msg_request {

	// Who is sending the instruction? What Instruction (ls, rs, etc.)
	int person_id;
	char instruction[128];

} typedef send_msg_request_t;

/**
 *
 */
struct response_msg {

	int status_code; // [OK, UNDEFINED, INVALID_INSTRUCTION]
	char message[128]; /* Message for the input process */ // TODO: Might not be needed as per requirements. Need to revisit this


} typedef response_msg_t;

pid_t serverpid;
int chid;  /* Holds the channel ID */
int rcvid; /* Key to delimit where the message came from (client) */
int coid;
int person_id;

// TODO: Encapsulate this so only the controller modifies it based on inputs
system_status_t system_status; /* Holds the status of the system - Shared among all programs */

/* States */
void *start();
void *scanning();
void *locked();
void *unlocked();
void *opened();
void *weight_scan();
void *closed();

#endif
