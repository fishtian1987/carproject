#ifndef COLLECTCOORDINATEDLOG_H
#define COLLECTCOORDINATEDLOG_H

#include "info/routeinfo.h"
#include <QDialog>
#include <QTimer>

QT_BEGIN_NAMESPACE
class MyLineEdit;
class QLabel;
class MySpinBox;
class QCheckBox;
class QPushButton;
class QToolButton;
class Package;
class DatabaseHelper;
class RouteInfo;
QT_END_NAMESPACE

class CollectCoordinateDlog : public QDialog
{
    Q_OBJECT

public:
    explicit CollectCoordinateDlog(QWidget *parent = 0);
    ~CollectCoordinateDlog();
public slots:
    void analysisDataSlot(QByteArray &);
private slots:
    bool eventFilter(QObject *,QEvent *);
    void startCollectionsSlot();
    void stopCollectionsSlot();
    void noticeSlot();
    void timerEvent();
private:
    void compoundCoordinate(QByteArray &);
    void connectAction();
    float getDistance();
    void showInfo();
private:
    MyLineEdit *fileLineEdit;
    QLabel *oldLabel;
    QLabel *newLabel;
    QLabel *diffLabel;
    QLabel *countLabel;
    MySpinBox *intervaSpinBox;
    QCheckBox *checkBox;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *closeButton;
    QToolButton *toolButton;
    Package *package;
    QTimer *timer;
    DatabaseHelper *dao;
    RouteInfo oldinfo;
    RouteInfo info;
    unsigned count;
};

#endif // COLLECTCOORDINATEDLOG_H
