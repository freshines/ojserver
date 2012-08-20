/*
 * mysql_connector.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: freshines
 */
#include "mysql_connector.h"

mysql_connector::mysql_connector() : _is_connected(false)
{
    mysql_init(&_mysql);
}


mysql_connector::~mysql_connector(void)
{
    disconnect();
}


bool mysql_connector::connect()
{
    bool ret = true;
    char host[MAX_CONFIG_ITEM_LENGTH];
    int port = 0;
    char user[MAX_CONFIG_ITEM_LENGTH];
    char password[MAX_CONFIG_ITEM_LENGTH];
    char database[MAX_CONFIG_ITEM_LENGTH];

    if (ret && !config::instance().get_string("MYSQL_HOST", host, sizeof(host)))
    {
        FATAL("get config item MYSQL_HOST error");
        ret = false;
    }

    if (ret && !config::instance().get_string("MYSQL_DB", database, sizeof(database)))
    {
        FATAL("get config item MYSQL_DB error");
        ret = false;
    }
    if (ret && !config::instance().get_string("MYSQL_USER", user, sizeof(user)))
    {
        FATAL("get config item MYSQL_USER error");
        ret = false;
    }

    if (ret && !config::instance().get_string("MYSQL_PASSWORD", password, sizeof(password)))
    {
        password[0] = 0;
    }
    if (ret && !config::instance().get_int("MYSQL_PORT", port))
    {
        FATAL("get config item MYSQL_PORT error");
        ret = false;
    }

    if (ret && !connect(host, port, database, user, password))
    {
        FATAL("use config connect mysql error, host:%s, port:%d, database:%s, user:%s, password:%s",
            host, port, database, user, password);
        ret = false;
    }

    return ret;
}


bool mysql_connector::connect(const char *host, unsigned int port,
                              const char *db, const char *user, const char *password)
{
    bool ret = true;

    if (ret && is_connected())
    {
        disconnect();
    }

    if (ret)
    {
        ret = mysql_real_connect(&_mysql, host, user, password, db, port, NULL, 0);
        _is_connected = ret;
    }

	if (ret)
	{
		char sql[SQL_LENGTH] = "set names 'utf8'";
		ret = execute_long(sql, strlen(sql));
		if (!ret)
		{
			WARNING("set connection of mysql to utf8 error, %s", error());
		}
	}

    return ret;
}


void mysql_connector::disconnect()
{
	mysql_close(&_mysql);
	_is_connected = false;
}
