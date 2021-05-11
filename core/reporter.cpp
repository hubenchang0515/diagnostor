#include <reporter.h>
#include <database.h>
#include <QtDebug>

Reporter::Reporter(AbstractDiagnostor* diagnostor) :
    QObject(dynamic_cast<QObject*>(diagnostor)),
    m_name(diagnostor->name())
{

}

/* 写诊断结果 */
void Reporter::write(int bugId, const QString& explain)
{
    QString sql = QString("INSERT INTO '%1' VALUES(NULL, ?, ?, ?);").arg(Reporter::TableName);
    QVariantList args = {m_name, bugId, explain};
    DataBase::singleton().exec(sql, args);
}


/** 静态属性 **/
bool Reporter::IsInited = false;

/** 静态方法 **/

/*  初始化，创建数据库表 */
bool Reporter::init()
{
    QString sql = QString("CREATE TABLE IF NOT EXISTS '%1'(id INTEGER PRIMARY KEY AUTOINCREMENT, module TEXT, 'bug-id' INTEGER, explain TEXT);").arg(Reporter::TableName);
    DataBase::singleton().lock();
    QSqlQuery query(DataBase::singleton().raw());
    query.exec(sql);
    QSqlError err = query.lastError();
    DataBase::singleton().unlock();

    if(err.isValid())
    {
        qWarning() << QString("failed to create table '%1': %2").arg(Reporter::TableName).arg(err.text());
        return false;
    }

    Reporter::IsInited = true;
    return true;
}
