/*
 * taskmgr.h
 *
 *  Created on: Aug 9, 2012
 *      Author: freshines
 */

#ifndef TASKMGR_H_
#define TASKMGR_H_

#include "common.h"
#include "work_thread.h"

class taskmgr
{
public:

	char flag[10]; //NULL is argv[1]

	taskmgr();

	~taskmgr();

	bool init();

	bool start(void *arg);

	static void* scan_task(void *arg);

	void set_thread_num(int num)
	{
		_thread_num = num;
	}

	int get_Idle_thread()
	{
		if (Idle_list.size() <= 0)
			return -1;
		int t = Idle_list.front();
		Idle_list.pop_front();
		return t;
	}
	work_thread *workers;
	void set_scanning_cycle(int scanning_cycle)
	{
		_scanning_cycle = scanning_cycle;
	}

	void writeLog(const char *templog)
	{
		time_t CurTime;
		struct tm TmTime;
		time(&CurTime);
		localtime_r(&CurTime, &TmTime);
		printf("%4d-%02d-%02d %02d:%02d:%02d %s\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec, templog);
	}



private:

    int _thread_num;

	static int _scanning_cycle;

	list<int> Idle_list;

	pthread_mutex_t Idle_list_lock;




};

#endif /* TASKMGR_H_ */
