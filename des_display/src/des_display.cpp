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

#include "../../des_controller/src/des.h"

int main() {

	std::cout << "The display is running as process_id: " << getpid() << std::endl;
	std::cout.flush();
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
