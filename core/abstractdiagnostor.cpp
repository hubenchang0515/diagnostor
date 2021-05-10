#include <abstractdiagnostor.h>
#include <QDebug>
#include <QDateTime>

QMutex AbstractDiagnostor::mutex;

AbstractDiagnostor::AbstractDiagnostor(QObject *parent) : QObject(parent)
{
    connect(this, &AbstractDiagnostor::start, this, &AbstractDiagnostor::doDiagnost);
}

AbstractDiagnostor::~AbstractDiagnostor()
{

}

void AbstractDiagnostor::lock()
{
    AbstractDiagnostor::mutex.lock();
}

void AbstractDiagnostor::unlock()
{
    AbstractDiagnostor::mutex.unlock();
}

bool AbstractDiagnostor::tryLock(int timeout)
{
    return AbstractDiagnostor::mutex.tryLock(timeout);
}

void AbstractDiagnostor::doDiagnost()
{
    qInfo() << QDateTime::currentDateTime().toString() << this->name() << "start";
    this->diagnost();
    qInfo() << QDateTime::currentDateTime().toString() << this->name() << "finished";
    emit this->done();
}
