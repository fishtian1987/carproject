#ifndef PACKAGE_H
#define PACKAGE_H

#include <QByteArray>

QT_BEGIN_NAMESPACE
class UsartDriver;
class Usart4GDriver;
class Message;
QT_END_NAMESPACE

class Package
{
public:
    Package();
    ~Package();
    void packMsg(Message *);
    void pack4gMsg(Message *msg);
    Message unpackMsg(QByteArray &buf);

private:
    UsartDriver *usart;
    Usart4GDriver *usart4g;
};

#endif // PACKAGE_H
