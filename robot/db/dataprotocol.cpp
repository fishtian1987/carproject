#include "dataprotocol.h"
#include "utils/usartdriver.h"
#include "info/message.h"
#include "db/package.h"
#include "info/atk.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>

DataProtocol::DataProtocol()
{
    package=new Package;
}

DataProtocol::~DataProtocol()
{
    if(package!=NULL)
        delete package;
}


void DataProtocol::sendBaseLocation(QList<RouteInfo> &data)
{
    Message msg;
    long long temp;

    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_ROUTE_BIAS);
    for(int i=0;i<data.size();i++)
    {
        temp=data[i].lat*100000000;
        this->lat=temp;
        msg.data.append(BYTE4(temp));
        msg.data.append(BYTE3(temp));
        msg.data.append(BYTE2(temp));
        msg.data.append(BYTE1(temp));
        msg.data.append(BYTE0(temp));
        temp=data[i].log*100000000;
        this->log=temp;
        msg.data.append(BYTE4(temp));
        msg.data.append(BYTE3(temp));
        msg.data.append(BYTE2(temp));
        msg.data.append(BYTE1(temp));
        msg.data.append(BYTE0(temp));
    }        
    package->packMsg(&msg);
}

void DataProtocol::sendRoutesData(QList<RouteInfo> &data)
{
    Message msg;
    long long _temp;
    short temp;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_ROUTE);
    for(int i=0;i<data.size();i++)
    {
        _temp=data[i].lat*100000000;
        temp=_temp-this->lat;
        this->lat=_temp;
        msg.data.append(BYTE1(temp));
        msg.data.append(BYTE0(temp));
        _temp=data[i].log*100000000;
        temp=_temp-this->log;
        this->log=_temp;
        msg.data.append(BYTE1(temp));
        msg.data.append(BYTE0(temp));
    }
    package->packMsg(&msg);
}

void DataProtocol::sendPasstoGPS(QByteArray buf)
{
    QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(memcpy(buf.data(), "ok", 2) != 0)
    {
        Message msg;
        msg.msgID=Atk::D_CGPSCONTROL;
        msg.data.append(buf);

        QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        package->packGPSMsg(&msg);

        if(memcpy(buf.data(), "\x00\x00\x00\x00", 4) !=0)
        {
            getPathby4G();
        }
    }
}

void DataProtocol::sendPasstoGPS()
{
    Message msg;

    int temp = 111111;
    msg.msgID=Atk::D_CGPSCONTROL;
    msg.data.append(BYTE3(temp));
    msg.data.append(BYTE2(temp));
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));

    package->packGPSMsg(&msg);
}

void DataProtocol::getPassby4G()
{
    package->pack4gMsg("cmd=getpass&carid=1");
}

void DataProtocol::getPathby4G()
{
    QMessageBox::information(NULL, "Title", "getpath", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    package->pack4gMsg("cmd=getpath&carid=1");
}

void DataProtocol::sendWorkStateby4G()
{
    package->pack4gMsg("cmd=workfinish&carid=1");
}


