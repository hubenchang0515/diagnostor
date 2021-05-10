#include "scheduler.h"
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <database.h>
#include <logger.h>
#include <reporter.h>
#include <packageversion.h>

Scheduler::Scheduler(int thread, QObject *parent) : QObject(parent)
{
    connect(this, &Scheduler::noDiagnostor, this, &Scheduler::empty);

    int cpuThreadNum = Scheduler::getCpuThreadNum();
    if(thread > 0)
    {
        m_threadNum = thread;
    }
    else if(cpuThreadNum)
    {
        m_threadNum = cpuThreadNum;
    }
    else
    {
        m_threadNum = 4;
    }
    qInfo() << "will create" << m_threadNum << "threads";

    for(int i = 0; i < m_threadNum; i++)
    {
        QThread* thread = new QThread(this);
        m_threads.append(thread);
    }
}

Scheduler::~Scheduler()
{
    for(auto& thread : m_threads)
    {
        thread->quit();
        thread->wait();
        thread->deleteLater();
    }
}

/* 添加诊断器 */
void Scheduler::push(AbstractDiagnostor* diagnostor)
{
    m_diagnostors.enqueue(diagnostor);
}

/* 添加一堆诊断器 */
void Scheduler::push(const QList<AbstractDiagnostor*>& diagnostors)
{
    for(auto& diagnostor : diagnostors)
    {
        this->push(diagnostor);
    }
}

/* 设置想要获取版本号的软件包 */
void Scheduler::setPackages(const QList<QString>& packages)
{
    m_packages = packages;
}

/* 启动 */
void Scheduler::start()
{
    for(QThread* thread : m_threads)
    {
        thread->start();
        startDiagnostor(thread);
    }
}

/* 停止 */
void Scheduler::stop()
{

    // 退出各个线程
    for(auto& thread : m_threads)
    {
        thread->quit();
    }

    // 获取并保存软件包版本
    PackageVersion::getVersion(m_packages);
    PackageVersion::save();

    // 等待各个线程退出完毕
    for(auto& thread : m_threads)
    {
        thread->wait();
    }

    // 等等数据库操作全部完成后退出
    connect(&DataBase::singleton(), &DataBase::done, [](const QSqlError& err, uint n){
        (void)(err);
        if(n == 0)
        {
            QCoreApplication::exit(0);
        }
    });
}

/* 在指定的线程上启动一个诊断器 */
void Scheduler::startDiagnostor(QThread* thread)
{
    if(thread == nullptr)
    {
        qWarning() << QString("try to diagnost in a null thread");
        return;
    }

    if(!m_diagnostors.empty())
    {
        AbstractDiagnostor* diagnostor = m_diagnostors.dequeue();
        connect(diagnostor, &AbstractDiagnostor::done, this, &Scheduler::diagnosted);
        diagnostor->moveToThread(thread);
        m_search[diagnostor] = thread;
        emit diagnostor->start();
    }
    else
    {
        emit this->noDiagnostor();
    }
}

/* 处理一个诊断完成的信号 */
void Scheduler::diagnosted()
{
    AbstractDiagnostor* diagnostor = qobject_cast<AbstractDiagnostor*>(QObject::sender());
    QThread* thread = m_search[diagnostor];
    m_search.remove(diagnostor);
    delete diagnostor;
    startDiagnostor(thread);

    // m_diagnostors 为空说明没有等待执行的诊断器
    // m_search 为空说明没有正在执行的诊断器
    // 即诊断完成
    if(m_diagnostors.empty() && m_search.empty())
    {
        this->stop();
    }
}

/* 处理诊断器队列空信号 */
void Scheduler::empty()
{
    // m_diagnostors 为空说明没有等待执行的诊断器
    // m_search 为空说明没有正在执行的诊断器
    // 即诊断完成
    if(m_diagnostors.empty() && m_search.empty())
    {
        this->stop();
    }
}

int Scheduler::getCpuThreadNum()
{
#ifdef linux
    QFile file(cpuPresentFile);
    if(!file.open(QFile::ReadOnly))
    {
        qWarning() << QString("failed to open file '%1'").arg(cpuPresentFile);
        return -1;
    }

    QString text = file.readAll();
    QStringList num = text.split("-");
    if(num.length() != 2)
    {
        qWarning() << QString("cannot parse file '%1': '%2'").arg(cpuPresentFile).arg(text);
        return -1;
    }

    bool ok1, ok2;
    int n1 = num[0].toInt(&ok1);
    int n2 = num[1].toInt(&ok2);
    if(ok1 && ok2)
    {
        return n2 - n1 + 1;
    }
    else
    {
        qWarning() << QString("cannot parse file '%1': '%2'").arg(cpuPresentFile).arg(text);
        return -1;
    }

#else
    return -1;
#endif
}
