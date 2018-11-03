/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   smswork.h
 * Author: root
 *
 * Created on 2018年11月2日, 下午8:27
 */

#ifndef SMSWORK_H
#define SMSWORK_H

#include <curl/curl.h>

namespace fas {

namespace utils {

void send_data(const char *url,char *data, CURL *curl);    
/**
* 查账户信息
*/
void get_user();
/**
* 使用智能匹配模板接口发短信
*/
void send_sms(char *mobile, char *text);
/**
* 指定模板单发
*/
void send_tpl_sms(char *mobile, int tpl_id, char *tpl_value);
/**
* 发送语音验证码
*/
void send_voice(char *mobile, int code);

}

}


#endif /* SMSWORK_H */

