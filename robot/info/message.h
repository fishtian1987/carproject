#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>

class Message
{
public:
    Message();
    uint8_t msgID;
    QByteArray data;
};

#endif // MESSAGE_H
