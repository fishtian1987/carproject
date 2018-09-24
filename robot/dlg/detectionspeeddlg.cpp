#include "detectionspeeddlg.h"
#include "ui_detectionspeeddlg.h"
#include "utils/utils.h"
#include "dlg/hintdialog.h"
#include "db/package.h"
#include "info/message.h"
#include "info/atk.h"
#include <QtWidgets>

DetectionSpeedDlg::DetectionSpeedDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetectionSpeedDlg)
{
    ui->setupUi(this);
    package=new Package;
    timer=new QTimer(this);
    setWindowTitle("监测速度");
    Utils::formInCenter(this,1);
    QObject::connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(startDetecSpeedSlot()));
    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(stopDetecSpeedSlot()));
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
}

DetectionSpeedDlg::~DetectionSpeedDlg()
{
    timer->stop();
    delete timer;
    delete package;
    delete ui;
}

void DetectionSpeedDlg::startDetecSpeedSlot()
{
    timer->start(200);
}

void DetectionSpeedDlg::stopDetecSpeedSlot()
{
    if(timer->isActive())
        timer->stop();
}

void DetectionSpeedDlg::timerEvent()
{
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_DETECTSPEED);
    package->packMsg(&msg);
}

void DetectionSpeedDlg::analysisDataSlot(QByteArray &data)
{
    short _temp;
    _temp=(uint8_t)data.at(1)<<8;
    _temp|=(uint8_t)data.at(2);
    QString str = QString::number(_temp);
    ui->RFLabel->setText("<p><font color=blue>"+str+"</font>");
    _temp=(uint8_t)data.at(3)<<8;
    _temp|=(uint8_t)data.at(4);
    str = QString::number(_temp);
    ui->LFLabel->setText("<p><font color=blue>"+str+"</font>");
    _temp=(uint8_t)data.at(5)<<8;
    _temp|=(uint8_t)data.at(6);
    str = QString::number(_temp);
    ui->LBLabel->setText("<p><font color=blue>"+str+"</font>");
    _temp=(uint8_t)data.at(7)<<8;
    _temp|=(uint8_t)data.at(8);
    str = QString::number(_temp);
    ui->RBLabel->setText("<p><font color=blue>"+str+"</font>");
    _temp=(uint8_t)data.at(9)<<8;
    _temp|=(uint8_t)data.at(10);
    str = QString::number(_temp/100.0);
    ui->speedLabel->setText("<p><font color=blue>"+str+"</font>");
}

void DetectionSpeedDlg::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<h2><font color=red>监测速度</font></h2>"
                   "<p><font color=blue>监测每个轮子的转速及整车速度.</font>");
    dialog.exec();
}
