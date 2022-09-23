#include "tablevier.h"
#include "ui_tablevier.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QMenuBar>
TableVier::TableVier(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableVier)
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
    QMenuBar *menubar = new QMenuBar;

    QMenu *menu = new QMenu("Базы данных");
    QAction *action = new QAction("MenuItems",this);
    connect(action, SIGNAL(triggered()), this, SLOT(bdChose()));
    menu->addAction(action);

    action = new QAction("Person",this);
    connect(action, SIGNAL(triggered()), this, SLOT(bdChose()));
    menu->addAction(action);
    menubar->addMenu(menu);

    action = new QAction("Role",this);
    connect(action, SIGNAL(triggered()), this, SLOT(bdChose()));
    menu->addAction(action);
    menubar->addMenu(menu);

    action = new QAction("RolePerson",this);
    connect(action, SIGNAL(triggered()), this, SLOT(bdChose()));
    menu->addAction(action);
    menubar->addMenu(menu);

    action = new QAction("RoleItems",this);
    connect(action, SIGNAL(triggered()), this, SLOT(bdChose()));
    menu->addAction(action);
    menubar->addMenu(menu);

    ui->verticalLayout->addWidget(menubar);
}

TableVier::~TableVier()
{
    delete ui;
}

void TableVier::on_Remove_clicked()
{
    tm->removeRow(row);
}

void TableVier::on_tableView_clicked(const QModelIndex &index)
{
    row = index.row();
}

void TableVier::on_Remove_2_clicked()
{
    tm->insertRow(tm->rowCount());
}

void TableVier::bdChose()
{
    QAction *action = (QAction *) sender();
    if(action->text() == "MenuItems")
    {
        QSqlQuery *q = new QSqlQuery(db);
        q->exec("SELECT name FROM Role;");
        tm = new QSqlTableModel(this, db);
        tm->setTable("MenuItems");
        tm->select();
        ui->tableView->setModel(tm);
    }
    if(action->text() == "Person")
    {
        tm = new QSqlTableModel(this, db);
        tm->setTable("Person");
        tm->select();
        ui->tableView->setModel(tm);
    }
    if(action->text() == "Role")
    {
        tm = new QSqlTableModel(this, db);
        tm->setTable("Role");
        tm->select();
        ui->tableView->setModel(tm);
    }
    if(action->text() == "RolePerson")
    {
        tm = new QSqlTableModel(this, db);
        tm->setTable("RolePerson");
        tm->select();
        ui->tableView->setModel(tm);
    }
    if(action->text() == "RoleItems")
    {
        tm = new QSqlTableModel(this, db);
        tm->setTable("RoleItems");
        tm->select();
        ui->tableView->setModel(tm);
    }
}
