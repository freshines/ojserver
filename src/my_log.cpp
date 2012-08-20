/*
 * my_log.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: freshines
 */

#include "my_log.h"


my_log::my_log()
{
	log_list.clear();
}

my_log& my_log::instance()
{
	static my_log l;
	return l;
}

int my_log::get_buffer_size()
{
	return log_list.size();
}

bool my_log::open_log()
{
	time_t CurTime;
	struct tm TmTime;
	time(&CurTime);
	localtime_r(&CurTime, &TmTime);
	config& conf = config::instance();
	char logpath[PATH_SIZE] = "";
	char logname[PATH_SIZE] = "";

	if(!conf.get_string("LOG_PATH", logpath, sizeof(logpath)))
	{
		printf("%4d-%02d-%02d %02d:%02d:%02d [ERROR] Config::get_string(LOG_PATH) failed!\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
		return false;
	}

	if(!conf.get_string("LOG_NAME", logname, sizeof(logpath)))
	{
		printf("%4d-%02d-%02d %02d:%02d:%02d [ERROR] Config::get_string(LOG_NAME) failed!\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
		return false;
	}

	set_logname(logname);
	set_logpath(logpath);

	if(!conf.get_int("LOG_LEVEL", LOG_LEVEL))
	{
		printf("%4d-%02d-%02d %02d:%02d:%02d [ERROR] Config::get_int(LOG_LEVEL) failed!\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
		LOG_LEVEL = 4;
	}

	if(!conf.get_int("LOG_FLUSH_TIME", LOG_FLUSH_TIME))
	{
		printf("%4d-%02d-%02d %02d:%02d:%02d [ERROR] Config::get_int(LOG_LEVEL) failed!\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
		LOG_FLUSH_TIME = 5;
	}

	strcat(logpath, "/");
	strcat(logpath, logname);
	FILE* fin = fopen(logpath, "a+");
	if(NULL == fin)
	{
		printf("%4d-%02d-%02d %02d:%02d:%02d [ERROR] OPEN logfile %s ERROR.\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec, logpath);
		return false;
	}
	else
	{
		fclose(fin);
	}
	NOTICE("OPEN logfile %s sucess.", logpath);
	return true;
}


bool my_log::log_flush()
{
	bool ret = false;
	FILE *fin;
	string path = log_path + "/" + log_name;
	string temp_log;
	list<string>::iterator it;
	fin = fopen(path.c_str(), "a+");
	if(NULL == fin)
	{
		printf("OPEN logfile %s FAILED.\n", path.c_str());
		return false;
	}
	while(!log_list.empty())
	{
		temp_log = log_list.front();
		fprintf(fin, "%s", temp_log.c_str());
		log_list.pop_front();
	}
//	NOTICE("%s", "Flush log buffer sucess.");
	fclose(fin);
	return ret;
}


void my_log::writelog(int level, const char *format, ...)
{
	if(level > LOG_LEVEL || level <= LOG_NONE)
	{
		return ;
	}

	char tmp[200];
	time_t CurTime;
	struct tm TmTime;
	time(&CurTime);

	if(CurTime < 0)
	{
		printf("CurTime ERROR.\n");
		return ;
	}

	localtime_r(&CurTime, &TmTime);

	va_list args;
	va_start(args, format);
	vsprintf(tmp,format,args);
	va_end(args);
	char temp_log[200], temp[200];

	sprintf(temp_log, "%4d-%02d-%02d %02d:%02d:%02d ", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
	switch(level)
	{
	case LOG_WARNING:
		sprintf(temp, "[WARNING]%s\n", tmp);
		strcat(temp_log, temp);
		break;
	case LOG_DEBUG:
		sprintf(temp, "[DEBUG] %s\n", tmp);
		strcat(temp_log, temp);
		break;
	case LOG_FATAL:
		sprintf(temp, "[FATAL]%s\n", tmp);
		strcat(temp_log, temp);
		break;
	case LOG_NOTICE:
		sprintf(temp, "[NOTICE] %s\n", tmp);
		strcat(temp_log, temp);
		break;
	default:
		sprintf(temp, "[ERROR]LOG LEVEL UNKNOWN.\n");
		strcat(temp_log, temp);
	}
	pthread_mutex_lock(&log_list_lock);
	log_list.push_back(temp_log);
	pthread_mutex_unlock(&log_list_lock);
}

void my_log::set_logname(const string logname)
{
	log_name = logname;
}

void my_log::set_logpath(const string logpath)
{
	log_path = logpath;
}
/*

int main()
{
//	FATAL(1,"config::get_string(LOG_NAME) failed!");
	if(!config::instance().load_conf("../conf", "ojserver.conf"))
		exit(1);
	if(!my_log::instance().open_log())
		exit(1);
	char str[100]="abcd";
	FATAL(str);
	WARNING(str);
	NOTICE(str);
	DEBUG(str);
	return 0;
}
*/

