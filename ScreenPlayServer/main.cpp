#include <QCoreApplication>
#include <QScopedPointer>
#include <QSharedPointer>

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

#include "src/aimberserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AimberServer ps(16395);
/*
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("ProtoLogin");
    db.setUserName("CommunityUser");
    //db.setPassword("windowsPW2");
    qDebug() << "Open Database " << db.open();

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM public.\"User\"");
    qDebug() << model->rowCount();
*/

    return a.exec();
}
