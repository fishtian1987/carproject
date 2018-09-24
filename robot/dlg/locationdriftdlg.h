#ifndef LOCATIONDRIFTDLG_H
#define LOCATIONDRIFTDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class MyLineEdit;
class QPushButton;
class QToolButton;
class Package;
QT_END_NAMESPACE

class LocationDriftDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LocationDriftDlg(QWidget *parent = 0);
    ~LocationDriftDlg();
protected:
    void keyPressEvent(QKeyEvent *);
public slots:
    void analysisDataSlot(QByteArray &);
private slots:
    bool eventFilter(QObject *,QEvent *);
    void readDataSlot();
    void setDataSlot();
    void noticeSlot();
private:
    void connectAction();
private:
    bool keyPressFlag;
    MyLineEdit *lineEdit;
    QPushButton *readButton;
    QPushButton *setButton;
    QPushButton *closeButton;
    QToolButton *toolButton;
    Package *package;
};

#endif // LOCATIONDRIFTDLG_H
