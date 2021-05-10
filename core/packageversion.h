/**
* APT版本
* 这个类用于获取、比较和记录APT软件包的版本
* 凡是通过此类获取过的软件包版本都会自动写入数据库中
* 使用示例:
*   auto apt = new AptVersion(this);
*   qDebug() << apt.getVersion("dde-daemon");
*/

#ifndef APTVERSION_H
#define APTVERSION_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <database.h>

class PackageVersion
{
public:
    enum class Type : int
    {
        Dpkg,
        Rpm,
    };

    /** 静态常量 **/
    static constexpr const char* TableName = "diagnostor-package-version";

    /** 构造和析构 **/
    explicit PackageVersion(const QString& v);
    PackageVersion() = default;
    PackageVersion(const PackageVersion&) = default;
    PackageVersion(PackageVersion&&) = default;

    /* 获取一个dpkg软件包的版本 */
    static PackageVersion fromDpkg(const QString& package, bool readCache=true);

    /* 获取一个软件包的版本 */
    static PackageVersion getVersion(const QString& package, Type t=Type::Dpkg,  bool readCache=true);

    /* 获取一堆软件包的版本 */
    static void getVersion(const QList<QString>& packages, Type t=Type::Dpkg,  bool readCache=true);

    /* 获取一组软件包的版本 */
    static void readVersions(const QStringList& packages);

    /* 存入数据库 */
    static void save();

    /* 转换为QString */
    const QString& toString() const;

    /** 运算符重载 **/
    bool operator < (const PackageVersion& another) const;
    bool operator > (const PackageVersion& another) const;
    bool operator == (const PackageVersion& another) const;
    bool operator != (const PackageVersion& another) const;
    bool operator <= (const PackageVersion& another) const;
    bool operator >= (const PackageVersion& another) const;
    PackageVersion& operator = (const PackageVersion& another) = default;
    PackageVersion& operator = (PackageVersion&& another) = default;

signals:

public slots:

private:
    /** 私有属性 **/
    QString m_version;

    /** 静态属性 **/
    static QMutex cacheMutex;
    static QMap<QString, PackageVersion> cache;

    /* 获取dpkg软件包的版本 */
    static QString dpkgVersion(const QString& package);

    /* 创建数据库表 */
    static void createTable();

    /* 保存到数据库 */
    static void doSave();
};

#endif // APTVERSION_H
