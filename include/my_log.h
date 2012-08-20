/*
 * my_log.h
 *
 *  Created on: Aug 2, 2012
 *      Author: freshines
 */

#ifndef MY_LOG_H_
#define MY_LOG_H_

#include "common.h"

#define LOG_NONE     0
#define LOG_FATAL    0x01    /*   fatal errors */
#define LOG_WARNING  0x02    /*   exceptional events */
#define LOG_NOTICE   0x04    /*   informational notices */
#define LOG_TRACE    0x08    /*   program tracing */
#define LOG_DEBUG    0x10    /*   full debugging */
#define LOG_ALL      0xff    /*   everything     */

#define PATH_SIZE 100

#define DEBUG(format, args...) \
	my_log::instance().writelog(LOG_DEBUG, format, ##args)

#define NOTICE(format, args...) \
	my_log::instance().writelog(LOG_NOTICE, format, ##args)

#define WARNING(format, args...) \
	my_log::instance().writelog(LOG_NOTICE, format, ##args)

#define FATAL(format, args...) \
	my_log::instance().writelog(LOG_FATAL, "[%s:%d] "format, __FILE__, __LINE__, ##args)


class my_log
{
public:

	static my_log& instance();

	int get_buffer_size();

	void set_logname(const string logname);

	void set_logpath(const string logpath);

	bool open_log();

	bool log_flush();

	void writelog(int level, const char *format, ...);

	int LOG_FLUSH_TIME;

	pthread_mutex_t log_list_lock;
private:

	int LOG_LEVEL;

	list<string> log_list;

	string log_path;

	string log_name;

	my_log();

	virtual ~my_log()
	{
	}
};



#endif /* MY_LOG_H_ */
