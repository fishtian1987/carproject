#ifndef USARTDRIVER_H
#define USARTDRIVER_H

#include <QString>
#include <QSerialPort>
#include <QObject>

class UsartDriver:public QObject
{
     Q_OBJECT
private:
    UsartDriver();
    static UsartDriver *usart;
    class CGarboU
    {
    public:
        ~CGarboU ()
        {
           if(UsartDriver::usart != NULL)
           {
               delete UsartDriver::usart;
               UsartDriver::usart = NULL;
           }
        }
    };

public:
    ~UsartDriver();
    static UsartDriver* instance();
    void openUsart();
    void closeUsart();
    void writeUsart(const QByteArray &data);
signals:
    void allPreprocessorDone(QByteArray &);
private slots:
    void readUsart();
private:
    static CGarboU  cGarboU;
    const QString portName = "/dev/ttySAC2";
    qint32 baudRate=115200;
    QSerialPort *serial;
};

class Usart4GDriver:public QObject
{
     Q_OBJECT
private:
    Usart4GDriver();
    static Usart4GDriver *usart;
    class CGarboU
    {
    public:
        ~CGarboU ()
        {
           if(Usart4GDriver::usart != NULL)
           {
               delete Usart4GDriver::usart;
               Usart4GDriver::usart = NULL;
           }
        }
    };

public:
    ~Usart4GDriver();
    static Usart4GDriver* instance();
    void openUsart();
    void closeUsart();
    void writeUsart(const QByteArray &data);
signals:
    void allPreprocessorDone(QByteArray &);
private slots:
    void readUsart();
private:
    static CGarboU  cGarboU;
    const QString portName = "/dev/ttySAC3";
    qint32 baudRate=115200;
    QSerialPort *serial;
};

class UsartGPSDriver:public QObject
{
     Q_OBJECT
private:
    UsartGPSDriver();
    static UsartGPSDriver *usart;
    class CGarboU
    {
    public:
        ~CGarboU ()
        {
           if(UsartGPSDriver::usart != NULL)
           {
               delete UsartGPSDriver::usart;
               UsartGPSDriver::usart = NULL;
           }
        }
    };

public:
    ~UsartGPSDriver();
    static UsartGPSDriver* instance();
    void openUsart();
    void closeUsart();
    void writeUsart(const QByteArray &data);
signals:
    void allPreprocessorDone(QByteArray &);
private slots:
    void readUsart();
private:
    static CGarboU  cGarboU;
    const QString portName = "/dev/ttySAC4";
    qint32 baudRate=115200;
    QSerialPort *serial;
};


#endif // USARTDRIVER_H
