#ifndef TRANSACTIONTHREAD_H
#define TRANSACTIONTHREAD_H

#include "db/databasehelper.h"
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>

class Transaction
{
public:
    virtual ~Transaction() { }
    virtual QList<RouteInfo> apply(DatabaseHelper *) = 0;
};

class queryDBTransaction : public Transaction
{
public:
    queryDBTransaction(unsigned short);
    QList<RouteInfo> apply(DatabaseHelper *);
private:
    unsigned short  num;
};

class TransactionThread : public QThread
{
    Q_OBJECT
public:
    TransactionThread();
    ~TransactionThread();
    void addTransaction(Transaction *transact);
    void setDatabaseHelp(DatabaseHelper *db);
    QList<RouteInfo> routeData();
signals:
    void allTransactionsDone();
protected:
    void run();
private:
    QList<RouteInfo> currentData;
    DatabaseHelper *dbdao=NULL;
    QQueue<Transaction *> transactions;
    QWaitCondition transactionAdded;
    QMutex mutex;
};

#endif // TRANSACTIONTHREAD_H
