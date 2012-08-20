/*
 * task_queue.h
 *
 *  Created on: Aug 5, 2012
 *      Author: freshines
 */

#ifndef TASK_QUEUE_H_
#define TASK_QUEUE_H_

#include "common.h"
#include "task.h"
#include "mysql_connector.h"

class task_queue
{
public:

	static task_queue& instance();

	bool get_task(task_t &task);

	int get_size();

	int scan_task(mysql_connector &_connector, const char *str);

	bool has_task(task_t &task);

	bool push_back_task(task_t &task);

	pthread_mutex_t task_list_lock;

	pthread_cond_t task_cond_lock;

protected:

	task_queue();

	~task_queue();

private:

	list<task_t> task_list;

};

#endif /* TASK_QUEUE_H_ */
