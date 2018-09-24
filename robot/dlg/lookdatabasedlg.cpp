#include "lookdatabasedlg.h"
#include "ui_lookdatabasedlg.h"
#include "dlg/hintdialog.h"
#include "dlg/editdatabasedlg.h"
#include "db/databasehelper.h"
#include "info/routeinfo.h"
#include "utils/utils.h"
#include <QtWidgets>

LookDatabaseDlg::LookDatabaseDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LookDatabaseDlg)
{
    ui->setupUi(this);
    dao=DatabaseHelper::instance();
    setWindowTitle("查看数据库");
    Utils::formInCenter(this,1);
    connectAction();
    clear();
    addTableNames();
}

LookDatabaseDlg::~LookDatabaseDlg()
{
    delete ui;
}

void LookDatabaseDlg::connectAction()
{
    QObject::connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(openEditDatabaseDlg()));
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(noticeSlot()));
    connect(ui->comboBox,SIGNAL(currentTextChanged(const QString &)),this,SLOT(addDatas(const QString &)));
    connect(ui->tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(openEditDatabaseDlg(int,int)));
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deleteData()));
    connect(ui->deleteTableButton,SIGNAL(clicked()),this,SLOT(deleteTable()));
}

void LookDatabaseDlg::openEditDatabaseDlg()
{
    openEditDatabaseDlg(ui->tableWidget->currentRow() ,0);
}

void LookDatabaseDlg::openEditDatabaseDlg(int row, int column)
{
    Q_UNUSED(column);
    RouteInfo info;
    if(QString::compare(dao->getAdminSqlTableName(),tableName)!=0){
        info.lat=ui->tableWidget->item(row,0)->text().toDouble();
        info.log=ui->tableWidget->item(row,1)->text().toDouble();
        EditDatabaseDlg dialog(this);
        connect(this,SIGNAL(editDataSignal(unsigned ,const RouteInfo &)),&dialog,SLOT(receiveDataSlot(unsigned,const RouteInfo &)));
        emit editDataSignal(row+1,info);
        if(dialog.exec()){
            RouteInfo info1=dialog.getEditData();
            if(info1!=info){
               QMessageBox msg(QMessageBox::Warning,"系统提示",
                                "<p><font color=blue>数据已被修改,是否保存?</font>");
               msg.addButton("取消",QMessageBox::AcceptRole);
               QPushButton *okbutton = (msg.addButton("确定",QMessageBox::AcceptRole));
               msg.exec();
               if(msg.clickedButton()==okbutton){
                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(info1.lat,'f',8));
                    ui->tableWidget->setItem(row,0,item);
                    QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(info1.log,'f',8));
                    ui->tableWidget->setItem(row,1,item0);
                    dao->updateSqlDatabase(tableName,row+1,info1);
               }
            }
        }
    }
}

void LookDatabaseDlg::addTableNames()
{
    ui->comboBox->clear();
    QStringList list =dao->getSqlTableNames();
    for(int i=0;i<list.length();i++)
        ui->comboBox->addItem(list.at(i));
}

void LookDatabaseDlg::addDatas(const QString &name)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    tableName=name;
    ui->tableWidget->setRowCount(dao->getSqlCount(name));
    QList<RouteInfo> list=dao->queryAllSqlDatabase(name);
    for(int i=0;i<list.size();i++){
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(list[i].lat,'f',8));
        ui->tableWidget->setItem(i,0,item);
        QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(list[i].log,'f',8));
        ui->tableWidget->setItem(i,1,item0);
    }

    if(QString::compare(dao->getAdminSqlTableName(),tableName)!=0){
        bool flag=list.size()>0;
        ui->deleteButton->setEnabled(flag);
        ui->deleteTableButton->setEnabled(flag);
        ui->editButton->setEnabled(flag);
    }
    QApplication::restoreOverrideCursor();
}

void LookDatabaseDlg::clear()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    ui->tableWidget->setRowCount(RowCount);
    ui->tableWidget->setColumnCount(ColumnCount);

    QStringList header;
    header<<"纬度"<<"经度"<<"备注";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidget->setColumnWidth(0, 125);
    ui->tableWidget->setColumnWidth(1, 125);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode (QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );
    ui->tableWidget->setCurrentCell(0, 0);

    ui->deleteButton->setEnabled(false);
    ui->deleteTableButton->setEnabled(false);
    ui->editButton->setEnabled(false);
}

void LookDatabaseDlg::deleteData()
{
    QMessageBox msg(QMessageBox::Warning,"系统提示",
                     "<p><font color=blue>您确定删除指定数据吗?</font>");
    msg.addButton("取消",QMessageBox::AcceptRole);
    QPushButton *okbutton = (msg.addButton("确定",QMessageBox::AcceptRole));
    msg.exec();
    if(msg.clickedButton()==okbutton){
        dao->deleteSqlDatabase(tableName,ui->tableWidget->currentRow()+1);
    }
}

void LookDatabaseDlg::deleteTable()
{
    QMessageBox msg(QMessageBox::Warning,"系统提示",
                     "<p><font color=blue>您确定删除指定数据表吗?</font>");
    msg.addButton("取消",QMessageBox::AcceptRole);
    QPushButton *okbutton = (msg.addButton("确定",QMessageBox::AcceptRole));
    msg.exec();
    if(msg.clickedButton()==okbutton){
        dao->clearSqlTable(tableName);
        addTableNames();
    }
}

void LookDatabaseDlg::noticeSlot()
{
    HintDialog dialog(this);
    dialog.setText("<p><font color=blue>查看存储采集坐标的数据库.</font>"
                   "<p><font color=blue>可删除选定的数据库.</font>"
                   "<p><font color=blue>请慎重操作.</font>");
    dialog.exec();
}
