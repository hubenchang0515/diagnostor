/**
* 日志记录器
* 用于记录日志，在诊断过程中的任何可能有用的信息都可以使用这个类来进行记录
* 考虑自动诊断可能存在误识别，可以基于日志进行人工分析
* 诊断器的开发者不应该调用 Logger::init，这个方法由调度器进行调用
* 只提供 Logger::Type::Info(默认) 和 Logger::Type::Error 两种日志
* 使用示例:
*   auto log = new Logger(name, this);
*   log->write("日志内容");
*   log->write("日志内容", Logger::Type::Error);
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMutex>
#include <abstractdiagnostor.h>
#include <database.h>

class Logger : public QObject
{
    Q_OBJECT
public:
    /** 静态常量 **/
    static constexpr const char* TableName = "diagnostor-log";

    /* 日志类型 */
    enum class Type
    {
        Info,
        Error,
    };

    /** 构造和析构 **/
    explicit Logger(AbstractDiagnostor* diagnostor, QObject *parent = nullptr);

    /* 写日志 */
    void write(const QString& content, Type t=Type::Info);

    /* 初始化，创建数据库表 */
    static bool init();

    /* 退出 */
    static void quit();

    /* 获取数据库名称 */
    static QString databaseName();

public slots:

private:
    /** 属性 **/
    const QString m_name;

    /** 静态属性 **/
    static bool IsInited;

    /** 静态方法 **/
    static QString type(Type t);
};

#endif // LOGGER_H
