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
#include <curl/curl.h>
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

void send_data(const char *url,char *data, CURL *curl)
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
void get_user()
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
void send_sms(char *mobile, char *text)
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
void send_tpl_sms(char *mobile, int tpl_id, char *tpl_value)
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
void send_voice(char *mobile, int code)
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
