#ifndef AUTONAVIGATIONDLG_H
#define AUTONAVIGATIONDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class MyLineEdit;
class QLabel;
class QRadioButton;
class QPushButton;
class QToolButton;
class DatabaseHelper;
QT_END_NAMESPACE

class AutoNavigationDlg : public QDialog
{
    Q_OBJECT
public:
    explicit AutoNavigationDlg(QWidget *parent = 0);
    ~AutoNavigationDlg();
    bool getControlMode();
private slots:
    void setMOdeSlot();
    void noticeSlot();
    void planSlot();
private:
    QStringList autoSolveRoute( int, int);
private:
    MyLineEdit *lineEdit;
    QLabel *label;
    QLabel *descLabel;
    QRadioButton *manualRadioButton;
    QRadioButton *autoRadioButton;
    QPushButton *closeButton;
    QPushButton *planButton;
    QToolButton *toolButton;
    DatabaseHelper *dao;
    bool controlMode;
};

#endif // AUTONAVIGATIONDLG_H
