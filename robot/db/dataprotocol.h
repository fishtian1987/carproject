#ifndef DATAPROTOCOL_H
#define DATAPROTOCOL_H

#include <QString>
#include <QByteArray>
#include "info/routeinfo.h"
#include "utils/usartdriver.h"

QT_BEGIN_NAMESPACE
class Package;
QT_END_NAMESPACE

class DataProtocol
{
public:
    DataProtocol();
    ~DataProtocol();
    void sendBaseLocation(QList<RouteInfo> &);
    void sendRoutesData(QList<RouteInfo> &);
    void sendPasstoGPS();
    void sendPasstoGPS(QByteArray buf);
    void getPassby4G();
private:
    long long lat;
    long long log;
    Package *package;
};

#endif // DATAPROTOCOL_H
