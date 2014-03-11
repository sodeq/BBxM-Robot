/*
 * threading.cpp
 *
 *  Created on: Jan 16, 2014
 *      Author: lynx
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#ifdef __cplusplus
}
#endif

#include <iostream>

#include "threadpp.hpp"

#define NUM_OF_THREADS 5
#define COUNT_DONE 10

#define DEBUG

using namespace std;

// MUTEX creation to avoid one global variable accessed at the same time
pthread_mutex_t mutex_var = PTHREAD_MUTEX_INITIALIZER;
int test_var = 10;
int count = 0;

// MUTEX conditionvariables
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

/* global variable -- 2 employees */
struct employee employees[] = {
	{1, 123, "employee", "one", 101},
	{2, 234, "karyawan", "dua", 202}
};

int main(int n, char** arg) {
	pthread_t t1, t2;

	string msg1("fun1");
	string msg2("fun2");
	int create1 = pthread_create(&t1, NULL, function1, reinterpret_cast<void*>(&msg1));
	if(create1 != 0)
		cout << "Error thread1\n";

	int create2 = pthread_create(&t2, NULL, function2, reinterpret_cast<void*>(&msg2));
	if(create2 != 0)
		cout <<"Error thread2\n";

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	// now test for do_loop function
#ifdef DEBUG
	printf("NOW TEST FOR DO_LOOP\n");
#endif /* DEBUG */

	int thr_id;
	pthread_t	myThread;
	int a	=1;
	int b	=2;

	thr_id = pthread_create(
				&myThread,	/* thread to run*/
				NULL, 		/* set attributes for the new thread */
				do_loop, 	/* function to run in myThread */
				(void*)&a	/* argument to pass to the function */
				);

//	do_loop((void*)&b);

	employee karyawan_baru, *new_empl;
	new_empl = &karyawan_baru;

	/*copyEmployee(employees,
				 karyawan_baru);*/

	printf("%s\n", karyawan_baru.firstName);

	return 0;
}













