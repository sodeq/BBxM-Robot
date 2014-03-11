/*
 * threadpp.hpp
 *
 *  Created on: Mar 9, 2014
 *      Author: lynx
 */

#ifndef THREADPP_HPP_
#define THREADPP_HPP_

#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

using namespace std;

#define THREAD_DELAY_1s 10000000
#define THREAD_DELAY_2s 20000000


void *thread_talk(void *arg_numeric) {
	int a = *(static_cast<int*>(arg_numeric));
	sleep(a+1);
	cout << "Thread: " << a << " has finished\n";
	pthread_exit(NULL);
	return 0;
}

void *function1(void* arg) {
	printf("FUNCTION-1 --> ");
	cout << *(reinterpret_cast<string*>(arg)) << endl;
	sleep(2);
	return 0;
}

void *function2(void* arg) {
	printf("FUNCTION-2 --> ");
	cout << *(reinterpret_cast<string*>(arg)) << endl;
	return 0;
}

void* do_loop(void* data) {
	int i,	// counter to print numbers
		j;	// counter for delay
	int me = *((int*)data);	// thread identifying number

	for(i = 0; i < 10; ++i) {
		for(j = 0; j < 1000000; ++j); /* delay loop */
		printf("'%d' - Got '%d'\n", me, i );
	}
	// now terminate the thread
	pthread_exit(NULL);
}

// global mutex
pthread_mutex_t a_mutex = PTHREAD_MUTEX_INITIALIZER;

struct employee {
		int number, id;
		char firstName[20];
		char lastName[30];
		char dept[30];
		int roomNumber;
	};

void copyEmployee(struct employee* src, struct employee* dst) {
	int rc;	// mutex lock/unlock result
	/* first! need to lock mutex,
	 * to assure exclusive access to 'a' and 'b'
	 */
	rc = pthread_mutex_lock(&a_mutex);
	dst->number = src->number;
	dst->id = src->id;
	strcpy(dst->firstName, src->firstName);
	strcpy(dst->lastName, src->lastName);
	strcpy(dst->dept, src->dept);
	dst->roomNumber = src->roomNumber;

	/* unlock mutex */
	rc = pthread_mutex_unlock(&a_mutex);
}

#endif /* THREADPP_HPP_ */



















