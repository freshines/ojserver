/*
 * taskmgr.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: freshines
 */

#include "taskmgr.h"
#include "stdio.h"

char MYSQL_HOST[50];
char MYSQL_USER[50];
char MYSQL_PASSWORD[50];
char MYSQL_DB[50];
char REJUDGE_CONN_STR[100];
char SUBMIT_STR[100];
char REJUDGE_STR[100];
char ERROR_REJUDGE_STR[100];
char CHALLENGE_STR[100];
char PRETEST_STR[100];
char TEST_ALL_STR[100];
int LISTEN_PORT;
int MYSQL_port;

int taskmgr::_scanning_cycle = 5;

taskmgr::taskmgr()
{
	workers = NULL;
	set_thread_num(5);
	Idle_list.clear();
	pthread_mutex_init(&Idle_list_lock, NULL);
}

taskmgr::~taskmgr()
{
	workers = NULL;
	Idle_list.clear();
	pthread_mutex_destroy(&Idle_list_lock);
}

bool taskmgr::init()
{
	bool ret = true;
	config& conf = config::instance();

	if(ret && !conf.load_conf("../conf", "ojserver.conf"))
		ret = false;
	if(ret && !my_log::instance().open_log())
		ret = false;


	if(ret && !conf.get_string("MYSQL_HOST", MYSQL_HOST, sizeof(MYSQL_HOST)))
	{
		FATAL("config::get_string(MYSQL_HOST) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("MYSQL_USER", MYSQL_USER, sizeof(MYSQL_USER)))
	{
		FATAL("config::get_string(MYSQL_USER) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("MYSQL_PASSWORD", MYSQL_PASSWORD, sizeof(MYSQL_PASSWORD)))
	{
		FATAL("config::get_string(MYSQL_PASSWORD) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("MYSQL_DB", MYSQL_DB, sizeof(MYSQL_DB)))
	{
		FATAL("config::get_string(MYSQL_DB) failed!");
		ret = false;
	}

	if(ret && !conf.get_int("MYSQL_PORT", MYSQL_port))
	{
		FATAL("config::get_int(MYSQL_PORT) failed!");
		ret = false;
	}

	if(ret && !conf.get_int("LISTEN_PORT", LISTEN_PORT))
	{
		FATAL("config::get_int(LISTEN_PORT) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("REJUDGE_CONN_STR", REJUDGE_CONN_STR, sizeof(REJUDGE_CONN_STR)))
	{
		FATAL("config::get_string(REJUDGE_CONN_STR) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("SUBMIT_STR", SUBMIT_STR, sizeof(SUBMIT_STR)))
	{
		FATAL("config::get_string(SUBMIT_STR) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("REJUDGE_STR", REJUDGE_STR, sizeof(REJUDGE_STR)))
	{
		FATAL("config::get_string(REJUDGE_STR) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("ERROR_REJUDGE_STR", ERROR_REJUDGE_STR, sizeof(ERROR_REJUDGE_STR)))
	{
		FATAL("config::get_string(ERROR_REJUDGE_STR) failed!\n");
		ret = false;
	}

	if(ret && !conf.get_string("CHALLENGE_STR", CHALLENGE_STR, sizeof(CHALLENGE_STR)))
	{
		FATAL("config::get_string(CHALLENGE_STR) failed!");
		ret = false;
	}
	if(ret && !conf.get_string("PRETEST_STR", PRETEST_STR, sizeof(PRETEST_STR)))
	{
		FATAL("config::get_string(PRETEST_STR) failed!");
		ret = false;
	}

	if(ret && !conf.get_string("TEST_ALL_STR", TEST_ALL_STR, sizeof(TEST_ALL_STR)))
	{
		FATAL("config::get_string(TEST_ALL_STR) failed!");
		ret = false;
	}

	int scanning_cycle, thread_num;
	if(ret && !conf.get_int("SCANNING_CYCLE", scanning_cycle))
	{
		FATAL("config::get_string(SCANNING_CYCLE) failed!");
	}
	else if(ret)
	{
		taskmgr::_scanning_cycle = scanning_cycle;
	}

	if(ret && !conf.get_int("THREAD_NUM", thread_num))
	{
		FATAL("config::get_string(THREAD_NUM) failed!");
	}
	else if(ret)
	{
		set_thread_num(thread_num);
	}

	writeLog("[NOTICE] Server init success.");
	if(ret)
		NOTICE("Server init success.");
	return ret;
}

void *log_task(void* arg)
{
	int t;
	while(1)
	{
		pthread_mutex_lock(&my_log::instance().log_list_lock);
		t = my_log::instance().get_buffer_size();
//		printf("buffer size: %d\n", t);
		if(t==0)
		{
			pthread_mutex_unlock(&my_log::instance().log_list_lock);
			sleep(my_log::instance().LOG_FLUSH_TIME);
		}
		else
		{
//			printf("enter log flush\n");
			my_log::instance().log_flush();
			pthread_mutex_unlock(&my_log::instance().log_list_lock);

		}
		sleep(my_log::instance().LOG_FLUSH_TIME);
	}
}

bool taskmgr::start(void *arg)
{
	pthread_t tid;
	char *str = (char *)arg;
	if (NULL == str)
		strcpy(flag, "");
	else
	{
		strncpy(flag, str, sizeof(flag));

		flag[10] = '\0';puts(flag);
	}
    pthread_create(&tid, NULL, scan_task, this);
    pthread_create(&tid, NULL, log_task, NULL);
	bool ret = true;

	workers = new(std::nothrow) work_thread[_thread_num];
	if (NULL == workers)
	{
		FATAL("alloc new work_thread failed.");
		ret = false;
	}
	for (int i=0; ret && i < _thread_num; i++)
	{
		char temp[20];
		snprintf(temp, sizeof(temp), "work_thread-%d", i);
		workers[i].set_name(temp);
		workers[i].set_id(i);
		workers[i].set_status(STATUS_UNCONN);
		workers[i].start();
		Idle_list.push_back(i);
	}

    if (ret)
    {
        NOTICE("create %d work thread success", _thread_num);
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in remote_addr;
    if (-1 == (server_fd = socket(AF_INET, SOCK_STREAM, 0)))
    {
    	    FATAL("Socket Error: %s",strerror(errno));
    	    perror("Socket() error\n");
    	    exit(1);
    }
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(LISTEN_PORT);
   server_addr.sin_addr.s_addr = INADDR_ANY;
   bzero(&(server_addr.sin_zero), 8);
   if (-1 == bind(server_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)))
   {
	   FATAL("Bind Error: %s\n",strerror(errno));
	   printf("Bind Error: %s\n",strerror(errno));
	   exit(1);
   }

   if (-1 == listen(server_fd, _thread_num))
   {
	   FATAL("Listen Error: %s",strerror(errno));
	   perror("Listen() error\n");
	   exit(1);
   }
   socklen_t sin_size = sizeof(struct sockaddr_in);
   while (ret)
   {
	   if ((client_fd = accept(server_fd, (struct sockaddr *)& remote_addr, &sin_size)) == -1)
	   {
		   FATAL("Accept() error: %s", strerror(errno));
		   continue;
	   }
	   NOTICE("received connection client_fd-%d from %s:%d.", client_fd, inet_ntoa(remote_addr.sin_addr), remote_addr.sin_port);
//	   printf("received connection client_fd-%d from %s:%d.\n", client_fd, inet_ntoa(remote_addr.sin_addr), remote_addr.sin_port);

	   int t;
	   pthread_mutex_lock(&Idle_list_lock);
	   if (-1 != (t = get_Idle_thread()))
	   {
			  workers[t].set_fd(client_fd);
			  pthread_mutex_lock(&workers[t]._thread_mutex_lock);
				  workers[t].set_status(STATUS_IDLE);
				  pthread_cond_signal(&workers[t]._thread_cond_lock);
			  pthread_mutex_unlock(&workers[t]._thread_mutex_lock);
			  NOTICE("%s take care of client_fd-%d.\n", workers[t].get_name(), client_fd);
	   }
	   else
	   {
		   NOTICE("Too many clients. Close client_fd-%d.\n", client_fd);
	   }
	   pthread_mutex_unlock(&Idle_list_lock);
   }
   close(server_fd);
   return ret;
}

void* taskmgr::scan_task(void *arg)
{
	taskmgr *mgr = (taskmgr *)(arg);
	bool ret = true;
	mysql_connector _connector;
    if (ret && !_connector.connect())
    {
        FATAL("connect to mysql error");
        ret = false;
    }
	while(ret)
	{
		pthread_mutex_lock(&mgr->Idle_list_lock);
		for (int i=0; i < mgr->_thread_num; i++)
		{
			if(mgr->workers[i].get_status() == STATUS_UNCONN && find(mgr->Idle_list.begin(), mgr->Idle_list.end(), i)==mgr->Idle_list.end())
			{
				mgr->Idle_list.push_back(i);
				NOTICE("workers %d back to Idle_list.", i);
			}
		}
		pthread_mutex_unlock(&(mgr->Idle_list_lock));
		int t = task_queue::instance().scan_task(_connector, mgr->flag);
		if(t)
			   NOTICE("Scan finish. Fetch %d new tasks.", t);

		//printf("Scan finish. Fetch %d new tasks.\n", t);
		sleep(taskmgr::_scanning_cycle);
	}
}

