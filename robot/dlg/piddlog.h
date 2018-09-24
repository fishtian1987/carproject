#ifndef PIDDLOG_H
#define PIDDLOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class MyLineEdit;
class QPushButton;
class QToolButton;
class Package;
QT_END_NAMESPACE

class PIDDlog : public QDialog
{
    Q_OBJECT

public:
    explicit PIDDlog(QWidget *parent = 0);
    ~PIDDlog();
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
    MyLineEdit *SPlineEdit;
    MyLineEdit *SIlineEdit;
    MyLineEdit *SDlineEdit;
    MyLineEdit *RPlineEdit;
    MyLineEdit *RIlineEdit;
    MyLineEdit *RDlineEdit;
    QPushButton *readButton;
    QPushButton *setButton;
    QPushButton *closeButton;
    QToolButton *toolButton;
    Package *package;
};

#endif // PIDDLOG_H
