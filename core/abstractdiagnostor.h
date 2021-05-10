/**
* 抽象诊断器
* 所有诊断器的父类，需要实现 virtual void diagnost();
*/

#ifndef ABSTRACTDIAGNOSTOR_H
#define ABSTRACTDIAGNOSTOR_H

#include <QObject>
#include <QMutex>

class AbstractDiagnostor : public QObject
{
    Q_OBJECT
public:
    /** 构造和析构 **/
    explicit AbstractDiagnostor(QObject *parent = nullptr);
    virtual ~AbstractDiagnostor();

    /** 静态方法 **/

    /** 全局锁，用于保护跨诊断器共用的资源 **/
    static void lock();
    static void unlock();
    static bool tryLock(int timeout = 0);

    /** 纯虚函数 **/
    virtual void diagnost() = 0;
    virtual QString name() = 0;

signals:
    void start();
    void done();

public slots:
    virtual void doDiagnost() final;

private:
    static QMutex mutex;
};

#endif // ABSTRACTDIAGNOSTOR_H
