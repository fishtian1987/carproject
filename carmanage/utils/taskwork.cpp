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
#include <string.h>

fas::utils::taskinfo::taskinfo() {
}

fas::utils::taskinfo::taskinfo(const taskinfo& orig) {
}

fas::utils::taskinfo::~taskinfo() {
}

std::string fas::utils::taskinfo::getdisname() const{
    return disname_;
}

void fas::utils::taskinfo::setpass(int npass)
{
    char tmpstr[20];
    sprintf(tmpstr, "%d", npass);
    pass_ = tmpstr;
}

std::string fas::utils::taskinfo::getpass() const{
    return pass_;
}

std::string fas::utils::taskinfo::getcarpath() const{
    return carpath_;
}

std::string fas::utils::taskinfo::getmobile() const{
    return mobile_;
}

/////////////////////////////////////////////////////////////////////////////////////

fas::utils::cartask::cartask() {
}

fas::utils::cartask::cartask(const cartask& orig) {
}

fas::utils::cartask::~cartask() {
}

bool fas::utils::cartask::Addtaskinfo(int boxid, taskinfo *oneinfo)
{
    std::map<int, taskinfo*>::iterator infoiter;
    infoiter = taskinfomap_.find(boxid);
    if(infoiter == taskinfomap_.end())
    {
        taskinfomap_.insert(std::make_pair(boxid, oneinfo));
    }  
}

bool fas::utils::cartask::PushTaskQueue(int boxid, const char *szpointline)
{
    std::map<int, taskinfo*>::iterator infoiter;
    taskinfo *oneinfo = NULL;
    infoiter = taskinfomap_.find(boxid);
    if(infoiter != taskinfomap_.end())
    {
        oneinfo = infoiter->second;
        oneinfo->setcarpath(szpointline);
        taskqueue_.push(boxid);        
    }
    else
    {
        oneinfo = new taskinfo();
        oneinfo->settaskid("");
        oneinfo->setmobile("");
        oneinfo->settaskid("");
        oneinfo->setdisname("");
        oneinfo->setpass("");
        
    }

    return true;
}

bool fas::utils::cartask::PopTaskQueue(std::string &strpointline, char *mobile, char *pass)
{
    int pos;
    bool ret = false;
    if(taskqueue_.size() > 0)
    {
        pos = taskqueue_.front();
        taskqueue_.pop();
        
        std::map<int, taskinfo*>::iterator infoiter;
        taskinfo *oneinfo = NULL;
        infoiter = taskinfomap_.find(pos);
        if(infoiter != taskinfomap_.end())
        {
            oneinfo = infoiter->second;
            strpointline = oneinfo->getcarpath();
            strcpy(mobile, oneinfo->getmobile().c_str());
            strcpy(pass, oneinfo->getpass().c_str());
            delete oneinfo;
            taskinfomap_.erase(pos);            
            
            ret = true;
        }
    }
    return ret;
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
    TaskMapIterator pIter;
    cartask * onetask = NULL;
    
    pIter = this->find(carid);
    if (pIter != this->end())
    {
        onetask = pIter->second;
    }
    
    return onetask;
}

bool fas::utils::taskmap::DeleteTask(int carid)
{
    TaskMapIterator pIter;
    bool flag = false;
    cartask * onetask = NULL;

    pIter = this->find(carid);
    if (pIter != this->end())
    {
        onetask = pIter->second;
        delete onetask;
        this->erase(carid);
        flag = true;
    }
    
    return flag;
}
