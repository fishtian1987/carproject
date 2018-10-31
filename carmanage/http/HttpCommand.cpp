/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpCommand.cpp
 * Author: root
 * 
 * Created on 2018年10月30日, 下午1:39
 */

#include <vector>
#include <utilstring.h>
#include <Buffer.h>

#include "HttpCommand.h"

fas::http::HttpCommand::HttpCommand() :
    command_(""), 
    carid_(""), 
    boxid_("") {
}

fas::http::HttpCommand::HttpCommand(const HttpCommand& orig) {
}

fas::http::HttpCommand::~HttpCommand() {
}

bool fas::http::HttpCommand::analyseHttpCommand(std::string strCmd)
{
    if(strCmd.length() == 0)
        return false;
    
      std::vector<string> lines,fields;
      fas::utils::StringSplitBychar(strCmd, '&', lines, 0);
      
      for(int i = 0; i < lines.size(); i++)
      {
          fields.clear();
          fas::utils::StringSplitBychar(lines[i].c_str(), '=', fields, 0);
          if (fields.size() != 2) {
              continue;
          }
          
          if(fields[0].compare("cmd") == 0)
          {
              this->command_ = fields[1];
          }
          else if(fields[0].compare("carid") == 0)
          {
              this->carid_ = fields[1];
          }
          else if(fields[0].compare("boxid") == 0)
          {
              this->boxid_ = fields[1];
          }
      }
}

std::string fas::http::HttpCommand::getCommand() const{
    return this->command_;
}

std::string fas::http::HttpCommand::getCarid() const{
    return this->carid_;
}

std::string fas::http::HttpCommand::getBox() const{
    return this->boxid_;
}
        

