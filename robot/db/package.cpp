#include "package.h"
#include "info/message.h"
#include "utils/usartdriver.h"
#include "info/atk.h"
#include <QDebug>

Package::Package()
{
    usart=UsartDriver::instance();
    usart4g=Usart4GDriver::instance();
    usartgps=UsartGPSDriver::instance();
}

Package::~Package()
{

}

void Package::packMsg(Message *msg)
{
    uint8_t cksum = 0;
    QByteArray sendBuffer;
    sendBuffer.append(Atk::UP_BYTE1);
    sendBuffer.append(Atk::UP_BYTE2);
    sendBuffer.append(msg->msgID);
    sendBuffer.append(msg->data.size());
    sendBuffer.append(msg->data,msg->data.size());

    for (int i=0; i<sendBuffer.size(); i++)
        cksum += sendBuffer[i];
    sendBuffer.append(cksum);

    for(int i=0;i<sendBuffer.size();i++)
        qDebug()<<(uint8_t)sendBuffer.at(i);

    usart->writeUsart(sendBuffer);
}

void Package::pack4gMsg(QString msg)
{
    QByteArray sendBuffer;
    sendBuffer.append(msg);
    usart4g->writeUsart(sendBuffer);
}

void Package::packGPSMsg(Message *msg)
{
    uint8_t cksum = 0;
    QByteArray sendBuffer;
    sendBuffer.append(Atk::UP_BYTE1);
    sendBuffer.append(Atk::UP_BYTE2);
    sendBuffer.append(msg->msgID);
    sendBuffer.append(msg->data.size());
    sendBuffer.append(msg->data,msg->data.size());

    for (int i=0; i<sendBuffer.size(); i++)
        cksum += sendBuffer[i];
    sendBuffer.append(cksum);

    for(int i=0;i<sendBuffer.size();i++)
        qDebug()<<(uint8_t)sendBuffer.at(i);

    usartgps->writeUsart(sendBuffer);
}

Message Package::unpackMsg(QByteArray &buf)
{
    uint8_t cksum = 0;
    Message msg;

    if(((uint8_t)buf.at(0)==Atk::DOWN_BYTE1)&&((uint8_t)buf.at(1)==Atk::DOWN_BYTE2))
    {
        for(int i=0;i<buf.size()-1;i++)
            cksum += buf[i];
        if(buf.endsWith(cksum))
        {
            msg.msgID=buf.at(2);
            msg.data=buf.mid(4,buf.size()-5);
        }
    }
    return msg;
}

