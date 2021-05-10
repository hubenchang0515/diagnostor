#include "database.h"
#include <QDebug>
#include <QThread>

DataBase DataBase::instance;

DataBase& DataBase::singleton()
{
    return DataBase::instance;
}

DataBase::DataBase()
{
    connect(this, &DataBase::toSql, this, &DataBase::doSql, Qt::QueuedConnection);
}

DataBase::~DataBase()
{
    if(m_isOpen)
    {
        m_database.close();
    }
}

/* 打开数据库 */
bool DataBase::open(const QString& name)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(QString("%1.sqlite").arg(name));
    if(m_database.open() == false)
    {
        qWarning() << QString("failed to open '%1.sqlite'").arg(name);
        return false;
    }

    m_isOpen = true;
    return true;
}


/* 关闭数据库 */
void DataBase::close()
{
    m_database.close();
    m_isOpen = false;
}

/* 发送执行sql语句的信号 */
void DataBase::exec(const QString& sql, const QVariantList& args, bool sync)
{
    this->m_count += 1;
    if(sync)
    {
        this->doSql(sql, args);
    }
    else
    {
        emit this->toSql(sql, args);
    }
}

/* 加锁 */
void DataBase::lock()
{
    m_mutex.lock();
}

/* 解锁 */
void DataBase::unlock()
{
    m_mutex.unlock();
}

/* 获取数据库原始连接 */
QSqlDatabase DataBase::raw()
{
    return m_database;
}

/* 执行sql语句 */
void DataBase::doSql(const QString& sql, const QVariantList& args)
{
    this->lock();
    QSqlQuery query(this->m_database);
    query.prepare(sql);
    int pos = 0;
    for(const auto& arg : args)
    {
        query.bindValue(pos, arg);
        pos += 1;
    }
    query.exec();
    QSqlError err = query.lastError();
    if(err.isValid())
    {
        qWarning() << QString("fail to exec sql '%1': %2").arg(sql).arg(err.text());
    }
    this->unlock();

    if(m_count > 0)
    {
        this->m_count -= 1;
    }
    else
    {
        qWarning() << "count of database mismatched";
    }
    emit this->done(err, m_count);
}
