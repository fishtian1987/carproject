#ifndef DETECTIONSPEEDDLG_H
#define DETECTIONSPEEDDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class Package;
QT_END_NAMESPACE

namespace Ui {
class DetectionSpeedDlg;
}

class DetectionSpeedDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DetectionSpeedDlg(QWidget *parent = 0);
    ~DetectionSpeedDlg();
public slots:
    void analysisDataSlot(QByteArray &);
private slots:
    void startDetecSpeedSlot();
    void stopDetecSpeedSlot();
    void noticeSlot();
    void timerEvent();
private:
    Ui::DetectionSpeedDlg *ui;
    Package *package;
    QTimer *timer;
};

#endif // DETECTIONSPEEDDLG_H
