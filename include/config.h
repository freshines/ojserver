/*
 * config.h
 *
 *  Created on: Aug 2, 2012
 *      Author: freshines
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "common.h"


class config
{
public:
	static const int MAX_CONF_ITEM = 1024;
	map <string, string> Map;

public:

	static config& instance();

	bool load_conf(const char *path, const char *name);

	bool get_int(const char *name, int &value) ;

	bool get_string(const char *name, char *value, int size) ;

	bool get_float(const char *name, float &value ) ;

protected:

	config();

	virtual ~config();

private:


};

#endif /* CONFIG_H_ */
