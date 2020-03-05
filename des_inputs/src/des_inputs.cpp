/**
 * @name        des_display.cpp
 * @description Prompts the user for inputs to the controller. This
 *              program is simulating all of the input events from the devices; e.g., card readers, door latches,
 *              scale.
 * @author(s):  Gerardo Enrique Alvarenga
 *              Chris Bishop
 */

#include <iostream>

#include "../../des_controller/include/des.h"

int main() {
	std::cout << "Enter the type of event: e (ls= left scan, rs= right scan, "
			"ws= weight scale, lo =left open, ro=right open, "
			"lc =left closed, rc = right closed , "
			"gru = guard right unlock, grl = guard right lock, "
			"gll=guard left lock, "
			"glu = guard eft unlock)" << std::endl;
	std::cout.flush();
	return 0;
}
