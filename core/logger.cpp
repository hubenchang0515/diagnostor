#include <logger.h>
#include <database.h>
#include <QDebug>
#include <QDateTime>

Logger::Logger(AbstractDiagnostor* diagnostor) :
    QObject(dynamic_cast<QObject*>(diagnostor)),
    m_name(diagnostor->name())
{

}

/* 写日志 */
void Logger::write(const QString& content, Type t)
{
    if(t == Type::Error)
    {
        qWarning() << content;
    }

    QString sql = QString("INSERT INTO '%1' VALUES(NULL, ?, ?, ?, ?);").arg(Logger::TableName);
    QVariantList args = {m_name, content, Logger::type(t), QDateTime::currentDateTime().toMSecsSinceEpoch()};
    DataBase::singleton().exec(sql, args);
}

/** 静态属性 **/
bool Logger::IsInited = false;

/** 静态方法 **/

/*  初始化，设置数据库名称 */
bool Logger::init()
{
    QString sql = QString("CREATE TABLE IF NOT EXISTS '%1'(id INTEGER PRIMARY KEY AUTOINCREMENT, module TEXT, content TEXT, type TEXT, timestamp INTEGER);").arg(Logger::TableName);
    DataBase::singleton().lock();
    QSqlQuery query(DataBase::singleton().raw());
    query.exec(sql);
    QSqlError err = query.lastError();
    DataBase::singleton().unlock();

    if(err.isValid())
    {
        qWarning() << QString("failed to create table '%1': %2").arg(Logger::TableName).arg(err.text());
        return false;
    }

    Logger::IsInited = true;
    return true;
}

QString Logger::type(Type t)
{
    switch(t)
    {
    case Type::Info:
        return "info";
    case Type::Error:
        return "error";
    }

    return "info";
}
