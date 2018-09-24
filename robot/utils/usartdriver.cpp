#include "usartdriver.h"
#include "utils/utils.h"
#include <QByteArray>

UsartDriver::UsartDriver()
{

}

UsartDriver::~UsartDriver()
{
    closeUsart();
    if(serial!=NULL)
        delete serial;
}

UsartDriver::CGarboU  UsartDriver::cGarboU;
UsartDriver *UsartDriver::usart=new UsartDriver;

UsartDriver* UsartDriver::instance()
{
    return usart;
}

void UsartDriver::openUsart()
{
    serial = new QSerialPort;
    serial->setPortName(portName);
    serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(readUsart()));
}

void UsartDriver::closeUsart()
{
    if(serial->isOpen()){
        serial->clear();
        serial->close();
        serial->deleteLater();
    }
}

void UsartDriver::writeUsart(const QByteArray &data)
{
    serial->write(data);
}

void UsartDriver::readUsart()
{
    if (serial->bytesAvailable() <= 0) {
        return;
    } 
    Utils::Delay_MSec(50);
    QByteArray buf=serial->readAll();
    if(!buf.isEmpty()){
        emit allPreprocessorDone(buf);
        buf.clear();
    }
}

////////////////////////////////////////////////////////////////////

Usart4GDriver::Usart4GDriver()
{

}

Usart4GDriver::~Usart4GDriver()
{
    closeUsart();
    if(serial!=NULL)
        delete serial;
}

Usart4GDriver::CGarboU  Usart4GDriver::cGarboU;
Usart4GDriver *Usart4GDriver::usart=new Usart4GDriver;

Usart4GDriver* Usart4GDriver::instance()
{
    return usart;
}

void Usart4GDriver::openUsart()
{
    serial = new QSerialPort;
    serial->setPortName(portName);
    serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(readUsart()));
}

void Usart4GDriver::closeUsart()
{
    if(serial->isOpen()){
        serial->clear();
        serial->close();
        serial->deleteLater();
    }
}

void Usart4GDriver::writeUsart(const QByteArray &data)
{
    serial->write(data);
}

void Usart4GDriver::readUsart()
{
    if (serial->bytesAvailable() <= 0) {
        return;
    }
    Utils::Delay_MSec(50);
    QByteArray buf=serial->readAll();
    if(!buf.isEmpty()){
        emit allPreprocessorDone(buf);
        buf.clear();
    }
}

////////////////////////////////////////////////////////////////////

UsartGPSDriver::UsartGPSDriver()
{

}

UsartGPSDriver::~UsartGPSDriver()
{
    closeUsart();
    if(serial!=NULL)
        delete serial;
}

UsartGPSDriver::CGarboU  UsartGPSDriver::cGarboU;
UsartGPSDriver *UsartGPSDriver::usart=new UsartGPSDriver;

UsartGPSDriver* UsartGPSDriver::instance()
{
    return usart;
}

void UsartGPSDriver::openUsart()
{
    serial = new QSerialPort;
    serial->setPortName(portName);
    serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(readUsart()));
}

void UsartGPSDriver::closeUsart()
{
    if(serial->isOpen()){
        serial->clear();
        serial->close();
        serial->deleteLater();
    }
}

void UsartGPSDriver::writeUsart(const QByteArray &data)
{
    serial->write(data);
}

void UsartGPSDriver::readUsart()
{
    if (serial->bytesAvailable() <= 0) {
        return;
    }
    Utils::Delay_MSec(50);
    QByteArray buf=serial->readAll();
    if(!buf.isEmpty()){
        emit allPreprocessorDone(buf);
        buf.clear();
    }
}




