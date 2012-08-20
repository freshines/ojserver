/*
 * thread.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: freshines
 */
#include "thread.h"

const char* thread::get_name() const
{
	return _name;
}

pthread_t thread::get_tid() const
{
	return _tid;
}

void thread::set_name(const char* name)
{
	snprintf(_name, sizeof(_name), "%s", name);
}

int thread::detach()
{
    while (0 == _tid)
    {
        usleep(10000);
    }
	return pthread_detach(_tid);
}

int thread::join()
{
    while (0 == _tid)
    {
        usleep(10000);
    }
    return pthread_join(_tid, NULL);
}

int thread::start()
{
	return !(pthread_create(&_tid, NULL, run_thread, this));
}

void* thread::run_thread(void *para)
{
	thread *t = static_cast<thread *>(para);
    while (0 == t->_tid)
    {
        usleep(1000);
    }
	t->run();
	return 0;
}

