/*
 * task.cpp
 *
 *  Created on: Aug 5, 2012
 *      Author: freshines
 */

#include "task.h"

task_t::task_t(task_t const &that)
{
	runid = that.runid;

	vname = that.vname;

	type = that.type;

	cha_id = that.cha_id;
}

int task_t::get_type() const
{
	return type;
}

int task_t::get_cha_id() const
{
	return cha_id;
}
string task_t::get_vname() const
{
	return vname;
}

int task_t::get_runid() const
{
	return runid;
}

bool task_t::set_runid(const int runid)
{
	this->runid = runid;
	return true;
}

bool task_t::set_type(const int type)
{
	this->type = type;
	return true;
}

bool task_t::set_cha_id(const int cha_id)
{
	this->cha_id = cha_id;
	return true;
}

bool task_t::set_vname(const string vname)
{
	this->vname = vname;
	return true;
}
