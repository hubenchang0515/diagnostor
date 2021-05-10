/**
* 调度器
* 用于调度诊断器
* 调度器会自动读取CPU的逻辑核心数量，并创建对应数量的线程，将诊断器放入线程中并发运行
* 使用示例:
*   Scheduler scheduler;
*   scheduler.push(new AbstractDiagnostor());
*   scheduler.start();
*/


#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QQueue>
#include "abstractdiagnostor.h"

class Scheduler : public QObject
{
    Q_OBJECT
public:
    static constexpr const char* cpuPresentFile = "/sys/devices/system/cpu/present";

    explicit Scheduler(int thread = 0, QObject *parent = nullptr);
    ~Scheduler();

    /* 添加诊断器 */
    template<typename T>
    inline void push()
    {
        this->push(new T);
    }

    /* 添加诊断器 */
    void push(AbstractDiagnostor* diagnostor);

    /* 添加一堆诊断器 */
    void push(const QList<AbstractDiagnostor*>& diagnostors);

    /* 设置想要获取版本号的软件包 */
    void setPackages(const QList<QString>& packages);

    /* 启动 */
    void start();

    /* 停止 */
    void stop();

    /* 获取CPU的超线程数量 */
    static int getCpuThreadNum();

signals:
    void noDiagnostor();

private slots:
    /* 处理一个诊断完成的信号 */
    void diagnosted();

    /* 处理诊断器队列空信号 */
    void empty();

private:
    int m_threadNum;
    QList<QThread*> m_threads;
    QQueue<AbstractDiagnostor*> m_diagnostors;
    QMap<AbstractDiagnostor*, QThread*> m_search; // 正在运行的诊断器 => 线程
    QList<QString> m_packages;

    /* 在指定的线程上启动一个诊断器 */
    void startDiagnostor(QThread* thread);
};

#endif // SCHEDULER_H
