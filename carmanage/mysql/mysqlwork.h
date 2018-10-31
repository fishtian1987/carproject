/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mysqlwork.h
 * Author: root
 *
 * Created on 2018年4月30日, 下午11:39
 */

#ifndef MYSQLWORK_H
#define MYSQLWORK_H

#include "mysql_connection_pool.h"

class mysqlwork {
public:
    mysqlwork();
    virtual ~mysqlwork();
 public:
    static mysqlwork* GetInstance();
    static void DestroyInstance();
    bool Init();
    bool SetBoxPass(int boxid, int pass);
    
private:
    static mysqlwork* m_pInstance;
    MysqlConnectionPool *pool;
};

#endif /* MYSQLWORK_H */

