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
    
private:
    int taskid_;
    std::string mobile_;
    int disid_;
    std::string disname_;
};

}

}

#endif /* TASKWORK_H */

