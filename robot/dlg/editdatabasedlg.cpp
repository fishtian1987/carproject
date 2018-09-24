#include "editdatabasedlg.h"
#include "dlg/hintdialog.h"
#include "com/mylineedit.h"
#include "info/routeinfo.h"
#include "utils/utils.h"
#include <QtWidgets>

EditDatabaseDlg::EditDatabaseDlg(QWidget *parent):
    QDialog(parent)
{
    routeInfo=new RouteInfo;
    QLabel *label0=new QLabel(" ID:   ");
    id=new MyLineEdit;
    id->setFocusPolicy(Qt::NoFocus);
    label0->setBuddy(id);
    QHBoxLayout *HLayout0 = new QHBoxLayout;
    HLayout0->addWidget(label0);
    HLayout0->addWidget(id);

    QLabel *label1=new QLabel("纬度:");
    lat=new MyLineEdit;
    label1->setBuddy(lat);
    QHBoxLayout *HLayout1 = new QHBoxLayout;
    HLayout1->addWidget(label1);
    HLayout1->addWidget(lat);

    QLabel *label2=new QLabel("经度:");
    log=new MyLineEdit;
    label2->setBuddy(log);
    QHBoxLayout *HLayout2 = new QHBoxLayout;
    HLayout2->addWidget(label2);
    HLayout2->addWidget(log);

    toolButton=new QToolButton;
    toolButton->setIcon(QIcon(":/images/help.png"));
    toolButton->setIconSize(QSize(16,16));

    buttonBox=new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel|QDialogButtonBox::Close);
    buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    buttonBox->button(QDialogButtonBox::Close)->setText("关闭");
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolButton);
    mainLayout->addLayout(HLayout0);
    mainLayout->addLayout(HLayout1);
    mainLayout->addLayout(HLayout2);
    mainLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    mainLayout->addWidget(buttonBox);

    QGroupBox *groupBox=new QGroupBox("记录");
    groupBox->setLayout(mainLayout);
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(groupBox);
    setLayout(grid);
    setWindowTitle("记录");
    setFixedHeight(sizeHint().height());

    Utils::formInCenter(this,0.75);
    connectAction();
    lat->installEventFilter(this);
    log->installEventFilter(this);
}

EditDatabaseDlg::~EditDatabaseDlg()
{
    delete routeInfo;
    delete id;
    delete lat;
    delete log;
    delete toolButton;
    delete buttonBox;
}

void EditDatabaseDlg::connectAction()
{
    QObject::connect(buttonBox, SIGNAL(accepted()),this, SLOT(accept()));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),this, SLOT(clickedlSlot(QAbstractButton*)));
    connect(toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
}

void EditDatabaseDlg::clickedlSlot(QAbstractButton *button)
{
    if(buttonBox->button(QDialogButtonBox::Cancel) == button){
        lat->setText(QString::number(routeInfo->lat,'f',8));
        log->setText(QString::number(routeInfo->log,'f',8));
    }
    if(buttonBox->button(QDialogButtonBox::Close) == button)
        close();
}

RouteInfo EditDatabaseDlg::getEditData()
{
    RouteInfo info;
    info.lat=lat->text().toDouble();
    info.log=log->text().toDouble();
    return info;
}

void EditDatabaseDlg::receiveDataSlot(unsigned i,const RouteInfo &data)
{
   id->setText(QString::number(i));
   lat->setText(QString::number(data.lat,'f',8));
   log->setText(QString::number(data.log,'f',8));
   routeInfo->lat=data.lat;
   routeInfo->log=data.log;
}

void EditDatabaseDlg::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<p><font color=blue>确定:修改的坐标生效</font>"
                   "<p><font color=blue>取消:回复修改前的坐标值</font>"
                   "<p><font color=blue>关闭:关闭对话框</font>");
    dialog.exec();
}

bool EditDatabaseDlg::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==lat){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             lat->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             lat->setPalette(p);
        }
    }

    if (watched== log){
        if (event->type()==QEvent::FocusIn){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::cyan);
             log->setPalette(p);
        }else if (event->type()==QEvent::FocusOut){
             QPalette p=QPalette();
             p.setColor(QPalette::Base,Qt::white);
             log->setPalette(p);
        }
    }
    return QWidget::eventFilter(watched,event);
}
