#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "utils/transactionthread.h"

QT_BEGIN_NAMESPACE
class DataProtocol;
class Package;
class Message;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initialize();
private slots:
    void openMagDriftAngleDlg();
    void openLocationDriftDlg();
    void openPIDDlg();
    void openRemoteSpeedDlg();
    void openNavigationSpeedDlg();
    void openCollectCoordinateDlg();
    void openAutoNavigationDlg();
    void openLookDatabaseDlg();
    void openDetectionSpeedDlg();
    void allTransactionsDone();
    void allPreprocessorDone(QByteArray &); 
    void test();
signals:
    void collectCoordinateSignal(QByteArray &);
    void magdriftangleSignal(QByteArray &);
    void locationDriftSignal(QByteArray &);
    void pidSignal(QByteArray &);
    void remoteSpeedSignal(QByteArray &);
    void navigationSpeedSignal(QByteArray &);
    void detectionSpeedSignal(QByteArray &);
private:
    void openDatabase();
    void openUsart();
    void addTransaction(Transaction *);
    void analysisMainData(QByteArray &);
    //void analysisSubData(QByteArray &);
    void analysis4GData(QByteArray &);
    void analysisGPSData(QByteArray &data);
    void senRoutesData();
    void SetSendDataState(uint8_t state);
    uint8_t GetSendDataState();
private:
    Ui::MainWindow *ui;
    uint8_t sendDataState;
    TransactionThread thread;
    DataProtocol *dataPro;
    Package *packet;
};

#endif // MAINWINDOW_H
