# Module
This directory is used to store diagnostors. 

The diagnostors must have to inherit the class `AbstractDiagnostor` and implement virtual methods `name` and `diagnost`.

This is the demo:

```cpp
#include <QObject>
#include <abstractdiagnostor.h>
#include <logger.h>
#include <reporter.h>

class DemoDiagnostor : public AbstractDiagnostor
{
    Q_OBJECT
public:
    explicit DemoDiagnostor(QObject *parent = nullptr);

signals:

public slots:
    virtual void diagnost() override;
    virtual QString name() override;

private:
    Logger* log;
    Reporter* report;
};

DemoDiagnostor::DemoDiagnostor(QObject *parent) :
    AbstractDiagnostor(parent),
    log(new Logger(this)),
    report(new Reporter(this))
{

}

QString DemoDiagnostor::name()
{
    return "DemoDiagnostor";
}

void DemoDiagnostor::diagnost()
{
   // TODO: write your  code here
}

```