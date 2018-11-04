/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   taskwork.cpp
 * Author: root
 * 
 * Created on 2018年11月3日, 上午10:31
 */

#include "taskwork.h"
#include <unistd.h>

fas::utils::taskinfo::taskinfo() {
}

fas::utils::taskinfo::taskinfo(const taskinfo& orig) {
}

fas::utils::taskinfo::~taskinfo() {
}

std::string fas::utils::taskinfo::getdisname() const{
    return disname_;
}

/////////////////////////////////////////////////////////////////////////////////////

fas::utils::cartask::cartask() {
}

fas::utils::cartask::cartask(const cartask& orig) {
}

fas::utils::cartask::~cartask() {
}

/////////////////////////////////////////////////////////////////////////////////////

fas::utils::taskmap::taskmap() {
    g_addallow = 0;
}

fas::utils::taskmap::~taskmap() {
}

bool fas::utils::taskmap::getallow(long &hashallow)
{
	int wait = 0xFFFF;
	int h = wait;
	do
	{
		if (0 == __sync_lock_test_and_set(&hashallow, 0xcc))
		{
			return true;
		}
		sleep(20);
		wait -= 20;
		if (h-wait > 5000)
		{
			__sync_lock_test_and_set(&hashallow, 0);
			h = wait;
		}
	} while(wait > 0);
	
	return false;
}

bool fas::utils::taskmap::releaseallow(long &hashallow)
{	
	__sync_lock_test_and_set(&hashallow, 0);
	return true;
}

bool fas::utils::taskmap::AddTask(int carid, cartask* onetask)
{
    TaskMapIterator pIter;
    bool flag = false;
    
    pIter = this->find(carid);
    if (pIter == this->end())
    {
        this->insert(std::make_pair(carid, onetask));
        flag = true;
    }
    
    return flag;
}

fas::utils::cartask * fas::utils::taskmap::CheckTask(int carid)
{

}

