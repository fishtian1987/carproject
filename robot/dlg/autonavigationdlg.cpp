#include "autonavigationdlg.h"
#include "hintdialog.h"
#include "utils/utils.h"
#include "com/mylineedit.h"
#include "db/databasehelper.h"
#include "dlg/hintdialog.h"
#include "info/atk.h"
#include <QtWidgets>

AutoNavigationDlg::AutoNavigationDlg(QWidget *parent) :
    QDialog(parent)
{
    dao=DatabaseHelper::instance();
    controlMode=false;
    descLabel=new QLabel;
    QHBoxLayout *HLayout0= new QHBoxLayout;
    HLayout0->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    HLayout0->addWidget(descLabel);
    HLayout0->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    lineEdit=new MyLineEdit;
    label=new QLabel("路径");
    label->setBuddy(lineEdit);
    QHBoxLayout *HLayout1 = new QHBoxLayout;
    HLayout1->addWidget(label);
    HLayout1->addWidget(lineEdit);

    manualRadioButton=new QRadioButton("手动模式");
    autoRadioButton=new QRadioButton("自动模式");
    manualRadioButton->setChecked(true);
    QHBoxLayout *HLayout2 = new QHBoxLayout;
    HLayout2->addWidget(manualRadioButton);
    HLayout2->addSpacerItem(new QSpacerItem(30, 10, QSizePolicy::Expanding));
    HLayout2->addWidget(autoRadioButton);

    planButton=new QPushButton("规划");
    closeButton=new QPushButton("关闭");
    QHBoxLayout *HLayout3 = new QHBoxLayout;
    HLayout3->addWidget(planButton);
    HLayout3->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    HLayout3->addWidget(closeButton);

    toolButton=new QToolButton;
    toolButton->setIcon(QIcon(":/images/help.png"));
    toolButton->setIconSize(QSize(16,16));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolButton);
    mainLayout->addLayout(HLayout0);
    mainLayout->addLayout(HLayout1);
    mainLayout->addLayout(HLayout2);
    mainLayout->addLayout(HLayout3);

    QGroupBox *groupBox=new QGroupBox("导航模式");
    groupBox->setLayout(mainLayout);
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(groupBox);
    setLayout(grid);

    Utils::formInCenter(this,0.75);
    setFixedHeight(sizeHint().height());
    setMOdeSlot();
    setWindowTitle("导航模式");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(manualRadioButton,SIGNAL(toggled(bool)),this,SLOT(setMOdeSlot()));
    connect(planButton,SIGNAL(clicked()),this,SLOT(planSlot()));
    connect(toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
}

AutoNavigationDlg::~AutoNavigationDlg()
{
    delete lineEdit;
    delete label;
    delete descLabel;
    delete manualRadioButton;
    delete autoRadioButton;
    delete closeButton;
    delete planButton;
    delete toolButton;
}

QStringList AutoNavigationDlg::autoSolveRoute( int sd, int ed)
{
    QStringList strlist;
    int i=0,j=0;
    unsigned short n=dao->queryVertexNumSqlDatabase();
    unsigned short ** graph=new unsigned short *[n];
    for(i=0;i<n;i++)
        graph[i]=new unsigned short[n];

    for(i=0;i<n;i++)
        for(j=0;j<n;j++)
            if(i==j)graph[i][j]=0;
            else graph[i][j]=Atk::INF;

    QList<VertexWeight> list=dao->queryVertexWeightSqlDatabase();
    for(i=0;i<list.size();i++){
        graph[ list.at(i).fistVertex][list.at(i).secondVertex]=list.at(i).weight;
        graph[list.at(i).secondVertex][list.at(i).fistVertex]=list.at(i).weight;
    }

    strlist=Utils::solveRoute(graph,n,sd,ed);
    for(i=0;i<n;i++)
    {
        delete [] graph[i];
        graph[i]=NULL;
    }
    delete [] graph;
    graph=NULL;

    return strlist;
}

void AutoNavigationDlg::planSlot()
{
    unsigned short i;
    QStringList list;
    if(!lineEdit->text().isEmpty()){
        QStringList list0 = lineEdit->text().split(',', QString::SkipEmptyParts);
        if(manualRadioButton->isChecked()){
            for(i=0;i<list0.size()-1;i++)
                list.append(list0.at(i)+"_"+list0.at(i+1));
            QStringList listName =dao->queryOneStringSqlDatabase("name","edge");
            for(i=0;i<list.size();i++){
                if(!listName.contains(list.at(i))){
                    controlMode=false;
                    HintDialog dialog(this);
                    dialog.setText("<h2><font color=red>警告</font></h2>"
                                   "<p><font color=blue>不存在规划路段"+list.at(i)+"请检查后重新输入。</font>");
                    dialog.exec();
                    return;
                }
            }
            listName.clear();
        }else{
            QStringList listVertex=dao->queryOneStringSqlDatabase("id","vertex");
            for(i=0;i<list0.size();i++){
                if(!listVertex.contains(list0.at(i))){
                    controlMode=false;
                    HintDialog dialog(this);
                    dialog.setText("<h2><font color=red>警告</font></h2>"
                                   "<p><font color=blue>不存在规划路点"+list0.at(i)+"请检查后重新输入。</font>");
                    dialog.exec();
                    return;
                }
            }
            listVertex.clear();
            list0=autoSolveRoute(list0.at(0).toInt(),list0.at(1).toInt());

            QString str;
            for(i=1;i<list0.size();i++)
                str+=" "+list0.at(i);

            HintDialog dialog(this);
            dialog.setText("<h2><font color=red>规划路经</font></h2>"
                           "<p><font color=blue>距离</font>"
                           "<p><font color=blue>"+list0.at(0)+"</font>"
                           "<p><font color=blue>路经</font>"
                           "<p><font color=blue>"+str+"</font>");
            dialog.exec();

            for(i=1;i<list0.size()-1;i++)
                list.append(list0.at(i)+"_"+list0.at(i+1));
            for(i=0;i<list.size();i++)
                qDebug()<<list.at(i);
        }
        dao->setQueryTableNames(list);
    }
    else
        controlMode=false;
}

void AutoNavigationDlg::setMOdeSlot()
{
    if(manualRadioButton->isChecked())
        descLabel->setText("<p><font color=blue>手动模式</font><p>");
    else
        descLabel->setText("<p><font color=blue>自动模式</font><p>");
    controlMode=true;
}

void AutoNavigationDlg::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<h2><font color=red>手动模式</font></h2>"
                   "<p><font color=blue>手动输入路径点.</font>"
                   "<h2><font color=red>自动模式</font></h2>"
                   "<p><font color=blue>手动输入要经过点及终点.</font>");
    dialog.exec();
}

bool AutoNavigationDlg::getControlMode()
{

    return controlMode;
}
