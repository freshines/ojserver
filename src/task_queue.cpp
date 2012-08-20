/*
 * task_queue.cpp
 *
 *  Created on: Aug 5, 2012
 *      Author: freshines
 */
#include "task_queue.h"
#include "work_thread.h"

task_queue::task_queue()
{
	task_list.clear();
	pthread_mutex_init(&task_list_lock, NULL);
	pthread_cond_init(&task_cond_lock, NULL);
}

task_queue::~task_queue()
{
	pthread_mutex_destroy(&task_list_lock);
	pthread_cond_destroy(&task_cond_lock);
}


task_queue& task_queue::instance()
{
	static task_queue queue;
	return queue;
}

bool task_queue::get_task(task_t &task)
{
	bool ret = true;
	if (task_list.size() == 0)
	{
		ret = false;
	}
	else
	{
		task = task_list.front();
		task_list.pop_front();
	}

	return ret;
}

int task_queue::get_size()
{
	return task_list.size();
}

int task_queue::scan_task(mysql_connector &_connector, const char *str)
{
	bool ret = true;
	task_t task;
	int cnt = 0;
    char query[200]={0};
    mysql_result result_set;
   if (strcmp(str,"sj")==0)
    {
        sprintf(query,"SELECT runid,vname FROM status,problem WHERE (result='Waiting' or result='Judging' or result='Rejudging') and status.pid=problem.pid order by runid");
    }
    else
	{
		sprintf(query,"SELECT runid,vname FROM status,problem WHERE result='Waiting' and status.pid=problem.pid order by runid");
	}
   if (ret && !_connector.execute(query, strlen(query)))
   {
       WARNING("execute sql from databse error, %s", _connector.error());
       ret = false;
   }

   if (ret && !_connector.fetch_result(result_set))
   {
       WARNING("get result info from databse error, %s", _connector.error());
       ret = false;
   }
   char ** row = NULL;

	pthread_mutex_lock(&task_list_lock);
	while (row = result_set.fetch_row())
	{
		task.set_runid(atoi(row[0]));
		task.set_vname(row[1]);
		task.set_type(NEED_JUDGE);
		if(!has_task(task))
		{
			push_back_task(task);
			cnt++;
		}
	}
	if(get_size())
	{
		pthread_cond_signal(&task_cond_lock);
	}
	pthread_mutex_unlock(&task_list_lock);
    return cnt;
}

bool task_queue::has_task(task_t &task)
{
	bool ret = false;
	list<task_t>::iterator it;
	for (it = task_list.begin(); it != task_list.end(); it++)
	{
		if((*it).get_runid() == task.get_runid())
		{
			ret = true;
		}
	}
	return ret;
}

bool task_queue::push_back_task(task_t &task)
{
	bool ret = true;
	task_list.push_back(task);
	return ret;
}
