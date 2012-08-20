/*
 * mysql_connector.h
 *
 *  Created on: Aug 9, 2012
 *      Author: freshines
 */

#ifndef MYSQL_CONNECTOR_H_
#define MYSQL_CONNECTOR_H_

#include "common.h"
#define MAX_CONFIG_ITEM_LENGTH 50
#define SQL_LENGTH 200
class mysql_result
{
public:
    ///< ctor
    mysql_result()
    {
        _res = NULL;
    }

    ///< dctor
    ~mysql_result()
    {
        if (_res != NULL)
        {
            mysql_free_result(_res);
            _res = NULL;
        }
    }

    ///< ctor
    mysql_result(MYSQL_RES * res)
    {
        _res = res;
    }

    void set_result(MYSQL_RES * res)
    {
        if (_res != NULL)
        {
            mysql_free_result(_res);
        }
        _res = res;
    }

    char ** fetch_row() const
    {
        return mysql_fetch_row(_res);
    }

    unsigned int get_field_num() const
    {
        return mysql_num_fields(_res);
    }

    unsigned long long get_row_num() const
    {
        return mysql_num_rows(_res);
    }

    const unsigned long* fetch_lengths(MYSQL_RES* res) const
    {
        return mysql_fetch_lengths(res);
    }
private:
    MYSQL_RES* _res;

    mysql_result& operator=(const mysql_result& that)
    {
        _res = that._res;
        return *this;
    }
};


class mysql_connector
{
public:

    mysql_connector();

    virtual ~mysql_connector();

    unsigned long long affected_rows()
    {
        return mysql_affected_rows(&_mysql);
    }


    bool connect();


    bool connect(const char* host, unsigned int port,
        const char* db, const char* user, const char* password);


	void disconnect();


    bool is_connected() const
    {
        return _is_connected;
    }


    bool execute(const char* qstr, size_t length)
    {
		bool ret = true;
		ret = connect();
        return (ret && !mysql_real_query(&_mysql, qstr, length));
    }


    bool execute_long(const char* qstr, size_t length)
    {
        return (!mysql_real_query(&_mysql, qstr, length));
    }


    bool fetch_result(mysql_result & result)
    {
        MYSQL_RES * res = mysql_store_result(&_mysql);
        result.set_result(res);
        return (res != NULL);
    }

    char const * client_version() const
    {
        return mysql_get_client_info();
    }

	bool status()
	{
		return !mysql_errno(&_mysql);
	}

	char const * error()
	{
		return mysql_error(&_mysql);
	}

    unsigned long real_escape_string(char const * from, char * to, unsigned long length)
    {
        return mysql_real_escape_string(&_mysql, to, from, length);
    }

private:

    mysql_connector& operator=(const mysql_connector& that);

    MYSQL _mysql;

    bool _is_connected;
};


#endif /* MYSQL_CONNECTOR_H_ */
