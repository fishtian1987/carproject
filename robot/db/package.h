#ifndef PACKAGE_H
#define PACKAGE_H

#include <QByteArray>

QT_BEGIN_NAMESPACE
class UsartDriver;
class Usart4GDriver;
class UsartGPSDriver;
class Message;
QT_END_NAMESPACE

class Package
{
public:
    Package();
    ~Package();
    void packMsg(Message *);
    void pack4gMsg(QString msg);
    void packGPSMsg(Message *msg);
    Message unpackMsg(QByteArray &buf);

private:
    UsartDriver *usart;
    Usart4GDriver *usart4g;
    UsartGPSDriver *usartgps;
};

#endif // PACKAGE_H
