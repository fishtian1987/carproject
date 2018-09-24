#include "navigationspeeddlog.h"
#include "db/package.h"
#include "com/mylineedit.h"
#include "info/atk.h"
#include "info/message.h"
#include "dlg/hintdialog.h"
#include "utils/utils.h"
#include <QtWidgets>

NavigationSpeedDlog::NavigationSpeedDlog(QWidget *parent) :
    QDialog(parent)
{
    package=new Package;

    QLabel *label1=new QLabel("低速:");
    lowlineEdit=new MyLineEdit;
    label1->setBuddy(lowlineEdit);
    QHBoxLayout *HLayout1 = new QHBoxLayout;
    HLayout1->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding));
    HLayout1->addWidget(label1);
    HLayout1->addWidget(lowlineEdit);
    HLayout1->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding));

    QLabel *label2=new QLabel("中速:");
    midlineEdit=new MyLineEdit;
    label2->setBuddy(midlineEdit);
    QHBoxLayout *HLayout2 = new QHBoxLayout;
    HLayout2->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding));
    HLayout2->addWidget(label2);
    HLayout2->addWidget(midlineEdit);
    HLayout2->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding));

    QLabel *label3=new QLabel("高速:");
    higlineEdit=new MyLineEdit;
    label3->setBuddy(higlineEdit);
    QHBoxLayout *HLayout3 = new QHBoxLayout;
    HLayout3->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding));
    HLayout3->addWidget(label3);
    HLayout3->addWidget(higlineEdit);
    HLayout3->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding));

    readButton=new QPushButton("读取");
    setButton=new QPushButton("设置");
    closeButton=new QPushButton("关闭");
    QHBoxLayout *HLayout4 = new QHBoxLayout;
    HLayout4->addWidget(readButton);
    HLayout4->addWidget(setButton);
    HLayout4->addWidget(closeButton);

    toolButton=new QToolButton;
    toolButton->setIcon(QIcon(":/images/help.png"));
    toolButton->setIconSize(QSize(16,16));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolButton);
    mainLayout->addLayout(HLayout1);
    mainLayout->addLayout(HLayout2);
    mainLayout->addLayout(HLayout3);
    mainLayout->addLayout(HLayout4);

    QGroupBox *groupBox=new QGroupBox("导航速度");
    groupBox->setLayout(mainLayout);
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(groupBox);
    setLayout(grid);
    setWindowTitle("导航速度");
    setFixedHeight(sizeHint().height());
    Utils::formInCenter(this,0.75);

    connectAction();
    lowlineEdit->installEventFilter(this);
    lowlineEdit->setText("0");
    midlineEdit->installEventFilter(this);
    midlineEdit->setText("0");
    higlineEdit->installEventFilter(this);
    higlineEdit->setText("0");
}

NavigationSpeedDlog::~NavigationSpeedDlog()
{
    delete package;
    delete lowlineEdit;
    delete midlineEdit;
    delete higlineEdit;
    delete readButton;
    delete setButton;
    delete closeButton;
    delete toolButton;
}

void NavigationSpeedDlog::connectAction()
{
    QObject::connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(readButton,SIGNAL(clicked()),this,SLOT(readDataSlot()));
    connect(setButton,SIGNAL(clicked()),this,SLOT(setDataSlot()));
    connect(toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
}

void NavigationSpeedDlog::analysisDataSlot(QByteArray &data)
{
    unsigned short _temp=0;
    _temp=(uint8_t)data.at(1)<<8;
    _temp|=(uint8_t)data.at(2);
    lowlineEdit->setText(QString::number(_temp));
    _temp=(uint8_t)data.at(3)<<8;
    _temp|=(uint8_t)data.at(4);
    midlineEdit->setText(QString::number(_temp));
    _temp=(uint8_t)data.at(5)<<8;
    _temp|=(uint8_t)data.at(6);
    higlineEdit->setText(QString::number(_temp));
}

void NavigationSpeedDlog::readDataSlot()
{
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_AUTOSPEED);
    package->packMsg(&msg);
}

void NavigationSpeedDlog::setDataSlot()
{
    int temp=lowlineEdit->text().toInt();
    if((temp>=65535)|(temp<=0)){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                       "<p><font color=blue>低速</font>"
                       "<p><font color=blue>参数必须在0~65535度之间.</font>");
        dialog.exec();
        return;
    }

    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_AUTOSPEED);
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));

    temp=midlineEdit->text().toInt();
    if((temp>=65535)|(temp<=0)){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                       "<p><font color=blue>中速</font>"
                       "<p><font color=blue>参数必须在0~65535度之间.</font>");
        dialog.exec();
        return;
    }
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));

    temp=higlineEdit->text().toInt();
    if((temp>=65535)|(temp<=0)){
        HintDialog dialog(this);
        dialog.setText("<h2><font color=red>警告</font></h2>"
                       "<p><font color=blue>高速</font>"
                       "<p><font color=blue>参数必须在0~65535度之间.</font>");
        dialog.exec();
        return;
    }
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));
    package->packMsg(&msg);
}

void NavigationSpeedDlog::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<p><font color=blue>每次设置请先读取数值.</font>"
                   "<p><font color=blue>确认之后在进行设置.</font>"
                   "<p><font color=blue>参数必须在0~65535度之间.</font>");
    dialog.exec();
}

bool NavigationSpeedDlog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==lowlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             lowlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             lowlineEdit->setPalette(p);
        }
    }

    if (watched==midlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             midlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             midlineEdit->setPalette(p);
        }
    }

    if (watched==higlineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             higlineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             higlineEdit->setPalette(p);
        }
    }
    return QWidget::eventFilter(watched,event);
}
