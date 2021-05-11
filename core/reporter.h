/**
* 报告生成器
* 用于生成最终的诊断结果
* 诊断器的开发者不应该调用 Reporter::init，这个方法由调度器进行调用
* 使用示例:
*   auto report = new Reporter(name, this);
*   report->write(bugId, "bug的说明信息")
* BUG-ID用于标识BUG，暂时使用PMS上的BUG号。一个BUG可能对应多个commit。
* TODO:根据BUG-ID，自动导出commit清单
*/

#ifndef REPORTER_H
#define REPORTER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMutex>
#include <abstractdiagnostor.h>
#include <database.h>

class Reporter : public QObject
{
    Q_OBJECT
public:
    /** 静态常量 **/
    static constexpr const char* TableName = "diagnostor-report";

    /** 构造和析构 **/
    explicit Reporter(AbstractDiagnostor* diagnostor);

    /* 初始化，创建数据库表 */
    static bool init();

    /* 退出 */
    static void quit();

    /* 写诊断结果 */
    void write(int bugId, const QString& explain);

public slots:

private:
    /** 属性 **/
    const QString m_name;

    /** 静态属性 **/
    static bool IsInited;
};

#endif // REPORTER_H
