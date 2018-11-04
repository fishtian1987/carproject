/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpCommand.h
 * Author: root
 *
 * Created on 2018年10月30日, 下午1:39
 */

#ifndef HTTPCOMMAND_H
#define HTTPCOMMAND_H

namespace fas {

    namespace http {

        class HttpCommand {
            public:
                HttpCommand();
                HttpCommand(const HttpCommand& orig);
                virtual ~HttpCommand();
                bool analyseHttpCommand(std::string strCmd);
                
                std::string getCommand() const;
                std::string getCarid() const;
                std::string getBox() const;
                std::string getMobile() const;
            private:
                std::string command_;
                std::string carid_;
                std::string boxid_;
                std::string mobile_;
        };

    }
    
}

#endif /* HTTPCOMMAND_H */

