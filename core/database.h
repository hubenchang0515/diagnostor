/**
* 数据库
* 用于保障数据库并发操作的线程安全
*/

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QVariant>

class DataBase : public QObject
{
    Q_OBJECT
public:
    static DataBase& singleton();

    bool open(const QString& name);
    void close();
    void exec(const QString& sql, const QVariantList& args={}, bool sync=false);
    void lock();
    void unlock();
    QSqlDatabase raw();

signals:
    void toSql(const QString& sql, const QVariantList& args={});
    void done(const QSqlError& err, uint n);

private slots:
    void doSql(const QString& sql, const QVariantList& args={});

private:
    static DataBase instance;

    bool m_isOpen;
    QSqlDatabase m_database;
    QMutex m_mutex;
    uint m_count;

    DataBase();
    ~DataBase();
};

#endif // DATABASE_H
