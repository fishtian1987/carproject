#ifndef LOOKDATABASEDLG_H
#define LOOKDATABASEDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class DatabaseHelper;
class RouteInfo;
QT_END_NAMESPACE

namespace Ui {
class LookDatabaseDlg;
}

class LookDatabaseDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LookDatabaseDlg(QWidget *parent = 0);
    ~LookDatabaseDlg();
signals:
    void editDataSignal(unsigned ,const RouteInfo &);
private slots:
    void addDatas(const QString &);
    void openEditDatabaseDlg(int,int);
    void openEditDatabaseDlg();
    void deleteData();
    void deleteTable();
    void noticeSlot();
private:
    void connectAction();
    void clear();
    void addTableNames();
private:
    enum {RowCount = 10, ColumnCount = 3 };
    Ui::LookDatabaseDlg *ui;
    DatabaseHelper *dao;
    QString tableName;
};

#endif // LOOKDATABASEDLG_H
