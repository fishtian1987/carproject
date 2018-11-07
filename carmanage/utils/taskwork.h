/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   taskwork.h
 * Author: root
 *
 * Created on 2018年11月3日, 上午10:31
 */

#ifndef TASKWORK_H
#define TASKWORK_H

#include <string>
#include <map>
#include <queue>

namespace fas {

namespace utils {

class taskinfo {
public:
    taskinfo();
    taskinfo(const taskinfo& orig);
    virtual ~taskinfo();
    
    void settaskid(const char *taskld) {taskid_ = atoi(taskld);}
    void setmobile(const char *mobile) {mobile_ = mobile;}
    void setdisid(const char *disid) {disid_ = atoi(disid);}
    void setdisname(const char *disname) {disname_ =  disname;}
    void setdisid(int ndisid) {disid_ = ndisid;}
    void setpass(int npass);
    void setpass(const char *pass) {pass_ =  pass;}
    void setcarpath(const char *carpath) {carpath_ = carpath;}
    
    std::string getdisname() const;
    std::string getpass() const;
    std::string getcarpath() const;
    std::string getmobile() const;
    
private:
    int taskid_;
    std::string mobile_;
    int disid_;
    std::string disname_;
    std::string pass_;
    std::string carpath_;
};

class cartask
{
public:
    cartask();
    cartask(const cartask& orig);
    virtual ~cartask();
    bool Addtaskinfo(int boxid, taskinfo *oneinfo);
    bool PushTaskQueue(int boxid, const char *szpointline);
    int PopTaskQueue(std::string &strpointline, char *mobile, char *pass);
    
private:
    std::map<int, taskinfo*> taskinfomap_;
    std::queue<int> taskqueue_;
};

class taskmap:public std::map<int, cartask*>
{
public:
    taskmap();
    virtual ~taskmap();
    bool AddTask(int carid, cartask* onetask);
    cartask * CheckTask(int carid);
    bool DeleteTask(int carid);
    
private:
    bool getallow(long &hashallow);
    bool releaseallow(long &hashallow);

    long g_addallow; 
};

typedef taskmap::iterator TaskMapIterator;

}

}

#endif /* TASKWORK_H */

