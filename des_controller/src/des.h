#ifndef DES_H_
#define DES_H_

#define SRVR_OK                  0
#define SRVR_UNDEFINED           1
#define SRVR_INVALID_INSTRUCTION 2


// TODO: Might need to create a person object to fulfill requirements

#define NUM_INPUTS 12
#define NUM_STATES 8

typedef enum
{
	LS = 0,
	RS = 1,
	WS = 2,
	LO = 3,
	RO = 4,
	LC = 5,
	RC = 6,
	GLU = 7,
	GRU = 8,
	GRL = 9,
	GLL = 10,
	EXT = 11,

} Input;

const char* INSTRUCTIONS[NUM_INPUTS] = {
		"ls",
		"rs",
		"ws",
		"lo",
		"ro",
		"lc",
		"rc",
		"gru",
		"grl",
		"gll",
		"glu"
};


typedef enum
{
	START_STATE       = 0,
	SCANNING_STATE    = 1,
	LOCKED_STATE      = 2,
	UNLOCKED_STATE    = 3,
	OPENED_STATE      = 4,
	WEIGHT_SCAN_STATE = 5,
	CLOSED_STATE      = 6,
	EXIT_STATE        = 7,

} State;

/**
 * Stores the system status information
 */
struct system_status {

	int person_id;
	int person_weight;
	int system_state;
	int current_step; /* The current step where the person is in the process */
	char message[128]; /* Message to be displayed to the console */

} typedef system_status_t;

/**
 * Holds the information needed by the controller
 */
struct send_msg_request {

	int person_id;
	int instruction;
	int extras;  /* For this application it holds the weight of the person */

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
int terminate; /* Set to 1 to terminate the program */

// TODO: Encapsulate this so only the controller modifies it based on inputs
system_status_t system_status; /* Holds the status of the system - Shared among all programs */

/* States */
void start(system_status_t&);
void scanning(system_status_t&);
void locked(system_status_t&);
void unlocked(system_status_t&);
void opened(system_status_t&);
void weight_scan(system_status_t&);
void closed(system_status_t&);
void exit_program(system_status_t&);
#endif
