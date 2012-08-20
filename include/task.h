/*
 * task.h
 *
 *  Created on: Aug 5, 2012
 *      Author: freshines
 */

#ifndef TASK_H_
#define TASK_H_

#include "common.h"

class task_t
{
public:

	task_t(){};

	task_t(task_t const &that);

	int get_runid() const;

	int get_type() const;

	int get_cha_id() const;

	string get_vname() const;

	bool set_runid(const int id);

	bool set_type(const int type);

	bool set_cha_id(const int cha_id);

	bool set_vname(const string vname);

private:

	int runid;

	string vname;

	int type;

	int cha_id;

};
#endif /* TASK_H_ */
