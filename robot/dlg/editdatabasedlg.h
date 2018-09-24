#ifndef EDITDATABASEDLG_H
#define EDITDATABASEDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class MyLineEdit;
class QToolButton;
class RouteInfo;
class DatabaseHelper;
class RouteInfo;
class QDialogButtonBox;
class QAbstractButton;
QT_END_NAMESPACE

class EditDatabaseDlg: public QDialog
{
    Q_OBJECT
public:
    explicit EditDatabaseDlg(QWidget *parent = 0);
    ~EditDatabaseDlg();
    RouteInfo getEditData();
public slots:
    void receiveDataSlot(unsigned ,const RouteInfo &);
private slots:
    bool eventFilter(QObject *,QEvent *);
    void noticeSlot();
    void clickedlSlot(QAbstractButton*);
private:
    void connectAction();
private:
    RouteInfo *routeInfo;
    MyLineEdit *id;
    MyLineEdit *lat;
    MyLineEdit *log;
    QToolButton *toolButton;
    QDialogButtonBox *buttonBox;
};

#endif // EDITDATABASEDLG_H
