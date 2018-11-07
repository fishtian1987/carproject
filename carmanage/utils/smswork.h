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
#include <Thread.h>
#include <queue>

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

class smspara
{
public:
    smspara();
    smspara(std::string mobile, std::string text);
    smspara(std::string mobile, int code);
    ~smspara();
    
    std::string getmobile() const;
    std::string gettext() const;
    int getcode() const;
    int gettype() const;
private:
    std::string mobile_;
    std::string text_;
    int code_;
    int type_; //1 sms 2 voice
};

class smswork {
public:
    smswork();
    virtual ~smswork();
 public:
    static smswork* GetInstance();
    static void DestroyInstance();
    void PushSmsQueue(std::string mobile, std::string text);
    void sendone();
private:
    static smswork* m_pInstance;
    std::queue<smspara> smsparas_;
};

void *SmsWorkThread(void *p);

}

}


#endif /* SMSWORK_H */

