#include "locationdriftdlg.h"
#include "com/mylineedit.h"
#include "db/package.h"
#include "dlg/hintdialog.h"
#include "info/atk.h"
#include "info/message.h"
#include "utils/utils.h"
#include <QtWidgets>

LocationDriftDlg::LocationDriftDlg(QWidget *parent) :
    QDialog(parent)
{
    keyPressFlag=false;
    QLabel *label0=new QLabel("阈值:");
    QLabel *label1=new QLabel("米");
    lineEdit=new MyLineEdit;
    label0->setBuddy(lineEdit);

    QHBoxLayout *HLayout1 = new QHBoxLayout;
    HLayout1->addWidget(label0);
    HLayout1->addWidget(lineEdit);
    HLayout1->addWidget(label1);
    HLayout1->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    readButton=new QPushButton("读取");
    setButton=new QPushButton("设置");
    closeButton=new QPushButton("关闭");
    QHBoxLayout *HLayout2 = new QHBoxLayout;
    HLayout2->addWidget(readButton);
    HLayout2->addWidget(setButton);
    HLayout2->addWidget(closeButton);

    toolButton=new QToolButton;
    toolButton->setIcon(QIcon(":/images/help.png"));
    toolButton->setIconSize(QSize(16,16));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolButton);
    mainLayout->addLayout(HLayout1);
    mainLayout->addSpacerItem(new QSpacerItem(20, 30, QSizePolicy::Expanding));
    mainLayout->addLayout(HLayout2);

    QGroupBox *groupBox=new QGroupBox("位置阈值");
    groupBox->setLayout(mainLayout);
    groupBox->title();
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(groupBox);
    setLayout(grid);
    setWindowTitle("位置阈值");
    setFixedHeight(sizeHint().height());
    Utils::formInCenter(this,0.75);
    connectAction();
    package=new Package;
    lineEdit->installEventFilter(this);
    lineEdit->setText("0.0");
}

void LocationDriftDlg::connectAction()
{
    QObject::connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(readButton,SIGNAL(clicked()),this,SLOT(readDataSlot()));
    connect(setButton,SIGNAL(clicked()),this,SLOT(setDataSlot()));
    connect(toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
}

LocationDriftDlg::~LocationDriftDlg()
{
    delete package;
    delete lineEdit;
    delete readButton;
    delete setButton;
    delete closeButton;
    delete toolButton;
}

void LocationDriftDlg::readDataSlot()
{
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_LOCAVPT);
    package->packMsg(&msg);
}

void LocationDriftDlg::setDataSlot()
{
    unsigned short temp=lineEdit->text().toFloat()*10;
    if(temp>600){
        HintDialog dialog(this);
        dialog.setText("<p><font color=blue>参数不能大于60.</font>");
        dialog.exec();
        return;
    }
    Message msg;
    msg.msgID=Atk::D_CMAINCONTROL;
    msg.data.append(Atk::C_LOCAVPT);
    msg.data.append(BYTE1(temp));
    msg.data.append(BYTE0(temp));
    package->packMsg(&msg);
}

void LocationDriftDlg::analysisDataSlot(QByteArray &data)
{
    unsigned short _temp=0;
    _temp=(uint8_t)data.at(1)<<8;
    _temp|=(uint8_t)data.at(2);
    lineEdit->setText(QString::number(_temp/10.0,'f', 1));
}

void LocationDriftDlg::keyPressEvent(QKeyEvent *event)
{
    if(keyPressFlag){
        switch(event->key()){
        case Qt::Key_Up:{
            readDataSlot();
        }break;
        case Qt::Key_Down:{
            setDataSlot();
        }break;
        case Qt::Key_Delete:{
            close();
        }break;
        default:break;
        }
        keyPressFlag=false;
    }else{
     keyPressFlag=true;
    }
}

void LocationDriftDlg::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<p><font color=blue>每次设置请先读取数值.</font>"
                   "<p><font color=blue>确认之后在进行设置.</font>"
                   "<p><font color=blue>参数不能大于60.</font>");
    dialog.exec();
}

bool LocationDriftDlg::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==lineEdit){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             lineEdit->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             lineEdit->setPalette(p);
        }
    }
    return QWidget::eventFilter(watched,event);
}
