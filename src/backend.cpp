#include "backend.h"

Backend::Backend(QObject *parent) : QObject(parent)
{

}

void Backend::getTest()
{
    emit setTest();
}

void Backend::exit()
{
    qDebug() << "EXIOTTT";
}
