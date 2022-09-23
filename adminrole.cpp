#include "adminrole.h"
#include "ui_adminrole.h"

AdminRole::AdminRole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminRole)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./menu.db");
    if(db.open())
    {
        qDebug("Open");
    }
    else
    {
        qDebug("NoOpen");
    }
}

AdminRole::~AdminRole()
{
    delete ui;
}
