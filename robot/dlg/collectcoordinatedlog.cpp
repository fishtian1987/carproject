#include "collectcoordinatedlog.h"
#include "utils/utils.h"
#include "dlg/hintdialog.h"
#include "info/message.h"
#include "info/atk.h"
#include "db/package.h"
#include "db/databasehelper.h"
#include "com/myspinbox.h"
#include "com/mylineedit.h"
#include <QtWidgets>

CollectCoordinateDlog::CollectCoordinateDlog(QWidget *parent) :
    QDialog(parent)
{
    package=new Package;
    dao=DatabaseHelper::instance();
    count=0;
    QLabel *lable0=new QLabel("文件名称:");
    fileLineEdit=new MyLineEdit;
    lable0->setBuddy(fileLineEdit);
    QHBoxLayout *HLayout1 = new QHBoxLayout;
    HLayout1->addWidget(lable0);
    HLayout1->addWidget(fileLineEdit);

    QLabel *lable1=new QLabel("上一坐标:");
    oldLabel=new QLabel;
    QHBoxLayout *HLayout2 = new QHBoxLayout;
    HLayout2->addWidget(lable1);
    HLayout2->addWidget(oldLabel);

    QLabel *lable2=new QLabel("当前坐标:");
    newLabel=new QLabel;
    QHBoxLayout *HLayout3 = new QHBoxLayout;
    HLayout3->addWidget(lable2);
    HLayout3->addWidget(newLabel);

    QLabel *lable3=new QLabel("差值:");
    diffLabel=new QLabel();
    QLabel *lable4=new QLabel("米");
    QLabel *lable5=new QLabel("计数:");
    countLabel=new QLabel();
    QHBoxLayout *HLayout4 = new QHBoxLayout;
    HLayout4->addWidget(lable3);
    HLayout4->addWidget(diffLabel);
    HLayout4->addWidget(lable4);
    HLayout4->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    HLayout4->addWidget(lable5);
    HLayout4->addWidget(countLabel);
    HLayout4->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    QLabel *lable6=new QLabel("时间间隔:");
    intervaSpinBox=new MySpinBox;
    QLabel *lable7=new QLabel("秒:");
    QHBoxLayout *HLayout5 = new QHBoxLayout;
    HLayout5->addWidget(lable6);
    HLayout5->addWidget(intervaSpinBox);
    HLayout5->addWidget(lable7);
    HLayout5->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    checkBox=new QCheckBox("存储坐标");

    startButton=new QPushButton("开始");
    stopButton=new QPushButton("停止");
    closeButton=new QPushButton("关闭");
    QHBoxLayout *HLayout6 = new QHBoxLayout;
    HLayout6->addWidget(startButton);
    HLayout6->addWidget(stopButton);
    HLayout6->addWidget(closeButton);

    toolButton=new QToolButton;
    toolButton->setIcon(QIcon(":/images/help.png"));
    toolButton->setIconSize(QSize(16,16));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolButton);
    mainLayout->addLayout(HLayout1);
    mainLayout->addLayout(HLayout2);
    mainLayout->addLayout(HLayout3);
    mainLayout->addLayout(HLayout4);
    mainLayout->addLayout(HLayout5);
    mainLayout->addWidget(checkBox);
    mainLayout->addLayout(HLayout6);

    QGroupBox *groupBox=new QGroupBox("数据采集");
    groupBox->setLayout(mainLayout);
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(groupBox);
    setLayout(grid);
    setWindowTitle("数据采集");
    setFixedHeight(sizeHint().height());
    Utils::formInCenter(this,0.7);

    fileLineEdit->setText(Utils::GetCurrentSystemDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    oldLabel->setText("<p><font color=blue>00.00000000 000.00000000</font>");
    newLabel->setText("<p><font color=blue>00.00000000 000.00000000</font>");
    diffLabel->setText("<p><font color=blue>0.00</font>");
    countLabel->setText("<p><font color=blue>0</font>");
    intervaSpinBox->setValues(10);
    timer=new QTimer(this);
    connectAction();
    fileLineEdit->installEventFilter(this);
}

CollectCoordinateDlog::~CollectCoordinateDlog()
{
    timer->stop();
    delete timer;
    delete package;
    delete fileLineEdit;
    delete oldLabel;
    delete newLabel;
    delete diffLabel;
    delete countLabel;
    delete intervaSpinBox;
    delete checkBox;
    delete startButton;
    delete stopButton;
    delete closeButton;
    delete toolButton;
}

void CollectCoordinateDlog::connectAction()
{
    QObject::connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(startButton,SIGNAL(clicked()),this,SLOT(startCollectionsSlot()));
    connect(stopButton,SIGNAL(clicked()),this,SLOT(stopCollectionsSlot()));
    connect(toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
    connect(intervaSpinBox,SIGNAL(valueChanged(int)),intervaSpinBox,SLOT(valueChange(int)));
}

void CollectCoordinateDlog::startCollectionsSlot()
{
    if(checkBox->isChecked()){
        QString file=fileLineEdit->text();
        if(file.isEmpty()){
            HintDialog dialog(this);
            dialog.setText("<h2><font color=red>警告</font></h2>"
                        "<p><font color=blue>文件名不能为空.</font>");
            dialog.exec();
            return;
        }
        else{
            file.trimmed();
            file.replace("-","");
            file.replace(":","");
            file.replace(" ","");
            dao->createSqlTable("Dot"+file);
        }
    }
    short timerInterva=intervaSpinBox->value();
    if(timerInterva==0){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                    "<p><font color=blue>时间间隔不能为零.</font>");
        dialog.exec();
        return;
    }
    timer->start(timerInterva*1000);
    count=0;
}

void CollectCoordinateDlog::stopCollectionsSlot()
{
    if(timer->isActive())
        timer->stop();
}

void CollectCoordinateDlog::timerEvent()
{
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_COORD);
    package->packMsg(&msg);
}

void CollectCoordinateDlog::analysisDataSlot(QByteArray &data)
{
    compoundCoordinate(data);
    showInfo();
    if(checkBox->isChecked())
        dao->insertSqlDatabase(info.lat,info.log);
}

void CollectCoordinateDlog::compoundCoordinate(QByteArray &data)
{
    long long temp,_temp=0;
    for(int i=1;i<6;i++){
        temp=(uint8_t)data.at(i);
        _temp|=temp<<((5-i)*8);
    }
    info.lat=_temp/100000000.0;
    _temp=0;
    for(int i=6;i<11;i++){
        temp=(uint8_t)data.at(i);
         _temp|=temp<<((10-i)*8);
    }
    info.log=_temp/100000000.0;
    if(count++==0)
        oldinfo=info;
}

float CollectCoordinateDlog::getDistance()
{
    double dis;
    if(count>1){
        dis=Utils::CalculateDistance(info,oldinfo);
        oldinfo=info;
        return dis;
    }
    return 0;
}

void CollectCoordinateDlog::showInfo()
{
    oldLabel->setText(newLabel->text());
    QString str = QString::number(info.lat,'f', 8);
    str+=" "+QString::number(info.log,'f', 8);
    newLabel->setText("<p><font color=blue>"+str+"</font>");
    str = QString::number(count);
    countLabel->setText("<p><font color=blue>"+str+"</font>");
    str = QString::number(getDistance(), 'f', 2);
    diffLabel->setText("<p><font color=blue>"+str+"</font>");
}

void CollectCoordinateDlog::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<p><font color=blue>文件名:默认为系统当前时间,您可以自定义.</font>"
                   "<p><font color=blue>存储坐标:勾选时数据写入数据库;不勾选时,您可以</font>"
                   "<p><font color=blue>调整时间间隔数值,查看坐标采集是否满足要求,</font>"
                   "<p><font color=blue>建议不要超过25米.</font>");
    dialog.exec();
}

bool CollectCoordinateDlog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==fileLineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             fileLineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             fileLineEdit->setPalette(p);
        }
    }
    return QWidget::eventFilter(watched,event);
}
