#include "utils.h"
#include "info/atk.h"
#include "info/routeinfo.h"
#include <QtWidgets>

Utils::Utils()
{

}

void Utils::Delay_MSec(int msec)
{
    QTime _Timer = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < _Timer )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Utils::Delay_MSec_Suspend(int msec)
{
    QTime _Timer = QTime::currentTime();
    QTime _NowTimer;
    do{
        _NowTimer=QTime::currentTime();
    }while (_Timer.msecsTo(_NowTimer)<=msec);
}

QDateTime Utils::GetCurrentSystemDateTime()
{
    return QDateTime::currentDateTime();
}

void Utils::formInCenter(QWidget *frm,float fac)
{
    QDesktopWidget w;
    QPoint movePoint(w.availableGeometry().width()*fac / 2 - frm->width()/ 2, w.availableGeometry().height() / 2 - frm->height() / 2);
    frm->move(movePoint);
}

float Utils::CalculateDistance(RouteInfo &pointA,RouteInfo &pointB)
{
    double latA=(90.0-pointA.lat)*Atk::DEG2RAD;
    double latB=(90.0-pointB.lat)*Atk::DEG2RAD;
    double C = sin(latA)*sin(latB)*cos((pointA.log-pointB.log)*Atk::DEG2RAD) + cos(latA)*cos(latB);
    return Atk::RADIUS*acos(C);
}

QStringList Utils::solveRoute(unsigned short **graph,unsigned short n,unsigned short d,unsigned short m)
{
    QStringList list;
    unsigned short i,j,u,k,min;
    unsigned short *dis=new unsigned short[n*sizeof(unsigned short)];
    unsigned short *shortest=new  unsigned short[n*sizeof(unsigned short)];
    bool *book=new bool[n*sizeof(bool)];
    short *path=new short[n*sizeof(short)];

    for(i=0;i<n;i++){
        dis[i]=graph[d][i];
        book[i]=false;
        shortest[i]=0;
        if(i!=d&&dis[i]<Atk::INF)path[i]=d;
        else path[i]=-1;
    }

    dis[d]=0;
    book[d]=true;

   for(i=0;i<n-1;i++){
       min=Atk::INF;
       u=d;
       for(j=0;j<n;j++){
           if(!book[j] && dis[j]<min){
               min=dis[j];
               u=j;
           }
       }
       book[u]=true;
       for(j=0;j<n;j++){
           if(!book[j]&&graph[u][j]<Atk::INF&&dis[j]>dis[u]+graph[u][j])
           { 
               dis[j]=dis[u]+graph[u][j];
               path[j]=u;
           }
       }
   }

    list.append(QString::number(dis[m]));
    k=0;
    shortest[k]=m;
    while(path[shortest[k]]!=d){
      k++;
      shortest[k]=path[shortest[k-1]];
    }
    shortest[++k]=d;
    for(j=k;j>0;j--)
        list.append(QString::number(shortest[j]));
    list.append(QString::number(shortest[0]));
    delete[] dis;
    delete[] book;
    delete[] path;
    delete[] shortest;

    return list;
}

