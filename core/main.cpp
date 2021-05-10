#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
#include <scheduler.h>
#include <logger.h>
#include <reporter.h>
#include <database.h>
#include <packageversion.h>
#include <config.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 设置命令行参数
    QCommandLineOption threadOpt("thread", "how many threads will be created.", "thread", "auto");
    QCommandLineOption outputOpt("output", "output file will be <output>.sqlite.", "output", "auto");
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOptions({threadOpt, outputOpt});
    parser.process(a);

    int thread = parser.value("thread").toInt();
    QString output = parser.value("output");

    if(output == "auto")
    {
        output = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    }

    // 初始化数据库
    DataBase::singleton().open(output);
    Logger::init();
    Reporter::init();

    // 启动调度器
    Scheduler scheduler{thread};
    scheduler.push(ConfigDiagnostorList);
    scheduler.setPackages(ConfigPackages);
    scheduler.start();

    // 启动App mainloop
    return a.exec();
}
