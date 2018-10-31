/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: root
 *
 * Created on 2018年4月27日, 下午1:50
 */

#include <HttpServer.h>
#include <EventLoop.h>
#include <NetAddress.h>
#include "mysqlwork.h"

using namespace fas;
using namespace fas::http;

int main() 
{
    mysqlwork::GetInstance()->Init(); 
    
    EventLoop *loop = new EventLoop;

    HttpServer *ser = new HttpServer(loop, NetAddress(AF_INET, 9999, "192.169.133.46"));
    //HttpServer *ser = new HttpServer(loop, NetAddress(AF_INET,  9999,  "192.168.0.76"));
    //HttpServer *ser = new HttpServer(loop, NetAddress(AF_INET,  9999,  "172.27.0.10"));

    ser->start();

    loop->loop();
}

