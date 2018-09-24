#ifndef TIMERUTILS_H
#define TIMERUTILS_H

#include <QTime>

QT_BEGIN_NAMESPACE
class QWidget;
class RouteInfo;
QT_END_NAMESPACE

class Utils
{
public:
    Utils();
    static void Delay_MSec(int);
    static void Delay_MSec_Suspend(int);
    static QDateTime GetCurrentSystemDateTime();
    static void formInCenter(QWidget *,float);
    static float CalculateDistance(RouteInfo &,RouteInfo &);
    static QStringList solveRoute(unsigned short**,unsigned short,unsigned short,unsigned short);
};

#endif // TIMERUTILS_H
