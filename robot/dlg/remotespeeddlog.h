#ifndef REMOTESPEEDDLOG_H
#define REMOTESPEEDDLOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class MyLineEdit;
class QPushButton;
class QToolButton;
class Package;
QT_END_NAMESPACE

class RemoteSpeedDlog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoteSpeedDlog(QWidget *parent = 0);
    ~RemoteSpeedDlog();
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
    MyLineEdit *FBlineEdit;
    MyLineEdit *RlineEdit;
    MyLineEdit *LRlineEdit;
    MyLineEdit *LMlineEdit;
    QPushButton *readButton;
    QPushButton *setButton;
    QPushButton *closeButton;
    QToolButton *toolButton;
    Package *package;
};

#endif // REMOTESPEEDDLOG_H
