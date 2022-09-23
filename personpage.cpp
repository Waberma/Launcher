#include "personpage.h"
#include "ui_personpage.h"
#include "qlistmodel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QAction>
#include <QSqlRecord>
#include <QCommonStyle>

void PersonPage::DeleteEl(QString el, QList<QString> &list)
{
    int i = 0;
    for(auto element : list)
    {
        if(element == el)
        {
            list.removeAt(i);
            break;
        }
        i++;
    }
}

QString PersonPage::toString(QList<QString> list)
{
    QString str;
    QString comma;
    for(auto element :list)
    {
        str += comma + "'" + element + "'";
        comma = ",";
    }
    return str;
}

PersonPage::PersonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PersonPage)
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

    persontm = new QSqlTableModel(this, db);
    persontm->setTable("Person");
    persontm->select();
    ui->Table->setModel(persontm);
    on_CancelPB_clicked();

    QCommonStyle style;
    ui->addrolepb->setIcon(style.standardIcon(QStyle::SP_ArrowUp));
    ui->addrolepb->setEnabled(false);

    ui->deleterolepb->setIcon(style.standardIcon(QStyle::SP_ArrowDown));
    ui->deleterolepb->setEnabled(false);
}

PersonPage::~PersonPage()
{
    clearAction();
    delete ui;
}

void PersonPage::on_Table_clicked(const QModelIndex &index)
{
    if(activite == false)
    {
        clearAction();
        ui->addWidget->clear();
        obtain.clear();
        row = index.row();
        QSqlQuery *updateq = new QSqlQuery(db);
        updateq->prepare("Update RoleActions "
                         "SET Active = true "
                         "WHERE role IN(SELECT name FROM ROLE "
                         "LEFT JOIN RolePerson "
                         "ON Role.ID = RolePerson.RoleID "
                         "LEFT JOIN Person "
                         "ON RolePerson.PersonID = Person.ID "
                         "Where Person.ID = :ID);");
        updateq->bindValue(":ID",persontm->record(row).value("ID").toInt());
        updateq->exec();

        QSqlQuery *addroleq = new QSqlQuery(db);
        addroleq->exec("SELECT Role FROM RoleActions "
                       "WHERE Active = 1");
        while(addroleq->next())
        {
            obtain.push_back(addroleq->record().value("Role").toString());
        }
        ui->addWidget->addItems(obtain);

        ui->editPB->setVisible(true);
    }
}

void PersonPage::on_addRowPB_clicked()
{
    persontm->insertRow(persontm->rowCount());
}

void PersonPage::on_editPB_clicked()
{
    ui->verticalSpacer_2->changeSize(0,0);
    activite = true;
    ui->editPB->setVisible(false);
    ui->OkPB->setVisible(true);
    ui->delWidget->setVisible(true);
    persontm->setFilter("ID = " + QString::number(row+1));
    ui->editPB->setVisible(false);
    ui->CancelPB->setVisible(true);

    QSqlQuery *deleteroleq = new QSqlQuery(db);
    deleteroleq->exec("SELECT Role FROM RoleActions "
                   "WHERE Active = 0");

    while(deleteroleq->next())
    {
        notObtain.push_back(deleteroleq->record().value("Role").toString());
    }
    ui->delWidget->addItems(notObtain);
    ui->addRowPB->setVisible(false);
    ui->addrolepb->setVisible(true);
    ui->deleterolepb->setVisible(true);
    statusBar("Выберите роль для начала работы!");
}

void PersonPage::on_OkPB_clicked()
{
    ui->verticalSpacer_2->changeSize(20,175);
    activite = false;
    QSqlQuery *editrolesq = new QSqlQuery(db);
    editrolesq->prepare("DELETE FROM RolePerson "
                        "WHERE PersonID = :PersonID;");
    editrolesq->bindValue(":PersonID", persontm->record(0).value("ID").toInt());
    editrolesq->exec();
    while(!obtain.isEmpty())
    {
        QSqlQuery *roleq = new QSqlQuery(db);
        roleq->prepare("SELECT ID FROM Role "
                       "WHERE name = :name");
        roleq->bindValue(":name", obtain.at(0));
        roleq->exec();
        roleq->next();
        editrolesq->prepare("INSERT INTO RolePerson (PersonID, RoleID) "
                          "VALUES (:PersonID, :RoleID);");
        editrolesq->bindValue(":PersonID", persontm->record(0).value("ID").toInt());
        editrolesq->bindValue(":RoleID", roleq->record().value("ID").toInt());
        editrolesq->exec();
        obtain.pop_front();
    }

    QSqlQuery *userId = new QSqlQuery(db);
    userId->prepare("SELECT ID FROM Person "
                      "WHERE Login = :login;");
    userId->bindValue(":login", user);
    userId->exec();
    userId->next();
    QSqlQuery *roleq = new QSqlQuery(db);
    roleq->prepare("SELECT RoleID "
               "FROM RolePerson "
               "WHERE PersonID = :ID;");
    roleq->bindValue(":ID", userId->record().value("ID"));
    roleq->exec();
    QList<QString> listRoles;
    while(roleq->next())
    {
        listRoles.push_back(roleq->record().value("RoleID").toString());
    }
    emit StartCreateMenu(listRoles);
    obtain.clear();
    notObtain.clear();
    on_CancelPB_clicked();
}

void PersonPage::on_CancelPB_clicked()
{
    ui->verticalSpacer_2->changeSize(20,175);
    activite = false;
    clearAction();
    row = 0;
    persontm->setFilter("ID IS NOT NULL");
    ui->editPB->setVisible(false);
    ui->addrolepb->setVisible(false);
    ui->deleterolepb->setVisible(false);
    ui->addRowPB->setVisible(true);
    ui->CancelPB->setVisible(false);
    ui->delWidget->setVisible(false);
    ui->addWidget->setVisible(true);
    ui->addWidget->clear();
    ui->delWidget->clear();
    ui->line->setVisible(false);
    ui->line_2->setVisible(false);
    ui->addrolepb->setVisible(false);
    ui->deleterolepb->setVisible(false);
    ui->OkPB->setVisible(false);
}

void PersonPage::clearAction()
{
    QSqlQuery *updateq = new QSqlQuery(db);
    updateq->exec("Update RoleActions "
                  "SET Active = false;");
}

void PersonPage::on_addrolepb_clicked()
{
    ui->delWidget->setVisible(true);
    action = "Добавить роль";
    QListWidgetItem *item = ui->delWidget->currentItem();
    obtain.push_back(item->text());
    DeleteEl(item->text(),notObtain);
    ui->delWidget->clear();
    ui->delWidget->addItems(notObtain);
    ui->addWidget->clear();
    ui->addWidget->addItems(obtain);
    ui->addrolepb->setEnabled(false);
}

void PersonPage::on_deleterolepb_clicked()
{
    action = "Удалить роль";
    QListWidgetItem *item = ui->addWidget->currentItem();
    notObtain.push_back(item->text());
    DeleteEl(item->text(),obtain);
    ui->addWidget->clear();
    ui->addWidget->addItems(obtain);
    ui->delWidget->clear();
    ui->delWidget->addItems(notObtain);
    ui->deleterolepb->setEnabled(false);
}

//void PersonPage::openDB(QString DBname)
//{
//    if(DBname == "Person")
//    {
//        persontm = new QSqlTableModel(this, db);
//        persontm->setTable("Person");
//        persontm->select();
//        ui->Table->setModel(persontm);
//        on_CancelPB_clicked();
//    }
//    if(DBname == "Role")
//    {
//        persontm = new QSqlTableModel(this, db);
//        persontm->setTable("Role");
//        persontm->select();
//        ui->Table->setModel(persontm);

//    }
//}

void PersonPage::on_addWidget_itemClicked(QListWidgetItem *item)
{
    ui->deleterolepb->setEnabled(true);
}

void PersonPage::on_delWidget_itemClicked(QListWidgetItem *item)
{
    ui->addrolepb->setEnabled(true);
}
