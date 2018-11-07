/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   smswork.cpp
 * Author: root
 * 
 * Created on 2018年11月2日, 下午8:27
 */

#include "smswork.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#define MAXPARAM 2048

/**
bingone

本样例依赖libcurl库
下载地址 https://curl.haxx.se/download.html
*/

// 发送语音验证码内容
int code = 1234;

void fas::utils::send_data(const char *url,char *data, CURL *curl)
{ 
    // specify the url
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // specify the POST data
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    // get response data
    curl_easy_perform(curl);
}
/**
* 查账户信息
*/
void fas::utils::get_user()
{
    CURL *curl;
    curl = curl_easy_init();
    if(NULL == curl) {
        perror("curl open fail\n");
    }   
    
    char url[100], apikey[50];
    char params[MAXPARAM + 1];
    char *cp = params;
    strcpy(apikey, "3f6d544b0fe24735c7061efc4db84b0b");
    sprintf(params,"apikey=%s", apikey);
    
    strcpy(url, "https://sms.yunpian.com/v2/user/get.json");    
    send_data(url, params, curl);

    curl_easy_cleanup(curl);    
}

/**
* 使用智能匹配模板接口发短信
*/
void fas::utils::send_sms(char *mobile, char *text)
{
    CURL *curl;    
    curl = curl_easy_init();
    mobile = curl_easy_escape(curl,mobile,strlen(mobile));
    if(NULL == curl) {
        perror("curl open fail\n");
    }    
    
    char url[100], apikey[50];
    char params[MAXPARAM + 1];
    char *cp = params;
    strcpy(apikey, "3f6d544b0fe24735c7061efc4db84b0b");
    sprintf(params,"apikey=%s&mobile=%s&text=%s", apikey, mobile, text);
    
    strcpy(url, "https://sms.yunpian.com/v2/sms/single_send.json");
    send_data(url, params, curl);
  
    curl_easy_cleanup(curl);       
}
/**
* 指定模板单发
*/
void fas::utils::send_tpl_sms(char *mobile, int tpl_id, char *tpl_value)
{
    CURL *curl;    
    curl = curl_easy_init();
    mobile = curl_easy_escape(curl,mobile,strlen(mobile));
    if(NULL == curl) {
        perror("curl open fail\n");
    }
    
    char url[100], apikey[50];
    char params[MAXPARAM + 1];
    char *cp = params;
    strcpy(apikey, "3f6d544b0fe24735c7061efc4db84b0b");
    sprintf(params, "apikey=%s&mobile=%s&tpl_id=%d&tpl_value=%s", apikey, mobile,
        tpl_id, tpl_value);
    
    strcpy(url, "https://sms.yunpian.com/v2/sms/tpl_single_send.json");
    send_data(url, params, curl);
    
    curl_easy_cleanup(curl);      
}
/**
* 发送语音验证码
*/
void fas::utils::send_voice(char *mobile, int code)
{
    CURL *curl;    
    curl = curl_easy_init();
    mobile = curl_easy_escape(curl,mobile,strlen(mobile));
    if(NULL == curl) {
        perror("curl open fail\n");
    }    
    
    char url[100], apikey[50];
    char params[MAXPARAM + 1];
    char *cp = params;
    strcpy(apikey, "3f6d544b0fe24735c7061efc4db84b0b");
    sprintf(params,"apikey=%s&mobile=%s&code=%d", apikey, mobile, code);
    
    strcpy(url, "https://voice.yunpian.com/v2/voice/send.json");
    send_data(url, params, curl);
    
    curl_easy_cleanup(curl);       
}

fas::utils::smspara::smspara()
{}

fas::utils::smspara::smspara(std::string mobile, std::string text) :
    mobile_(mobile),
    text_(text),
    type_(1) {
}
fas::utils::smspara::smspara(std::string mobile, int code) :
    mobile_(mobile),
    code_(code),
    type_(2) {
}

fas::utils::smspara::~smspara()
{}

std::string fas::utils::smspara::getmobile() const {
    return mobile_;
}
std::string fas::utils::smspara::gettext() const {
    return text_;
}
int fas::utils::smspara::getcode() const {
    return code_;
}
int fas::utils::smspara::gettype() const {
    return type_;
}

fas::utils::smswork::smswork() {
}

fas::utils::smswork::~smswork() {

}

void fas::utils::smswork::DestroyInstance()
{
    if ( m_pInstance != 0 )
    {
        delete m_pInstance;
        m_pInstance = 0;
    }
}

fas::utils::smswork *fas::utils::smswork::m_pInstance = 0;

fas::utils::smswork *fas::utils::smswork::GetInstance()
{
    if ( m_pInstance == 0 )
    {
        m_pInstance = new fas::utils::smswork();
    }

    return m_pInstance;
}

void fas::utils::smswork::PushSmsQueue(std::string mobile, std::string text)
{
    smsparas_.push(smspara(mobile, text));  
}

void fas::utils::smswork::sendone()
{
    if(smsparas_.size() > 0)
    {
        int type = smsparas_.front().gettype();
        if(type == 1)
        {
            std::string mobile = smsparas_.front().getmobile();
            std::string text = smsparas_.front().gettext();
            
            send_sms((char *)mobile.c_str(), (char *)text.c_str());
        }
        else if(type == 2)
        {
            std::string mobile = smsparas_.front().getmobile();
            int code = smsparas_.front().getcode();
            
            send_voice((char *)mobile.c_str(), code);
        }
        
        smsparas_.pop();
    }
}

void *fas::utils::SmsWorkThread(void *p)
{
    while(1)
    {
        fas::utils::smswork::GetInstance()->sendone();
        sleep(10);
    }
    
    return 0;
}