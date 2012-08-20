/*
 * work_thread.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: freshines
 */

#include "work_thread.h"


work_thread::work_thread()
{
	pthread_mutex_init(&_thread_mutex_lock, NULL);
	pthread_cond_init(&_thread_cond_lock, NULL);
	bool ret = true;
	config& conf = config::instance();
	_fd = 0;
	_status = STATUS_UNCONN;

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
}

// dctor
work_thread::~work_thread()
{
}

int work_thread::run()
{
//	printf("go into run().");
	bool ret = true;
    if (ret && !_connector.connect())
    {
        FATAL("connect to mysql error");
        ret = false;
    }
	task_t task;
	while(1)
	{
		pthread_mutex_lock(&_thread_mutex_lock);
		while(get_status() == STATUS_UNCONN)
		{
			pthread_cond_wait(&_thread_cond_lock, &_thread_mutex_lock);
		}
		pthread_mutex_unlock(&_thread_mutex_lock);
		int fd = _fd;
		char buffer[255]={0};
		int tnum = _id;
		struct timeval case_startv,case_nowv;
		struct timezone case_startz,case_nowz;
		gettimeofday(&case_startv,&case_startz);
		int time_passed;
		while (1)
		{
			usleep(1000);
			gettimeofday(&case_nowv,&case_nowz);
			time_passed=(case_nowv.tv_sec-case_startv.tv_sec)*1000+(case_nowv.tv_usec-case_startv.tv_usec)/1000;
			if (recv(fd,buffer,255,MSG_DONTWAIT)>0||time_passed>5000) break;
		}
		char connect_type[50]={0};
		sscanf(buffer,"%s",connect_type);
//		printf("word_thread::run current fd:%d %s\n",fd, buffer);
		if (strcmp(connect_type, SUBMIT_STR)==0)
		{
			int runid;
			char vname[100];
			sscanf(buffer,"%s%d%s",connect_type,&runid,vname);
			char templog[200]={0};
			sprintf(templog,"received a submit, runid: %s:%d",vname, runid);
			NOTICE(templog);

			task.set_runid(runid);
			task.set_type(NEED_JUDGE);
			task.set_vname(vname);

			pthread_mutex_lock(&task_queue::instance().task_list_lock);
			task_queue::instance().push_back_task(task);
			pthread_cond_signal(&task_queue::instance().task_cond_lock);
			pthread_mutex_unlock(&task_queue::instance().task_list_lock);
		}
		else if (strcmp(connect_type, PRETEST_STR)==0)
		{
			int runid;
			char vname[100];
			sscanf(buffer,"%s%d%s",connect_type,&runid,vname);
			char templog[200]={0};
			sprintf(templog,"received a pretest, runid: %s:%d", vname, runid);
			NOTICE(templog);

			task.set_runid(runid);
			task.set_type(DO_PRETEST);
			task.set_vname(vname);

			pthread_mutex_lock(&task_queue::instance().task_list_lock);
			task_queue::instance().push_back_task(task);
			pthread_cond_signal(&task_queue::instance().task_cond_lock);
			pthread_mutex_unlock(&task_queue::instance().task_list_lock);
		}
		else if (strcmp(connect_type,ERROR_REJUDGE_STR)==0)
		{
			int runid;
			char vname[100];
			sscanf(buffer,"%s%d%s",connect_type,&runid,vname);
			char templog[200]={0};
			sprintf(templog,"received a error rejudge, runid: %d",runid);
			NOTICE(templog);

			task.set_runid(runid);
			task.set_type(NEED_JUDGE);
			task.set_vname(vname);

			pthread_mutex_lock(&task_queue::instance().task_list_lock);
			task_queue::instance().push_back_task(task);
			pthread_cond_signal(&task_queue::instance().task_cond_lock);
			pthread_mutex_unlock(&task_queue::instance().task_list_lock);
		}
		else if (strcmp(connect_type,CHALLENGE_STR)==0) {
			int cha_id;
			char vname[100];
			sscanf(buffer,"%s%d%s",connect_type,&cha_id,vname);
			char templog[200]={0};
			sprintf(templog,"received a challenge, cha_id: %s:%d", vname, cha_id);
			NOTICE(templog);

			task.set_cha_id(cha_id);
			task.set_type(DO_CHALLENGE);
			task.set_vname(vname);

			pthread_mutex_lock(&task_queue::instance().task_list_lock);
			task_queue::instance().push_back_task(task);
			pthread_cond_signal(&task_queue::instance().task_cond_lock);
			pthread_mutex_unlock(&task_queue::instance().task_list_lock);

		}
		else if (strcmp(connect_type,REJUDGE_CONN_STR)==0) {
			char templog[200]={0};
			char vname[100];
			sscanf(buffer,"%s%s",connect_type,vname);
			sprintf(templog,"judger %d : %s connected.",tnum,vname);
//			printf("judger %d : %s connected. \n",tnum,vname);

			NOTICE(templog);
			set_status(STATUS_IDLE);
	//        if(get_status == STATUS_IDLE)
			   judger_thread();
			sprintf(templog,"judger %d : %s finished.",tnum,vname);
			NOTICE(templog);

		}
		else if (strcmp(connect_type,REJUDGE_STR)==0) {
			int repid,recid;
			sscanf(buffer,"%s%d%d",connect_type,&repid,&recid);
	//        MYSQL_RES *res;

			char sql[200]={0};
			mysql_result result_set;
			sprintf(sql,"SELECT runid,vname FROM status,problem WHERE result='Rejudging' and contest_belong=%d and status.pid=%d and status.pid=problem.pid order by runid",recid,repid);

			if (ret && !_connector.execute(sql, strlen(sql)))
			{
				WARNING("execute sql from databse error, %s", _connector.error());
				ret = false;
			}

			if (ret && !_connector.fetch_result(result_set))
			{
				WARNING("get result info from databse error, %s", _connector.error());
				ret = false;
			}
			int rejudge_num=0;
			char ** row = NULL;
			pthread_mutex_lock(&task_queue::instance().task_list_lock);

			while (ret && (row = result_set.fetch_row()))
			{
				rejudge_num++;
				task.set_runid(atoi(row[0]));
				task.set_vname(row[1]);
				task.set_type(NEED_JUDGE);
				if(!task_queue::instance().has_task(task))
				{
					task_queue::instance().push_back_task(task);
				}
			}
			pthread_cond_signal(&task_queue::instance().task_cond_lock);
			pthread_mutex_unlock(&task_queue::instance().task_list_lock);
			char templog[200]={0};
			sprintf(templog,"received a rejudge request, pid: %d, cid: %d, num: %d",repid,recid,rejudge_num);
			NOTICE(templog);
		}
		else if (strcmp(connect_type,TEST_ALL_STR)==0)
		{
			int recid;
			sscanf(buffer,"%s%d",connect_type,&recid);

			char sql[200]={0};
			mysql_result result_set;
			sprintf(sql,"SELECT runid,vname FROM status,problem WHERE result='Testing' and contest_belong=%d and status.pid=problem.pid order by runid",recid);
			if (ret && !_connector.execute(sql, strlen(sql)))
			{
				WARNING("execute sql from databse error, %s", _connector.error());
				ret = false;
			}

			if (ret && !_connector.fetch_result(result_set))
			{
				WARNING("get result info from databse error, %s", _connector.error());
				ret = false;
			}
			int rejudge_num=0;
			char ** row = NULL;
			pthread_mutex_lock(&task_queue::instance().task_list_lock);

			while (ret && (row = result_set.fetch_row()))
			{
				rejudge_num++;
				task.set_runid(atoi(row[0]));
				task.set_vname(row[1]);
				task.set_type(DO_TESTALL);
				if(!task_queue::instance().has_task(task))
				{
					task_queue::instance().push_back_task(task);
				}
			}
			pthread_cond_signal(&task_queue::instance().task_cond_lock);
			pthread_mutex_unlock(&task_queue::instance().task_list_lock);
			char templog[200]={0};
			sprintf(templog,"received a test all request, cid: %d, num: %d",recid,rejudge_num);
			NOTICE(templog);
		}
		else
		{
			WARNING("Illegal connection! Server Recieved: %s.", buffer);
		}
		close(fd);
		set_status(STATUS_UNCONN);
		} //end of while
	}


void work_thread::judger_thread()
{
//	printf("%s go into the judger_thread.\n", get_name());
	bool ret = true;
	task_t task;
	char templog[200];
	while (get_status() == STATUS_IDLE)
	{
		pthread_mutex_lock(&task_queue::instance().task_list_lock);
		while (!task_queue::instance().get_task(task))
		{
			pthread_cond_wait(&task_queue::instance().task_cond_lock, &task_queue::instance().task_list_lock);
		}
		pthread_mutex_unlock(&task_queue::instance().task_list_lock);
		if (task.get_type()==DO_CHALLENGE)
			sprintf(templog,"%s Fetched RunID %d.", get_name(), task.get_runid());
		else if (task.get_type()==NEED_JUDGE)
			sprintf(templog,"%s Fetched RunID %d.", get_name(), task.get_runid());
		else if (task.get_type()==DO_PRETEST)
			sprintf(templog,"%s Fetched RunID %d.", get_name(), task.get_runid());
		NOTICE(templog);

		if (task.get_type()==NEED_JUDGE || task.get_type()==DO_PRETEST || task.get_type()==DO_TESTALL)
		{
			set_status(STATUS_BUSY);
			dealneed_judge(task);
		}
		else if (task.get_type()==DO_CHALLENGE)
		{
			set_status(STATUS_BUSY);
//			dealdo_challenge(arg, task);
		}
//		NOTICE("%s Fetched RunID %d.\n", get_name(), task.get_runid());
	}
}


bool work_thread::dealneed_judge(task_t &task)
{
	bool ret = true;
    char buffer[MAX_DATA_SIZE]={0};
    int tnum=_id;
    int tfd=_fd;

    char templog[20000]={0};
//    sprintf(templog,"Run fetched to work_thread-%d", tnum);
//    NOTICE(templog);
    char sql[200]={0};
    mysql_result result_set;
    sprintf(sql,"SELECT status.source,status.runid,status.language,status.pid,problem.ignore_noc FROM status,problem WHERE status.pid=problem.pid and runid=%d", task.get_runid());

    if (ret && !_connector.execute(sql, strlen(sql)))
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
    row = result_set.fetch_row();
    if (task.get_type()==NEED_JUDGE&&strcmp(row[4],"1")==0)
    	task.set_type(DO_TESTALL);
    char filename[200]={0};
    sprintf(filename,"raw_files/%d.bott",task.get_runid());
    NOTICE("FILENAME: %s", filename);
    FILE* datafile=fopen(filename,"w");
    if (datafile==NULL)
    {
    	char tmp[100];
        sprintf(tmp,"CANNOT OPEN FILE %s!!!",filename);
        WARNING(tmp);
    }
    fprintf(datafile,"<type> %d\n", task.get_type());
    fprintf(datafile,"__SOURCE-CODE-BEGIN-LABLE__\n");
    fprintf(datafile,"%s\n",row[0]);
    fprintf(datafile,"__SOURCE-CODE-END-LABLE__\n");
    fprintf(datafile,"<runid> %s\n<language> %s\n<pid> %s\n",row[1],row[2],row[3]);
    int temp_pid=atoi(row[3]);
//    mysql_free_result(res);
    sprintf(sql,"SELECT number_of_testcase,time_limit,case_time_limit,memory_limit,special_judge_status,vname,vid FROM problem WHERE pid=%d",temp_pid);
    if (ret && !_connector.execute(sql, strlen(sql)))
    {
        WARNING("execute sql from databse error, %s", _connector.error());
        ret = false;
    }

    if (ret && !_connector.fetch_result(result_set))
    {
        WARNING("get result info from databse error, %s", _connector.error());
        ret = false;
    }

    row = NULL;
    row = result_set.fetch_row();
    fprintf(datafile,"<testcases> %s\n<time_limit> %s\n<case_limit> %s\n<memory_limit> %s\n<special> %s\n<vname> %s\n<vid> %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
//    mysql_free_result(res);
    sprintf(sql,"UPDATE status set result='Judging' WHERE runid=%d", task.get_runid());
    if (ret && !_connector.execute(sql, strlen(sql)))
    {
        WARNING("execute sql from databse error, %s", _connector.error());
        ret = false;
    }
    fclose(datafile);                                             //code export;
    int source=open(filename,O_RDONLY);
    int temp_t;
    while((temp_t=read(source,buffer,sizeof(buffer)))>0)
        write(tfd,buffer,temp_t);
    close(source);                                                //send code to client.
    sprintf(filename,"results/%dres.bott", task.get_runid());
    NOTICE("filename: %s", filename);
    FILE *target_file=fopen(filename,"w");
    bool got_things=false;
    memset(buffer,0,sizeof(buffer));
//    puts("B");
    while (!got_things)
    {
        while ((temp_t=recv(tfd,buffer,MAX_DATA_SIZE,MSG_DONTWAIT))>0)
        {
            got_things=true;
            fputs(buffer,target_file);
        }
        if (temp_t==0)
        {
            fclose(target_file);
            char templog[200]={0};
            sprintf(templog,"Lost connection with judger %d.",tnum);
            close(tfd);
            set_status(STATUS_UNCONN);
            WARNING(templog);
            sprintf(templog,"Lost connection, Runid %d requeued.",task.get_runid());
            WARNING(templog);
            sprintf(sql,"UPDATE status set result='Judge Error & Requeued' WHERE runid=%d", task.get_runid());
            if (ret && !_connector.execute(sql, strlen(sql)))
            {
                WARNING("execute sql from databse error, %s", _connector.error());
                ret = false;
            }

            pthread_mutex_lock(&task_queue::instance().task_list_lock);
            task_queue::instance().push_back_task(task);
//            WARNING("Result deal failed, Runid %d requeued.",task.get_runid());
            pthread_cond_signal(&task_queue::instance().task_cond_lock);
            pthread_mutex_unlock(&task_queue::instance().task_list_lock);

            return false;
        }
        usleep(5000);
        if (buffer[0]!='<')
        	got_things=false;
    }
//    puts("A");
    set_status(STATUS_BUSY);
    fclose(target_file);
    int temp_runid;
//    char temp1[50],temp2[50],temp3[50];
//    sscanf(buffer,"%s%s%s%d",temp1,temp2,temp3,&temp_runid);
//    sprintf(filename,"results/%dres.bott",temp_runid);
//    target_file=fopen(filename,"a+");
//    fputs(buffer,target_file);
//    printf("AA: %s", buffer);
//    /*
//     *  <type> 3
//		<runid> 749
//		<memory_used> 1692
//		<time_used> 0
//		<result> Accepted
//		__COMPILE-INFO-BEGIN-LABLE__
//
//		__COMPILE-INFO-END-LABLE__
//		Fetched RunID 749
//     *
//     */
//    while (recv(tfd,buffer,MAX_DATA_SIZE,MSG_DONTWAIT)>0)
//        fputs(buffer,target_file);
//    fclose(target_file);                                                    //接收结果文件。
//    printf("BB: %s", buffer);
    sprintf(sql,"SELECT pid,contest_belong,username FROM status WHERE runid=%d", task.get_runid());

    if (ret && !_connector.execute(sql, strlen(sql)))
     {
         WARNING("execute sql from databse error, %s", _connector.error());
         ret = false;
     }

     if (ret && !_connector.fetch_result(result_set))
     {
         WARNING("get result info from databse error, %s", _connector.error());
         ret = false;
     }

    row = NULL;
    row = result_set.fetch_row();
    temp_pid=atoi(row[0]);
    int temp_cid=atoi(row[1]);
    char temp_username[256]={0};
    strcpy(temp_username,row[2]);
    if(!result_dealer(filename,temp_pid, task.get_runid(),temp_cid,temp_username))
    {
    	pthread_mutex_lock(&task_queue::instance().task_list_lock);
        task_queue::instance().push_back_task(task);
        WARNING("Result deal failed, Runid %d requeued.",task.get_runid());
        pthread_cond_signal(&task_queue::instance().task_cond_lock);
        pthread_mutex_unlock(&task_queue::instance().task_list_lock);
    }
    sprintf(templog,"Task %d Run Finished", task.get_runid());
    NOTICE(templog);
    set_status(STATUS_IDLE);
    return true;
}

bool work_thread::result_dealer(char filename[200],int temp_pid,int temp_runid,int temp_cid,char temp_username[])
{
	bool ret = true;
    FILE *target_file=fopen(filename,"r");
    if (NULL == target_file)
    {
    	WARNING("Open file %s failed.", filename);
    	return false;
    }

    int type,ri,mu,tu;
    char resu[100]={0};
    char ts[10][50];
    if (0 == fscanf(target_file,"%s %d\n%s %d\n%s %d\n%s %d\n%s ",ts[0],&type,ts[1],&ri,ts[2],&mu,ts[3],&tu,ts[4]))
    {
    	return false;
    }
    fgets(resu,100,target_file);
    char sql[MAX_DATA_SIZE]={0};

    sprintf(sql,"SELECT username,contest_belong,pid FROM status WHERE runid=%d",ri);
    mysql_result result_set;
    if (ret && !_connector.execute(sql, strlen(sql)))
    {
        WARNING("execute sql from databse error, %s", _connector.error());
        ret = false;
    }

    if (ret && !_connector.fetch_result(result_set))
    {
        WARNING("get result info from databse error, %s", _connector.error());
        ret = false;
    }
    temp_runid=ri;
    char ** row = NULL;
    row = result_set.fetch_row();
    strcpy(temp_username,row[0]);
    temp_cid=atoi(row[1]);
    temp_pid=atoi(row[2]);
//    mysql_free_result(res);
    resu[strlen(resu)-1]=0;
    sprintf(sql,"UPDATE status set result='%s', memory_used=%d, time_used=%d WHERE runid=%d",resu,mu,tu,ri);
    if (ret && !_connector.execute(sql, strlen(sql)))
    {
        WARNING("execute sql from databse error, %s", _connector.error());
        ret = false;
    }

    if (strcmp(resu,"Accepted")==0)
    {
        sprintf(sql,"SELECT count(*) FROM status WHERE username='%s' AND pid='%d' AND result='Accepted'",temp_username,temp_pid);
//        mysql_sql(mysql,sql);
        if (ret && !_connector.execute(sql, strlen(sql)))
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
        row = result_set.fetch_row();
//        res=mysql_use_result(mysql);
//        row=mysql_fetch_row(res);
        if (atoi(row[0])==1)
        {
//            mysql_free_result(res);
            sprintf(sql,"UPDATE user set total_ac=total_ac+1 WHERE username='%s'",temp_username);
            if (ret && !_connector.execute(sql, strlen(sql)))
            {
                WARNING("execute sql from databse error, %s", _connector.error());
                ret = false;
            }

//            mysql_sql(mysql,sql);
//            res=mysql_use_result(mysql);
        }
//        mysql_free_result(res);
        sprintf(sql,"UPDATE problem set total_ac=total_ac+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Wrong Answer")==0) {
        sprintf(sql,"UPDATE problem set total_wa=total_wa+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Runtime Error")==0) {
        sprintf(sql,"UPDATE problem set total_re=total_re+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Presentation Error")==0) {
        sprintf(sql,"UPDATE problem set total_pe=total_pe+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Time Limit Exceed")==0) {
        sprintf(sql,"UPDATE problem set total_tle=total_tle+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Memory Limit Exceed")==0) {
        sprintf(sql,"UPDATE problem set total_mle=total_mle+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Output Limit Exceed")==0) {
        sprintf(sql,"UPDATE problem set total_ole=total_ole+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Restricted Function")==0) {
        sprintf(sql,"UPDATE problem set total_rf=total_rf+1 WHERE pid=%d",temp_pid);
    }
    else if (strcmp(resu,"Compile Error")==0) {
        sprintf(sql,"UPDATE problem set total_ce=total_ce+1 WHERE pid=%d",temp_pid);
    }
    if (ret && !_connector.execute(sql, strlen(sql)))
    {
        WARNING("execute sql from databse error, %s", _connector.error());
        ret = false;
    }

    char tempce[50000]={0};
    char ce_info_data[MAX_DATA_SIZE]={0};

    while (strcmp(tempce,"__COMPILE-INFO-BEGIN-LABLE__")!=0&&strcmp(tempce,"__COMPILE-INFO-BEGIN-LABLE__\n")!=0&&strcmp(tempce,"__COMPILE-INFO-BEGIN-LABLE__\r\n")!=0) fgets(tempce,50000,target_file);
    while (1) {
        fgets(tempce,50000,target_file);
        if (strcmp(tempce,"__COMPILE-INFO-END-LABLE__")==0||strcmp(tempce,"__COMPILE-INFO-END-LABLE__\n")==0||strcmp(tempce,"__COMPILE-INFO-END-LABLE__\r\n")==0) break;
        strcat(ce_info_data,tempce);
    }
    std::string str1;
    str1=ce_info_data;
    int lastpos=0;

    while (str1.find("\"",lastpos,1)!=std::string::npos) {
        lastpos=str1.find("\"",lastpos,1);
        str1.replace(lastpos,1,"\\\"");
        lastpos+=2;
    }
    strcpy(ce_info_data,str1.c_str());
    sprintf(sql,"UPDATE status set ce_info=\"%s\" WHERE runid=%d",ce_info_data,ri);
//    mysql_sql(mysql,ceupdate);
    if (ret && !_connector.execute(sql, strlen(sql)))
    {
        WARNING("execute sql from databse error, %s", _connector.error());
        ret = false;
    }

    char templog[200]={0};
    sprintf(templog,"Received a result, user: %s, runid: %d result:%s",temp_username,temp_runid,resu);
    NOTICE(templog);
    fclose(target_file);

    return ret;
//    mysql_close(mysql);
//    free(mysql);
}
