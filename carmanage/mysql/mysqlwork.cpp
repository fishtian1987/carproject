/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mysqlwork.cpp
 * Author: root
 * 
 * Created on 2018年4月30日, 下午11:39
 */

#include <vector>

#include "mysqlwork.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

mysqlwork::mysqlwork() {
}

mysqlwork::~mysqlwork() {
    if(pool)
    {
        delete pool; 
    }
}

void mysqlwork::DestroyInstance()
{
    if ( m_pInstance != 0 )
    {
        delete m_pInstance;
        m_pInstance = 0;
    }
}

mysqlwork *mysqlwork::m_pInstance = 0;

mysqlwork *mysqlwork::GetInstance()
{
    if ( m_pInstance == 0 )
    {
        m_pInstance = new mysqlwork();
    }

    return m_pInstance;
}

bool mysqlwork::Init()
{
    pool = new MysqlConnectionPool();
    int ret = 0;
    ret = pool->initMysqlConnPool("localhost", 3306, "car_txy", "dLn226kPmc4RXXpN", "car_txy");
    ret = pool->openConnPool(10);
    
    return true;
}

bool mysqlwork::SetBoxPass(int boxid, int pass)
{
    mysqlConnection *mysqlConn = pool->fetchConnection();
    if(mysqlConn != NULL)
    {
        char sqlstr[100];
        snprintf(sqlstr, 100, "update tp_car_box set pass = %d where id = %d", pass, boxid);
        
        pool->executeSql(mysqlConn, sqlstr);
        pool->recycleConnection(mysqlConn);
    }
    
    return true;
}



bool mysqlwork::SetTaskState(int Taskid)
{
    mysqlConnection *mysqlConn = pool->fetchConnection();
    if(mysqlConn != NULL)
    {
        char sqlstr[100];
        snprintf(sqlstr, 100, "update tp_task set status = '2',endtime = UNIX_TIMESTAMP(now()) where id = %d", Taskid);    
        
        pool->executeSql(mysqlConn, sqlstr);
        pool->recycleConnection(mysqlConn);
    }
    
    return true;
}

int mysqlwork::queryTaskIDbyBoxID(int boxid)
{
    int ret = -1;
    mysqlConnection *mysqlConn = pool->fetchConnection();
    if(mysqlConn != NULL)
    {
        char sqlstr[100];
        snprintf(sqlstr, 100, "SELECT id FROM `tp_task` WHERE carriage_num = '%d' ORDER BY starttime DESC LIMIT 1", boxid);    
        
        pool->executeSql(mysqlConn, sqlstr);
        
        MYSQL_RES *res_ptr = mysql_store_result(mysqlConn->sock);
        if(res_ptr) { 
            MYSQL_ROW sqlrow = mysql_fetch_row(res_ptr);
            if(sqlrow != NULL)
            {
                ret = atoi(sqlrow[0]);
            }
        }
        
        mysql_free_result(res_ptr); 
        pool->recycleConnection(mysqlConn);
    }
    
    return ret;    
}

bool mysqlwork::queryPassbyBoxID(int boxid, char *pass, int len)
{
    mysqlConnection *mysqlConn = pool->fetchConnection();
    if(mysqlConn != NULL)
    {
        char sqlstr[100];
        snprintf(sqlstr, 100, "SELECT pass FROM `tp_car_box` WHERE id = '%d'", boxid);    
        
        pool->executeSql(mysqlConn, sqlstr);
        
        MYSQL_RES *res_ptr = mysql_store_result(mysqlConn->sock);
        if(res_ptr) { 
            MYSQL_ROW sqlrow = mysql_fetch_row(res_ptr);
            if(sqlrow != NULL)
            {
                strcpy(pass, sqlrow[0]);               
            }
        }
        
        mysql_free_result(res_ptr); 
        pool->recycleConnection(mysqlConn);
    }
    
    return true;       
}

bool mysqlwork::queryTaskInfobyBoxID(int boxid, fas::utils::taskinfo *oneinfo)
{
    bool bFind = false;
    mysqlConnection *mysqlConn = pool->fetchConnection();
    if(mysqlConn != NULL)
    {
        char sqlstr[200];
        snprintf(sqlstr, 200, "SELECT id,consignee_mobile,distribution_point_id,disname FROM `tp_task` WHERE carriage_num = '%d' ORDER BY starttime DESC LIMIT 1", boxid);    
        
        pool->executeSql(mysqlConn, sqlstr);
        
        MYSQL_RES *res_ptr = mysql_store_result(mysqlConn->sock);
        if(res_ptr) { 
            int j = mysql_num_fields(res_ptr);          
            if (j == 4) {
                MYSQL_ROW sqlrow = mysql_fetch_row(res_ptr);
                if(sqlrow != NULL)
                {
                    oneinfo->settaskid(sqlrow[0]);
                    oneinfo->setmobile(sqlrow[1]);
                    oneinfo->settaskid(sqlrow[2]);
                    oneinfo->setdisname(sqlrow[3]);
                    bFind = true;
                }
            }
        }
        
        mysql_free_result(res_ptr); 
        pool->recycleConnection(mysqlConn);
    }
    
    return bFind;    
}

int mysqlwork::queryDisIDbyDisName(const char *DisName)
{
    int ret = -1;
    mysqlConnection *mysqlConn = pool->fetchConnection();
    if(mysqlConn != NULL)
    {
        char sqlstr[100];
        snprintf(sqlstr, 100, "SELECT id FROM `tp_distribution_point` WHERE name = '%s' LIMIT 1", DisName);    
        
        pool->executeSql(mysqlConn, sqlstr);
        
        MYSQL_RES *res_ptr = mysql_store_result(mysqlConn->sock);
        if(res_ptr) { 
            MYSQL_ROW sqlrow = mysql_fetch_row(res_ptr);
            if(sqlrow != NULL)
            {
                ret = atoi(sqlrow[0]);
            }
        }
        
        mysql_free_result(res_ptr); 
        pool->recycleConnection(mysqlConn);
    }
    
    return ret;    
}

int mysqlwork::queryPointLinebyAreaid(int areaid, std::vector<std::string> &pointlines)
{
    int ret = 0;
    pointlines.clear();
    mysqlConnection *mysqlConn = pool->fetchConnection();
    if(mysqlConn != NULL)
    {
        char sqlstr[100];
        snprintf(sqlstr, 100, "SELECT point_ids FROM `tp_distribution_point_line`");    
        
        pool->executeSql(mysqlConn, sqlstr);
        
        MYSQL_RES *res_ptr = mysql_store_result(mysqlConn->sock);
        if(res_ptr) { 
            MYSQL_ROW sqlrow;
            while((sqlrow = mysql_fetch_row(res_ptr)))
            {
                pointlines.push_back(sqlrow[0]);
                ret++;
            }
        }
        
        mysql_free_result(res_ptr); 
        pool->recycleConnection(mysqlConn);
    }
    
    return ret;     
}

/*
    int num = 100;
    MYSQL_RES *res_ptr;
    int i, j;
    MYSQL_ROW sqlrow;
    while(num > 0){
        mysqlConnection *mysqlConn = pool->fetchConnection();
        assert(mysqlConn != NULL);
        pool->executeSql(mysqlConn, "select * from tp_car");
        res_ptr = mysql_store_result(mysqlConn->sock);
        if(res_ptr) {               
            printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr));   
            j = mysql_num_fields(res_ptr);          
            while((sqlrow = mysql_fetch_row(res_ptr))){
                for(i = 0; i < j; i++)
                    printf("%s\t", sqlrow[i]);
                printf("\n");          
            }
            if (mysql_errno(mysqlConn->sock)) {
                fprintf(stderr,"Retrive error:%s\n",mysql_error(mysqlConn->sock));               
            }
        }
        mysql_free_result(res_ptr);       
        pool->recycleConnection(mysqlConn);
        num--;
    }
*/




