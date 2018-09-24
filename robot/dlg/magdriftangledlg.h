#ifndef MAGDRIFTANGLEDLG_H
#define MAGDRIFTANGLEDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class MyLineEdit;
class QPushButton;
class QToolButton;
class Package;
QT_END_NAMESPACE

class MagDriftAngleDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MagDriftAngleDlg(QWidget *parent = 0);
    ~MagDriftAngleDlg();
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

#endif // MAGDRIFTANGLEDLG_H
