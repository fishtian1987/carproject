#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QtSql>
#include <QList>
#include "info/routeinfo.h"
#include "info/edges.h"
#include "info/vertexweight.h"

class DatabaseHelper
{
protected:
    DatabaseHelper();
public:
    ~DatabaseHelper();

public:
    static DatabaseHelper* instance();
    bool openSqlDatabase();
    void createSqlTable(QString);
    void clearSqlTable(const QString &);
    unsigned short queryVertexNumSqlDatabase();
    void setQueryTableNames(const QStringList &);
    QString getAdminSqlTableName();
    QStringList getSqlTableNames();
    unsigned short getSqlCount(const QString &);
    QString errorLog();
    void insertSqlDatabase(double,double);
    void updateSqlDatabase(const QString &,unsigned,RouteInfo &);
    void deleteSqlDatabase(const QString &,unsigned);
    QStringList queryOneStringSqlDatabase(const QString &,const QString &);
    QList<VertexWeight> queryVertexWeightSqlDatabase();
    Edges queryEdgeSqlDatabase(const QString &);
    QList<RouteInfo> queryAllSqlDatabase(const QString &);
    QList<RouteInfo> querySqlDatabase(unsigned short );

private:
    QList<RouteInfo> querySqlDatabaseSub(unsigned short num);
    void transSqlDatabaseExteral(QList<RouteInfo> &,unsigned short);
private:
    class CGarbo
    {
    public:
        ~CGarbo ()
        {
           if(DatabaseHelper::dbdao != NULL)
           {
               delete DatabaseHelper::dbdao;
               DatabaseHelper::dbdao = NULL;
           }
        }
    };

private:
    static DatabaseHelper *dbdao;
    static CGarbo  cGarbo ;
    QSqlDatabase db;
    unsigned short indexRoutesData;
    QList<Edges> tableList;
    unsigned short tableListIndex;
    unsigned short recordCount;
    QString tableInfoName;
    QString tableName1;
};

#endif // DATABASEHELPER_H
