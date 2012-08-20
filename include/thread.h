/*
 * thread.h
 *
 *  Created on: Aug 8, 2012
 *      Author: freshines
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "common.h"

class thread
{
public:
	thread(): _tid(0)
	{
		bzero(_name, sizeof(_name));
	}

	virtual ~thread()
	{

	}

	const char* get_name() const;

	pthread_t get_tid() const;

	void set_name(const char* name);


	int detach();

	int join();

	virtual int run() = 0;

	int start();

private:

	static void* run_thread(void* para);

	pthread_t _tid;

	char _name[256];
};

#endif /* THREAD_H_ */
