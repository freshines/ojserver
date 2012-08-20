/*
 * work_thread.h
 *
 *  Created on: Aug 8, 2012
 *      Author: freshines
 */

#ifndef WORK_THREAD_H_
#define WORK_THREAD_H_
#include "common.h"
#include "thread.h"
#include "task.h"
#include "task_queue.h"
#include "mysql_connector.h"

#define MAX_JUDGER_NUMBER 255
#define MAX_DATA_SIZE 855350
#define CHECK_STATUS 1
#define NEED_JUDGE 2
#define SEND_DATA 3
#define DO_CHALLENGE 4
#define DO_PRETEST 5
#define DO_TESTALL 6
#define JUDGER_STATUS_REPORT 1
#define NEED_DATA 2
#define RESULT_REPORT 3
#define CHALLENGE_REPORT 4
#define CPPLANG 1
#define CLANG 2
#define JAVALANG 3
#define FPASLANG 4
#define AC_STATUS 0
#define CE_STATUS 1
#define RE_STATUS 2
#define WA_STATUS 3
#define TLE_STATUS 4
#define MLE_STATUS 5
#define PE_STATUS 6
#define STATUS_UNCONN 0
#define STATUS_IDLE 1
#define STATUS_BUSY 2

class work_thread : public thread
{
public:
	work_thread();

	~work_thread();

	void set_status(const int status)
	{
		_status = status;
	}

	int get_status()
	{
		return _status;
	}

	int get_fd()
	{
		return _fd;
	}

	int set_fd(const int fd)
	{
		_fd = fd;
	}

	int set_id(const int id)
	{
		_id = id;
	}

	int get_id()
	{
		return _id;
	}

	int run();

	void judger_thread();

	bool dealneed_judge(task_t &task);

	bool result_dealer(char filename[200],int temp_pid,int temp_runid,int temp_cid,char temp_username[]);

	char REJUDGE_CONN_STR[100];
	char SUBMIT_STR[100];
	char REJUDGE_STR[100];
	char ERROR_REJUDGE_STR[100];
	char CHALLENGE_STR[100];
	char PRETEST_STR[100];
	char TEST_ALL_STR[100];
	int LISTEN_PORT;

	pthread_cond_t _thread_cond_lock;
	pthread_mutex_t _thread_mutex_lock;

private:

	int _fd;

	int _id;

	int _status;

	mysql_connector _connector;



};

#endif /* WORK_THREAD_H_ */
