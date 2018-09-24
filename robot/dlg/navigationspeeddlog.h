#ifndef NAVIGATIONSPEEDDLOG_H
#define NAVIGATIONSPEEDDLOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class MyLineEdit;
class QPushButton;
class QToolButton;
class Package;
QT_END_NAMESPACE

class NavigationSpeedDlog : public QDialog
{
    Q_OBJECT
public:
    explicit NavigationSpeedDlog(QWidget *parent = 0);
    ~NavigationSpeedDlog();
private:
    void connectAction();
public slots:
    void analysisDataSlot(QByteArray &);
private slots:
    bool eventFilter(QObject *,QEvent *);
    void readDataSlot();
    void setDataSlot();
    void noticeSlot();
private:
    MyLineEdit *lowlineEdit;
    MyLineEdit *midlineEdit;
    MyLineEdit *higlineEdit;
    QPushButton *readButton;
    QPushButton *setButton;
    QPushButton *closeButton;
    QToolButton *toolButton;
    Package *package;
};

#endif // NAVIGATIONSPEEDDLOG_H
