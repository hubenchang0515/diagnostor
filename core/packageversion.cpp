#include <packageversion.h>
#include <database.h>
#include <QProcess>
#include <QDebug>

QMutex PackageVersion::cacheMutex;

PackageVersion::PackageVersion(const QString& v) :
    m_version(v)
{

}

/* 获取一个dpkg软件包的版本 */
PackageVersion PackageVersion::fromDpkg(const QString& package, bool readCache)
{
    if(readCache && PackageVersion::cache.contains(package))
    {
        PackageVersion::cacheMutex.lock();
        auto v = PackageVersion::cache[package];
        PackageVersion::cacheMutex.unlock();
        return v;
    }
    else
    {
        auto v = PackageVersion(PackageVersion::dpkgVersion(package));
        PackageVersion::cacheMutex.lock();
        PackageVersion::cache[package] = v;
        PackageVersion::cacheMutex.unlock();
        return v;
    }
}

/* 获取一个软件包的版本 */
PackageVersion PackageVersion::getVersion(const QString& package, PackageVersion::Type t,  bool readCache)
{
    switch(t)
    {
    case Type::Dpkg:
        return PackageVersion::fromDpkg(package, readCache);

    default:
        return PackageVersion();
    }
}

/* 获取一堆软件包的版本 */
void PackageVersion::getVersion(const QList<QString>& packages, PackageVersion::Type t,  bool readCache)
{
    for(auto& package : packages)
    {
        PackageVersion::getVersion(package, t, readCache);
    }
}



/* 获取一组软件包的版本 */
void readVersions(const QStringList& packages)
{
    for(auto& package : packages)
    {
        PackageVersion::getVersion(package);
    }
}

/* 存入数据库 */
void PackageVersion::save()
{
    PackageVersion::createTable();
    PackageVersion::doSave();
}

const QString& PackageVersion::toString() const
{
    return m_version;
}

/* 比较版本大小 */
bool PackageVersion::operator < (const PackageVersion& another) const
{
    QStringList v1 = this->m_version.split(".");
    QStringList v2 = another.m_version.split(".");
    for(int i = 0; i < v1.length() && i < v2.length(); i++)
    {
        bool ok1, ok2;
        int i1 = v1[i].toInt(&ok1);
        int i2 = v2[i].toInt(&ok2);

        if(ok1 && ok2) // 数字版本号
        {
            if(i1 < i2)
            {
                return true;
            }
            else if(i1 > i2) {
                return false;
            }
        }
        else // 字符串版本号
        {
            if( v1[i] < v2[i])
            {
                return true;
            }
            else if (v1[i] > v2[i]) {
                return false;
            }
        }


    }

    // 相等
    return false;
}

/** 偏序推导 **/

bool PackageVersion::operator > (const PackageVersion& another) const
{
    return another < *this;
}

bool PackageVersion::operator == (const PackageVersion& another) const
{
    return !(*this < another) && !(another < *this);
}

bool PackageVersion::operator != (const PackageVersion& another) const
{
    return !(*this == another);
}

bool PackageVersion::operator <= (const PackageVersion& another) const
{
    return !(*this > another);
}

bool PackageVersion::operator >= (const PackageVersion& another) const
{
    return !(*this < another);
}


QMap<QString, PackageVersion> PackageVersion::cache;

/* 获取软件包的版本 */
QString PackageVersion::dpkgVersion(const QString& package)
{
    auto process = new QProcess(nullptr);
    process->setProgram("dpkg");
    process->setArguments({"-s", package});
    process->start(QIODevice::ReadOnly);
    if(!process->waitForFinished())
    {
        process->kill();
        process->deleteLater();
        return "";
    }

    QString data = process->readAllStandardOutput();
    process->deleteLater();

    QStringList lines = data.split("\n");
    for(auto& line : lines)
    {
        QStringList pair = line.split((" "));
        if(pair[0] == "Version:")
        {
            return pair[1];
        }
    }

    return "";
}

/* 创建数据库表 */
void PackageVersion::createTable()
{
    QString sql = QString("CREATE TABLE IF NOT EXISTS '%1'(id INTEGER PRIMARY KEY AUTOINCREMENT, package TEXT, version TEXT);").arg(PackageVersion::TableName);
    DataBase::singleton().exec(sql);
}

/* 保存到数据库 */
void PackageVersion::doSave()
{
    QString sql = QString("INSERT INTO '%1' VALUES(NULL, ?, ?);").arg(PackageVersion::TableName);
    for(auto& key : PackageVersion::cache.keys())
    {
        QString ver = PackageVersion::cache[key].toString();
        QVariantList args = {key, ver};
        DataBase::singleton().exec(sql, args);
    }
}
