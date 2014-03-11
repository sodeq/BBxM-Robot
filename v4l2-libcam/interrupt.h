/*
 * interrupt.h
 *
 *  Created on: Jan 19, 2014
 *      Author: lynx
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

bool run = false;
void sigcatch (int sig)
{
	fprintf (stderr, "Exiting...\n");
	run = false;
}



#endif /* INTERRUPT_H_ */
