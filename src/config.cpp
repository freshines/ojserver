/*
 * config.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: freshines
 */

#include "config.h"

config::config()
{
	Map.clear();
}

config::~config()
{
}

config& config::instance()
{
	static config c;
	return c;
}

bool config::load_conf(const char *path, const char *name)
{
	time_t CurTime;
	struct tm TmTime;
	time(&CurTime);
	localtime_r(&CurTime, &TmTime);
	bool ret = true;
	char str[200], v1[100], v2[100];
	int p=200;
	if (NULL == path || NULL == name)
	{
		printf("%4d-%02d-%02d %02d:%02d:%02d [ERROR] Config file path ERROR.\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
		ret = false;
	}
	if (ret)
	{
		strcpy(str, path);
		strcat(str, "/");
		strcat(str, name);
		FILE *fin = fopen(str, "r");
		if(NULL == fin)
		{
			printf("%4d-%02d-%02d %02d:%02d:%02d [ERROR] Open config file %s ERROR.\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec, str);
			ret = false;
		}
		else
		{
			while(fgets(str, p, fin) != NULL)
			{
				if(0 == strlen(str) || str[0] == '#')
					continue;
				else
				{
					sscanf(str, "%s : %s", v1, v2);
//					if(Map.find(v1) != Map.end())
					Map[v1] = v2;
				}
			}
		}
	}
	printf("%4d-%02d-%02d %02d:%02d:%02d [NOTICE] Load config file success.\n", TmTime.tm_year+1900, TmTime.tm_mon+1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
	return ret;
}

bool config::get_int(const char *name, int &value)
{
	bool ret = true;
	if(NULL == name || strlen(name)<1)
	{
		ret = false;
	}
	else if(Map.find(name) != Map.end())
	{
		value = atoi(Map[name].c_str());
	}
	else
		ret = false;
	return ret;
}

bool config::get_float(const char *name, float &value)
{
	bool ret = true;
	if(NULL == name || strlen(name)<1)
	{
		ret = false;
	}
	else if(Map.find(name) != Map.end())
	{
		value = atof(Map[name].c_str());
	}
	else
		ret = false;
	return ret;
}

bool config::get_string(const char *name, char *value, int size)
{
    bool ret = true;
    char str[50];
    strcpy(str, name);
    if (NULL == name || NULL == value)
    {
        ret = false;
    }
    else if(Map.find(name) != Map.end())
    {
        strncpy(value, Map[name].c_str(), size);
    }
    else
    	ret = false;
    value[size - 1] = '\0';
    return ret;
}




