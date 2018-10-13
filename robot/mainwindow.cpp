#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlg/magdriftangledlg.h"
#include "dlg/locationdriftdlg.h"
#include "dlg/piddlog.h"
#include "dlg/remotespeeddlog.h"
#include "dlg/navigationspeeddlog.h"
#include "utils/usartdriver.h"
#include "db/databasehelper.h"
#include "dlg/collectcoordinatedlog.h"
#include "db/dataprotocol.h"
#include "dlg/autonavigationdlg.h"
#include "dlg/lookdatabasedlg.h"
#include "dlg/detectionspeeddlg.h"
#include "utils/utils.h"
#include "info/message.h"
#include "db/package.h"
#include "info/atk.h"
#include <QtWidgets>
#include "dlg/hintdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sendDataState=Atk::PREPARE_STATE;
    dataPro=new DataProtocol;
    packet=new Package;
    thread.setDatabaseHelp(DatabaseHelper::instance());
    QObject::connect(ui->actionMagDriftAngle,SIGNAL(triggered()),this,SLOT(openMagDriftAngleDlg()));
    connect(ui->actionGpsBias,SIGNAL(triggered()),this,SLOT(openLocationDriftDlg()));
    connect(ui->actionPID,SIGNAL(triggered()),this,SLOT(openPIDDlg()));
    connect(ui->actionRemoteSpeed,SIGNAL(triggered()),this,SLOT(openRemoteSpeedDlg()));
    connect(ui->actionNavSpeed,SIGNAL(triggered()),this,SLOT(openNavigationSpeedDlg()));
    connect(ui->actionCollectCoord,SIGNAL(triggered()),this,SLOT(openCollectCoordinateDlg()));
    connect(ui->actionNavigation,SIGNAL(triggered()),this,SLOT(openAutoNavigationDlg()));
    connect(ui->actionLook,SIGNAL(triggered()),this,SLOT(openLookDatabaseDlg()));
    connect(ui->actionDetecpeed,SIGNAL(triggered()),this,SLOT(openDetectionSpeedDlg()));
    connect(&thread, SIGNAL(allTransactionsDone()),this, SLOT(allTransactionsDone()));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(test()));
    initialize();
}

void MainWindow::test()
{
//    static int number=0;
//    if(number==0){
//        SetSendDataState(Atk::C_ROUTE_BIAS);
//        addTransaction(new queryDBTransaction(0));
//    }
//    else{
//         SetSendDataState(Atk::C_ROUTE);
//        addTransaction(new queryDBTransaction(10));
//    }
//    qDebug()<<"基数"<<number;
//   number++;
    HintDialog dialog(this);
    dialog.setText("<h2><font color=red>警告</font></h2>"
                   "<p><font color=blue>速度因子</font>"
                   "<p><font color=blue>参数必须在0~10之间.</font>");
    dialog.exec();

    Usart4GDriver *usart4g;
    usart4g=Usart4GDriver::instance();
    QByteArray sendBuffer;
    sendBuffer.append("aa");
    usart4g->writeUsart(sendBuffer);
}

MainWindow::~MainWindow()
{
    if(dataPro!=NULL)
        delete dataPro;
    if(packet!=NULL)
        delete packet;
    delete ui;

}

void MainWindow::initialize()
{
   openUsart();
   openDatabase();
}

void MainWindow::openUsart()
{
    UsartDriver *usart=UsartDriver::instance();
    usart->openUsart();
    connect(usart,SIGNAL(allPreprocessorDone(QByteArray &)),this,SLOT(allPreprocessorDone(QByteArray &)));

    Usart4GDriver *usart4g = Usart4GDriver::instance();
    usart4g->openUsart();
    connect(usart4g, SIGNAL(allPreprocessorDone(QByteArray&)), this, SLOT(allPreprocessorDone(QByteArray&)));

    UsartGPSDriver *usartgps = UsartGPSDriver::instance();
    usartgps->openUsart();
    connect(usartgps, SIGNAL(allPreprocessorDone(QByteArray&)), this, SLOT(allPreprocessorDone(QByteArray&)));
}

void MainWindow::openDatabase()
{
    DatabaseHelper *dao=DatabaseHelper::instance();
    if(!dao->openSqlDatabase()){
        int r=QMessageBox::warning(this,"系统提示","数据库打开失败:"+dao->errorLog(),
                                   QMessageBox::Yes | QMessageBox::Retry| QMessageBox::Cancel);
        if (r == QMessageBox::Retry) {
            openDatabase();
        }
    }
}

void MainWindow::allPreprocessorDone(QByteArray &data)
{
    Message msg;
    msg=packet->unpackMsg(data);

    switch(msg.msgID){
    case Atk::INVALID:break;
    case Atk::D_CMAINCONTROL:{
        analysisMainData(msg.data);
    } break;
    case Atk::D_CSUBCONTROL:{
        //analysisSubData(msg.data);
    }break;
    case Atk::D_C4GCONTROL:{
        analysis4GData(msg.data);
    }break;
    case Atk::D_CGPSCONTROL:{
        analysisGPSData(msg.data);
    }
    default:break;
    }
}

void MainWindow::analysisMainData(QByteArray &data)
{
    switch(data.at(0)){
    case Atk::C_ROUTE_BIAS:{
        SetSendDataState(Atk::C_ROUTE_BIAS);
        addTransaction(new queryDBTransaction(0));
    }break;
    case Atk::C_ROUTE:{
        SetSendDataState(Atk::C_ROUTE);
        senRoutesData();
    }break;
    case Atk::C_COORD:{
        emit collectCoordinateSignal(data);
    }break;
    case Atk::C_MAGBIAS:{
        emit magdriftangleSignal(data);
    }break;
    case Atk::C_LOCAVPT:{
        emit locationDriftSignal(data);
    }break;
    case Atk::C_PID:{
        emit pidSignal(data);
    }break;
    case Atk::C_REMOTESPEED:{
        emit remoteSpeedSignal(data);
    }break;
    case Atk::C_AUTOSPEED:{
        emit navigationSpeedSignal(data);
    }break;
    case Atk::C_DETECTSPEED:{
        emit detectionSpeedSignal(data);
    }break;
    default:break;
    }
}

//void MainWindow::analysisSubData(QByteArray &data)
//{

//}

void MainWindow::analysis4GData(QByteArray &data)
{
    switch(data.at(0)){
    case Atk::C_PASS_4G:{

    }break;
    default:break;
    }
}

void MainWindow::analysisGPSData(QByteArray &data)
{
    switch (data.at(0)) {
    case Atk::C_PASS_GPS:{
        dataPro->getPassby4G();
    }break;
    default:break;
    }
}

void MainWindow::allTransactionsDone()
{
    QList<RouteInfo> data=thread.routeData();

//    for(int i=0;i<data.size();i++)
//        qDebug()<<QString::number(data.at(i).lat,'f',8)<<" "<<QString::number(data.at(i).log,'f',8);
    switch(GetSendDataState())
    {
    case Atk::PREPARE_STATE:
        break;
    case Atk::C_ROUTE_BIAS:
    {
        dataPro->sendBaseLocation(data);
    }break;
    case Atk::C_ROUTE:
    {
        dataPro->sendRoutesData(data);
    }break;
    default:break;
    }
}

void MainWindow::senRoutesData()
{
    addTransaction(new queryDBTransaction(10));
}

void MainWindow::addTransaction(Transaction *transact)
{
    thread.addTransaction(transact);
}

void MainWindow::SetSendDataState(uint8_t state)
{
    sendDataState=state;
}

uint8_t MainWindow::GetSendDataState()
{
    return sendDataState;
}

void MainWindow::openLookDatabaseDlg()
{
    LookDatabaseDlg dialog(this);
    dialog.exec();
}

void MainWindow::openMagDriftAngleDlg()
{
     MagDriftAngleDlg dialog(this);
     connect(this,SIGNAL(magdriftangleSignal(QByteArray &)),&dialog,SLOT(analysisDataSlot(QByteArray &)));
     dialog.exec();
}

void MainWindow::openLocationDriftDlg()
{
    LocationDriftDlg dialog(this);
    connect(this,SIGNAL(locationDriftSignal(QByteArray &)),&dialog,SLOT(analysisDataSlot(QByteArray &)));
    dialog.exec();
}

void MainWindow::openPIDDlg()
{
    PIDDlog dialog(this);
    connect(this,SIGNAL(pidSignal(QByteArray &)),&dialog,SLOT(analysisDataSlot(QByteArray &)));
    dialog.exec();
}

void MainWindow::openRemoteSpeedDlg()
{
    RemoteSpeedDlog dialog(this);
    connect(this,SIGNAL(remoteSpeedSignal(QByteArray &)),&dialog,SLOT(analysisDataSlot(QByteArray &)));
    dialog.exec();
}

void MainWindow::openNavigationSpeedDlg()
{
    NavigationSpeedDlog dialog(this);
    connect(this,SIGNAL(navigationSpeedSignal(QByteArray &)),&dialog,SLOT(analysisDataSlot(QByteArray &)));
    dialog.exec();
}

void MainWindow::openCollectCoordinateDlg()
{
    CollectCoordinateDlog dialog(this);
    connect(this,SIGNAL(collectCoordinateSignal(QByteArray &)),&dialog,SLOT(analysisDataSlot(QByteArray &)));
    dialog.exec();
}

void MainWindow::openAutoNavigationDlg()
{
    AutoNavigationDlg dialog(this);
    if(dialog.exec()){
        if(dialog.getControlMode()){
            Utils::Delay_MSec(5000);
            SetSendDataState(Atk::C_ROUTE_BIAS);
            addTransaction(new queryDBTransaction(0));
        }
    }
}

void MainWindow::openDetectionSpeedDlg()
{
    DetectionSpeedDlg dialog(this);
    connect(this,SIGNAL(detectionSpeedSignal(QByteArray &)),&dialog,SLOT(analysisDataSlot(QByteArray &)));
    dialog.exec();
}
