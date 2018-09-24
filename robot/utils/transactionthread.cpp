#include "transactionthread.h"

Transaction * const EndTransaction = 0;

queryDBTransaction::queryDBTransaction(unsigned short  num)
{
    this->num=num;
}

QList<RouteInfo> queryDBTransaction::apply(DatabaseHelper *dbdao)
{
     return dbdao->querySqlDatabase(num);
}

TransactionThread::TransactionThread()
{
    start();
}

TransactionThread::~TransactionThread()
{
    {
        QMutexLocker locker(&mutex);
        while (!transactions.isEmpty())
            delete transactions.dequeue();
        transactions.enqueue(EndTransaction);
        transactionAdded.wakeOne();
    }
    wait();
}

void TransactionThread::addTransaction(Transaction *transact)
{
    QMutexLocker locker(&mutex);
    transactions.enqueue(transact);
    transactionAdded.wakeOne();
}


void TransactionThread::run()
{
    Transaction *transact = 0;
    forever {
        {
            QMutexLocker locker(&mutex);

            if (transactions.isEmpty())
                transactionAdded.wait(&mutex);
            transact = transactions.dequeue();
            if (transact == EndTransaction)
                break;
        }
        currentData=transact->apply(dbdao);
        delete transact;
        {
            QMutexLocker locker(&mutex);
            if (transactions.isEmpty())
                emit allTransactionsDone();
        }
    }
}

void TransactionThread::setDatabaseHelp(DatabaseHelper *db)
{
    QMutexLocker locker(&mutex);
    dbdao=db;
}

QList<RouteInfo> TransactionThread::routeData()
{
    QMutexLocker locker(&mutex);
    return currentData;
}
