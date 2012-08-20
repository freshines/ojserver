/*
 * main.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: freshines
 */

#include "taskmgr.h"




int main(int argc, char* argv[])
{
	taskmgr mgr;
	pthread_t tid;
    mkdir("raw_files",0777);
    mkdir("results",0777);
    mkdir("cha_raw_files",0777);
    mkdir("cha_results",0777);
    mkdir("../log",0777);

    if (!mgr.init())
    {
    	mgr.writeLog("[ERROR] Server init failed.");
    	return -1;
    }
 //   printf("A");
 //   pthread_create(&tid, NULL, log_task, NULL);
//    pthread_create(&tid, NULL, scann_task, (void *)arg);
//    printf("B");
    if(argc>1)
    	mgr.start(argv[1]);
    else
    	mgr.start(NULL);


}
