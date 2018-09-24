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

#include "mysqlwork.h"


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



