#include "databasehelper.h"
#include "stddef.h"

DatabaseHelper::DatabaseHelper():
    indexRoutesData(1),
    tableListIndex(0),
    recordCount(0)
{
    tableInfoName="info";
}

DatabaseHelper::~DatabaseHelper()
{
    if(db.isOpen())
        db.close();
}

DatabaseHelper::CGarbo  DatabaseHelper::cGarbo ;
DatabaseHelper* DatabaseHelper::dbdao=new DatabaseHelper;

DatabaseHelper* DatabaseHelper::instance()
{
    return dbdao;
}

//获取指定表的条数
unsigned short DatabaseHelper::getSqlCount(const QString &tablename)
{
    int count=0;
    QSqlQuery query;
    if(!db.isOpen())
        db.open();
    query.exec("select count (id) from "+tablename);
    while(query.next())
    {
        count =query.value(0).toInt();
    }
    query.clear();
    db.close();
    return count;
}

//获取主表名
QString DatabaseHelper::getAdminSqlTableName()
{
    return "tableEdgeName";
}

//获取数据库表
QStringList DatabaseHelper::getSqlTableNames()
{
    QStringList names;
    if(!db.isOpen())
        db.open();
    names=db.tables(QSql::Tables);
    if(names.contains("sqlite_sequence"))
        names.removeOne("sqlite_sequence");
    db.close();
    return names;
}

//删表
void DatabaseHelper::clearSqlTable(const QString &name)
{
    QSqlQuery query;

    if(!db.isOpen())
        db.open();
    query.exec("drop table if exists "+name);
    query.clear();
    db.close();
}

//打开数据库
bool DatabaseHelper::openSqlDatabase()
{
    if(QSqlDatabase::contains("qt_sql_default_connection")){
         db = QSqlDatabase::database("qt_sql_default_connection");
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("/home/pi/build/navrobot.db");
        return db.open();
    }
    return true;
}

//建表
void DatabaseHelper::createSqlTable(QString name)
{
    QSqlQuery query;
    if(!db.isOpen())
        db.open();

    if(!name.isEmpty()){
        query.exec("create table if not exists "+name+"(_id integer primary key autoincrement,"
                   " lat DOUBLE,log DOUBLE);");
        tableName1=name;
    }
    query.clear();
    db.close();
}

//错误日志
QString DatabaseHelper::errorLog()
{
    return this->db.lastError().text();
}

//查询vertex数
unsigned short DatabaseHelper::queryVertexNumSqlDatabase()
{
    unsigned short num;
    QSqlQuery query;
    QString str="select vertex from "+tableInfoName;

    if(!db.isOpen())
        db.open();

    query.exec(str);
    while(query.next())
    {
        num=query.value(0).toInt();
    }
    query.clear();
    db.close();
    return num;
}

//查询长度和起止点
QList<VertexWeight> DatabaseHelper::queryVertexWeightSqlDatabase()
{
    QSqlQuery query;
    QList<VertexWeight> list;
    QString str="select start,end,weight from matrix";

    if(!db.isOpen())
        db.open();
    query.exec(str);

    while(query.next())
    {
        VertexWeight info;
        info.fistVertex=query.value(0).toInt();
        info.secondVertex=query.value(1).toInt();
        info.weight=query.value(2).toInt();
        list.append(info);
    }
    query.clear();
    db.close();
    return list;
}

//查询
QStringList DatabaseHelper::queryOneStringSqlDatabase(const QString &field,const QString &tabname)
{
    QSqlQuery query;
    QStringList list;
    QString str="select "+field+" from "+tabname;

    if(!db.isOpen())
        db.open();
    query.exec(str);

    while(query.next())
    {
        list.append(query.value(0).toString());
    }
    query.clear();
    db.close();
    return list;
}

//查询edge
Edges DatabaseHelper::queryEdgeSqlDatabase(const QString &_name)
{
    Edges edge;
    QSqlQuery query;
    QString str="select name,dis,sort from edge where name = '"+_name+"'";

    if(!db.isOpen())
        db.open();
    query.exec(str);
    while(query.next())
    {
        edge.name=query.value(0).toString();
        edge.dis=query.value(1).toInt();
        edge.sort=query.value(2).toBool();
    }
    query.clear();
    db.close();
    return edge;
}

//
QList<RouteInfo> DatabaseHelper::querySqlDatabase(unsigned short num)
{
    QList<RouteInfo> routeInfoList;
    if(recordCount==0)
        recordCount=getSqlCount(tableList.at(tableListIndex).name);
    routeInfoList=querySqlDatabaseSub(num);
    transSqlDatabaseExteral(routeInfoList,num);
    return routeInfoList;
}

void DatabaseHelper::transSqlDatabaseExteral(QList<RouteInfo> &data,unsigned short num)
{
    unsigned short dataNum=data.size();

    if((dataNum>0&&dataNum<num)||(recordCount==indexRoutesData)){
        if(tableListIndex==tableList.size()-1)
            return;
        indexRoutesData=1;
        tableListIndex++;
        data.removeLast();
        recordCount=getSqlCount(tableList.at(tableListIndex).name);
        data.append(querySqlDatabaseSub(num-dataNum+1));
        transSqlDatabaseExteral(data,num);
    }
}

QList<RouteInfo> DatabaseHelper::querySqlDatabaseSub(unsigned short num)
{
    QSqlQuery query;
    QString _sort;
    tableList.at(tableListIndex).sort?_sort="asc":_sort="desc";
    QString str="select lat,log from "+tableList.at(tableListIndex).name+" order by id "+_sort+" limit ";//asc desc
    str+=QString::number(indexRoutesData);

    if(num>0){
        str+=","+QString::number(num);
        indexRoutesData+=num;
    }
    if(!db.isOpen()){
        db.open();
    }
    query.exec(str);
    QList<RouteInfo> routeInfoList;
    while(query.next())
    {
        RouteInfo routeInfo;
        routeInfo.lat=query.value(0).toDouble();
        routeInfo.log=query.value(1).toDouble();
        routeInfoList.append(routeInfo);
    }
    query.clear();
    db.close();
    return routeInfoList;
}

QList<RouteInfo> DatabaseHelper::queryAllSqlDatabase(const QString &tablename)
{
    QSqlQuery query;
    if(!db.isOpen())
        db.open();
    query.exec("select lat,log from "+tablename);
    QList<RouteInfo> routeInfoList;
    while(query.next())
    {
        RouteInfo routeInfo;
        routeInfo.lat=query.value(0).toDouble();
        routeInfo.log=query.value(1).toDouble();
        routeInfoList.append(routeInfo);
    }
    query.clear();
    db.close();
    return routeInfoList;
}

//插入数据
void DatabaseHelper::insertSqlDatabase(double lat,double log)
{
    QSqlQuery query;
    if(!db.isOpen())
        db.open();

    query.prepare("INSERT INTO "+tableName1+"(lat, log) "
               "VALUES (:lat, :log)");
    query.bindValue(":lat", lat);
    query.bindValue(":log", log);
    query.exec();
    query.clear();
    db.close();
}

//组织路段表，表名
void DatabaseHelper::setQueryTableNames(const QStringList &data)
{
    Edges edge;
    tableList.clear();
    for(int i=0;i<data.size();i++){
        edge=queryEdgeSqlDatabase(data.at(i));
        if(!edge.sort){
            QStringList list = edge.name.split('_', QString::SkipEmptyParts);
            edge.name=list.at(1)+"_"+list.at(0);
        }
        edge.name="edge_"+edge.name;
        tableList.append(edge);
    }
}

//更新数据
void DatabaseHelper::updateSqlDatabase(const QString &tablename,unsigned id,RouteInfo &data)
{
    QSqlQuery query;
    if(!db.isOpen())
        db.open();
    query.exec("update "+tablename+" set lat="+QString::number(data.lat)+",log ="+QString::number(data.log)+" where _id="+QString::number(id));
    query.clear();
    db.close();
}

//删除数据
void DatabaseHelper::deleteSqlDatabase(const QString &tablename,unsigned id)
{
    QSqlQuery query;
    if(!db.isOpen())
        db.open();
    if(!query.exec("delete from "+tablename+" where _id="+QString::number(id)))
        qDebug()<<query.lastError();
    else
        qDebug()<<"deleted!";
    qDebug()<<tablename<<","<<id;
    query.clear();
    db.close();
}
