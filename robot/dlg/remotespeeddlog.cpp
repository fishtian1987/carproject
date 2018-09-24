#include "remotespeeddlog.h"
#include "info/atk.h"
#include "info/message.h"
#include "db/package.h"
#include "dlg/hintdialog.h"
#include "com/mylineedit.h"
#include "utils/utils.h"
#include <QtWidgets>

RemoteSpeedDlog::RemoteSpeedDlog(QWidget *parent) :
    QDialog(parent)
{
    package=new Package;

    QLabel *label1=new QLabel("FB因子:");
    FBlineEdit=new MyLineEdit;
    label1->setBuddy(FBlineEdit);

    QLabel *label2=new QLabel("R因子:");
    RlineEdit=new MyLineEdit;
    label2->setBuddy(RlineEdit);

    QLabel *label3=new QLabel("LR因子:");
    LRlineEdit=new MyLineEdit;
    label3->setBuddy(LRlineEdit);

    QLabel *label4=new QLabel("极限速度:");
    LMlineEdit=new MyLineEdit;
    label4->setBuddy(LMlineEdit);

    readButton=new QPushButton("读取");
    setButton=new QPushButton("设置");
    closeButton=new QPushButton("关闭");

    toolButton=new QToolButton;
    toolButton->setIcon(QIcon(":/images/help.png"));
    toolButton->setIconSize(QSize(16,16));

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(toolButton,0,0,1,1,Qt::AlignHCenter);
    grid->addWidget(label1,1,0,1,1,Qt::AlignHCenter);
    grid->addWidget(FBlineEdit,1,1,1,1);
    grid->addWidget(label2,1,6,1,1,Qt::AlignHCenter);
    grid->addWidget(RlineEdit,1,7,1,1);

    grid->addWidget(label3,2,0,1,1,Qt::AlignHCenter);
    grid->addWidget(LRlineEdit,2,1,1,1);
    grid->addWidget(label4,2,6,1,1,Qt::AlignHCenter);
    grid->addWidget(LMlineEdit,2,7,1,1);

    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(20);
    grid->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout *HLayout1 = new QHBoxLayout;
    HLayout1->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    HLayout1->addWidget(readButton);
    HLayout1->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    HLayout1->addWidget(setButton);
    HLayout1->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    HLayout1->addWidget(closeButton);
    HLayout1->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

    QVBoxLayout *HLayout2 = new QVBoxLayout;
    HLayout2->addLayout(grid);
    HLayout2->addLayout(HLayout1);

    QGroupBox *groupBox=new QGroupBox("遥控速度设置");
    groupBox->setLayout(HLayout2);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(groupBox);
    setLayout(mainLayout);
    setWindowTitle("遥控速度设置");
    setFixedHeight(sizeHint().height());

    Utils::formInCenter(this,0.45);
    connectAction();
    FBlineEdit->installEventFilter(this);
    FBlineEdit->setText("0.0");
    RlineEdit->installEventFilter(this);
    RlineEdit->setText("0.0");
    LRlineEdit->installEventFilter(this);
    LRlineEdit->setText("0.0");
    LMlineEdit->installEventFilter(this);
    LMlineEdit->setText("0.0");
}

RemoteSpeedDlog::~RemoteSpeedDlog()
{
    delete package;
    delete FBlineEdit;
    delete RlineEdit;
    delete LRlineEdit;
    delete LMlineEdit;
    delete readButton;
    delete setButton;
    delete closeButton;
    delete toolButton;
}

void RemoteSpeedDlog::connectAction()
{
    QObject::connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(readButton,SIGNAL(clicked()),this,SLOT(readDataSlot()));
    connect(setButton,SIGNAL(clicked()),this,SLOT(setDataSlot()));
    connect(toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
}

void RemoteSpeedDlog::analysisDataSlot(QByteArray &data)
{
    unsigned short _temp;
    _temp=(uint8_t)data.at(1);
    FBlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    _temp=(uint8_t)data.at(2);
    RlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    _temp=(uint8_t)data.at(3);
    LRlineEdit->setText(QString::number(_temp/10.0,'f', 1));
    _temp=(uint8_t)data.at(4)<<8;
    _temp|=(uint8_t)data.at(5);
    LMlineEdit->setText(QString::number(_temp));
}

void RemoteSpeedDlog::readDataSlot()
{
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_REMOTESPEED);
    package->packMsg(&msg);
}

void RemoteSpeedDlog::setDataSlot()
{
    short temp=FBlineEdit->text().toFloat()*10;
    short temp1=RlineEdit->text().toFloat()*10;
    short temp2=LRlineEdit->text().toFloat()*10;

    if((temp>100)|(temp<0)|(temp1>100)|(temp1<0)|(temp2>100)|(temp2<0)){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                       "<p><font color=blue>速度因子</font>"
                       "<p><font color=blue>参数必须在0~10之间.</font>");
        dialog.exec();
        return;
    }
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_REMOTESPEED);
    msg.data.append(BYTE0(temp));
    msg.data.append(BYTE0(temp1));
    msg.data.append(BYTE0(temp2));

    temp=LMlineEdit->text().toFloat();
    if((temp>10000)|(temp<0)){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                       "<p><font color=blue>极限速度</font>"
                       "<p><font color=blue>参数必须在0~10000之间.</font>");
        dialog.exec();
        return;
    }
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));
    package->packMsg(&msg);
}

void RemoteSpeedDlog::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<p><font color=blue>每次设置请先读取数值.</font>"
                   "<p><font color=blue>确认之后在进行设置.</font>"
                   "<p><font color=blue>因子参数必须在0~10之间.</font>"
                   "<p><font color=blue>极限参数必须在0~10000之间.</font>");
    dialog.exec();
}

bool RemoteSpeedDlog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==FBlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             FBlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             FBlineEdit->setPalette(p);
        }
    }

    if (watched==RlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             RlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             RlineEdit->setPalette(p);
        }
    }

    if (watched==LRlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             LRlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             LRlineEdit->setPalette(p);
        }
    }

    if (watched==LMlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             LMlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             LMlineEdit->setPalette(p);
        }
    }
    return QWidget::eventFilter(watched,event);
}
