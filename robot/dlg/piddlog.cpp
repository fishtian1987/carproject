#include "piddlog.h"
#include "db/package.h"
#include "com/mylineedit.h"
#include "dlg/hintdialog.h"
#include "info/atk.h"
#include "info/message.h"
#include "utils/utils.h"
#include <QtWidgets>

PIDDlog::PIDDlog(QWidget *parent) :
    QDialog(parent)
{
    package=new Package;

    QLabel *label1=new QLabel("P");
    QLabel *label2=new QLabel("I");
    QLabel *label3=new QLabel("D");

    QLabel *label4=new QLabel("速度:");
    SPlineEdit=new MyLineEdit;
    SIlineEdit=new MyLineEdit;
    SDlineEdit=new MyLineEdit;
    label4->setBuddy(SPlineEdit);

    QLabel *label5=new QLabel("转向:");
    RPlineEdit=new MyLineEdit;
    RIlineEdit=new MyLineEdit;
    RDlineEdit=new MyLineEdit;
    label5->setBuddy(RPlineEdit);

    readButton=new QPushButton("读取");
    setButton=new QPushButton("设置");
    closeButton=new QPushButton("关闭");

    toolButton=new QToolButton;
    toolButton->setIcon(QIcon(":/images/help.png"));
    toolButton->setIconSize(QSize(16,16));

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(toolButton,0,0,1,1,Qt::AlignHCenter);
    grid->addWidget(label1,0,1,1,1,Qt::AlignHCenter);
    grid->addWidget(label2,0,2,1,1,Qt::AlignHCenter);
    grid->addWidget(label3,0,3,1,1,Qt::AlignHCenter);
    grid->addWidget(label4,1,0,1,1,Qt::AlignHCenter);
    grid->addWidget(SPlineEdit,1,1,1,1);
    grid->addWidget(SIlineEdit,1,2,1,1);
    grid->addWidget(SDlineEdit,1,3,1,1);
    grid->addWidget(label5,2,0,1,1,Qt::AlignHCenter);
    grid->addWidget(RPlineEdit,2,1,1,1);
    grid->addWidget(RIlineEdit,2,2,1,1);
    grid->addWidget(RDlineEdit,2,3,1,1);

    grid->addWidget(readButton,3,1,1,1);
    grid->addWidget(setButton,3,2,1,1);
    grid->addWidget(closeButton,3,3,1,1);

    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(30);
    grid->setContentsMargins(20, 20, 20, 20);

    QGroupBox *groupBox=new QGroupBox("PID设置");
    groupBox->setLayout(grid);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(groupBox);
    setLayout(mainLayout);
    setWindowTitle("PID设置");
    setFixedHeight(sizeHint().height());
    Utils::formInCenter(this,0.45);
    connectAction();

    SPlineEdit->installEventFilter(this);
    SPlineEdit->setText("0.0");
    SIlineEdit->installEventFilter(this);
    SIlineEdit->setText("0.0");
    SDlineEdit->installEventFilter(this);
    SDlineEdit->setText("0.0");
    RPlineEdit->installEventFilter(this);
    RPlineEdit->setText("0.0");
    RIlineEdit->installEventFilter(this);
    RIlineEdit->setText("0.0");
    RDlineEdit->installEventFilter(this);
    RDlineEdit->setText("0.0");
}

PIDDlog::~PIDDlog()
{
    delete package;
    delete SPlineEdit;
    delete SIlineEdit;
    delete SDlineEdit;
    delete RPlineEdit;
    delete RIlineEdit;
    delete RDlineEdit;
    delete readButton;
    delete setButton;
    delete closeButton;
    delete toolButton;
}

void PIDDlog::connectAction()
{
    QObject::connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(readButton,SIGNAL(clicked()),this,SLOT(readDataSlot()));
    connect(setButton,SIGNAL(clicked()),this,SLOT(setDataSlot()));
    connect(toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
}

void PIDDlog::analysisDataSlot(QByteArray &data)
{
    qDebug()<<"开始";
    unsigned short _temp=0;
    _temp=(uint8_t)data.at(1)<<8;
    _temp|=(uint8_t)data.at(2);
    SPlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    _temp=(uint8_t)data.at(3)<<8;
    _temp|=(uint8_t)data.at(4);
    SIlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    _temp=(uint8_t)data.at(5)<<8;
    _temp|=(uint8_t)data.at(6);
    SDlineEdit->setText(QString::number(_temp/10.0,'f', 1));

    _temp=(uint8_t)data.at(7)<<8;
    _temp|=(uint8_t)data.at(8);
    RPlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    _temp=(uint8_t)data.at(9)<<8;
    _temp|=(uint8_t)data.at(10);
    RIlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    _temp=(uint8_t)data.at(11)<<8;
    _temp|=(uint8_t)data.at(12);
    RDlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    qDebug()<<"结束";
}

void PIDDlog::readDataSlot()
{
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_PID);
    package->packMsg(&msg);
}

void PIDDlog::setDataSlot()
{
    int temp=SPlineEdit->text().toFloat()*10;
    int temp1=SIlineEdit->text().toFloat()*10;
    int temp2=SDlineEdit->text().toFloat()*10;

    if((temp>65535)|(temp<0)|(temp1>65535)|(temp1<0)|(temp2>65535)|(temp2<0)){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                       "<p><font color=blue>速度PID</font>"
                       "<p><font color=blue>参数必须在0~6553.5之间.</font>");
        dialog.exec();
        return;
    }
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_PID);
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));
    msg.data.append(BYTE1(temp1));
    msg.data.append(BYTE0(temp1));
    msg.data.append(BYTE1(temp2));
    msg.data.append(BYTE0(temp2));

    temp=RPlineEdit->text().toFloat()*10;
    temp1=RIlineEdit->text().toFloat()*10;
    temp2=RDlineEdit->text().toFloat()*10;
    if((temp>65535)|(temp<0)|(temp1>65535)|(temp1<0)|(temp2>65535)|(temp2<0)){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                       "<p><font color=blue>转向PID</font>"
                       "<p><font color=blue>参数必须在0~6553.5之间.</font>");
        dialog.exec();
        return;
    }
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));
    msg.data.append(BYTE1(temp1));
    msg.data.append(BYTE0(temp1));
    msg.data.append(BYTE1(temp2));
    msg.data.append(BYTE0(temp2));
    package->packMsg(&msg);
}

void PIDDlog::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<p><font color=blue>每次设置请先读取数值.</font>"
                   "<p><font color=blue>确认之后在进行设置.</font>"
                   "<p><font color=blue>参数必须在0~6553.5之间.</font>");
    dialog.exec();
}

bool PIDDlog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==SPlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             SPlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             SPlineEdit->setPalette(p);
        }
    }

    if (watched==SIlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             SIlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             SIlineEdit->setPalette(p);
        }
    }

    if (watched==SDlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             SDlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             SDlineEdit->setPalette(p);
        }
    }

    if (watched==RPlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             RPlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             RPlineEdit->setPalette(p);
        }
    }

    if (watched==RIlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             RIlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             RIlineEdit->setPalette(p);
        }
    }

    if (watched==RDlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             RDlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             RDlineEdit->setPalette(p);
        }
    }

    return QWidget::eventFilter(watched,event);
}
