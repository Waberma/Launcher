#include "rolepage.h"
#include "ui_rolepage.h"
#include "qlistmodel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QAction>
#include <QSqlRecord>
#include <QCommonStyle>

QString RolePage::toString(QList<QString> list)
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

void RolePage::deleteEl(QString el, QList<QString> &list)
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

RolePage::RolePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RolePage)
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

    roletm = new QSqlTableModel(this, db);
    roletm->setTable("Role");
    roletm->select();
    ui->Table->setModel(roletm);
    on_CancelPB_clicked();

    QCommonStyle style;
    ui->additempb->setIcon(style.standardIcon(QStyle::SP_ArrowUp));
    ui->additempb->setEnabled(false);

    ui->deleteitempb->setIcon(style.standardIcon(QStyle::SP_ArrowDown));
    ui->deleteitempb->setEnabled(false);
}

RolePage::~RolePage()
{
    clearAction();
    delete ui;
}

void RolePage::on_Table_clicked(const QModelIndex &index)
{
    if(!activite)
    {
        clearAction();
        ui->addWidget->clear();
        obtain.clear();
        row = index.row();
        ui->editPB->setVisible(true);
        QSqlQuery *updateq = new QSqlQuery(db);
        updateq->prepare("Update ItemActions "
                         "SET Active = true "
                         "WHERE item IN(Select title "
                         "FROM MenuItems "
                         "LEFT JOIN RoleItems "
                         "ON MenuItems.ID = RoleItems.ItemID "
                         "WHERE RoleItems.RoleID = :ID);");
        updateq->bindValue(":ID",roletm->record(row).value("ID").toInt());
        updateq->exec();

        QSqlQuery *addroleq = new QSqlQuery(db);
        addroleq->exec("SELECT Item FROM ItemActions "
                       "WHERE Active = 1");

        while(addroleq->next())
        {
            obtain.push_back(addroleq->record().value("Item").toString());
        }
        ui->addWidget->addItems(obtain);
    }
}

void RolePage::on_addRowPB_clicked()
{
    roletm->insertRow(roletm->rowCount());
}

void RolePage::on_editPB_clicked()
{
    ui->verticalSpacer->changeSize(0,0);
    activite = true;
    ui->editPB->setVisible(false);
    ui->OkPB->setVisible(true);
    ui->delWidget->setVisible(true);
    roletm->setFilter("ID = " + QString::number(row+1));
    ui->editPB->setVisible(false);
    ui->CancelPB->setVisible(true);

    QSqlQuery *deleteroleq = new QSqlQuery(db);
    deleteroleq->exec("SELECT Item FROM ItemActions "
                   "WHERE Active = 0");

    while(deleteroleq->next())
    {
        notObtain.push_back(deleteroleq->record().value("Item").toString());
    }
    ui->delWidget->addItems(notObtain);
    ui->addRowPB->setVisible(false);
    ui->additempb->setVisible(true);
    ui->deleteitempb->setVisible(true);
    statusBar("Выберите элемент меню для начала работы!");
}

void RolePage::on_OkPB_clicked()
{
    ui->verticalSpacer->changeSize(20,175);
    activite = false;
    obtain.removeDuplicates();
    QSqlQuery *editrolesq = new QSqlQuery(db);
    editrolesq->prepare("DELETE FROM RoleItems "
                        "WHERE RoleID = :RoleID;");
    editrolesq->bindValue(":RoleID", roletm->record(0).value("ID").toInt());
    editrolesq->exec();
    while(!obtain.isEmpty())
    {
        QSqlQuery *itemq = new QSqlQuery(db);
        itemq->prepare("SELECT ID FROM MenuItems "
                       "WHERE title = :title");
        itemq->bindValue(":title", obtain.at(0));
        itemq->exec();
        itemq->next();
        editrolesq->prepare("INSERT INTO RoleItems (ItemID, RoleID) "
                          "VALUES (:ItemID, :RoleID);");
        editrolesq->bindValue(":RoleID", roletm->record(0).value("ID").toInt());
        editrolesq->bindValue(":ItemID", itemq->record().value("ID").toInt());
        editrolesq->exec();
        obtain.pop_front();
    }


    QSqlQuery *userIdq = new QSqlQuery(db);
    userIdq->prepare("SELECT ID FROM Person "
                      "WHERE Login = :login;");
    userIdq->bindValue(":login", user);
    userIdq->exec();
    userIdq->next();
    QSqlQuery *roleq = new QSqlQuery(db);
    roleq->prepare("SELECT RoleID "
               "FROM RolePerson "
               "WHERE PersonID = :ID;");
    roleq->bindValue(":ID", userIdq->record().value("ID"));
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

void RolePage::on_CancelPB_clicked()
{
    ui->verticalSpacer->changeSize(20,175);
    activite = false;
    clearAction();
    row = 0;
    roletm->setFilter("ID IS NOT NULL");
    ui->editPB->setVisible(false);
    ui->additempb->setVisible(false);
    ui->deleteitempb->setVisible(false);
    ui->addRowPB->setVisible(true);
    ui->CancelPB->setVisible(false);
    ui->delWidget->setVisible(false);
    ui->addWidget->setVisible(true);
    ui->addWidget->clear();
    ui->delWidget->clear();
    obtain.clear();
    notObtain.clear();
    ui->line->setVisible(false);
    ui->line_2->setVisible(false);
    ui->OkPB->setVisible(false);
}

void RolePage::clearAction()
{
    QSqlQuery *updateq = new QSqlQuery(db);
    updateq->exec("Update ItemActions "
                  "SET Active = false;");
}

void RolePage::on_additempb_clicked()
{
    ui->delWidget->setVisible(true);
    QListWidgetItem *item = ui->delWidget->currentItem();

    obtain.push_back(item->text());
    deleteEl(item->text(), notObtain);

    QSqlQuery *additems = new QSqlQuery(db);
    additems->exec("SELECT parent, title "
                   "FROM MenuItems "
                   "WHERE title = '" + item->text() + "';");
    additems->next();
    int parent = additems->record().value("parent").toInt();
    if(!additems->record().value("parent").isNull())
    {
        QSqlQuery *addparent = new QSqlQuery(db);
        do{
            addparent = new QSqlQuery(db);
            addparent->prepare("Select ID, parent, title "
                               "FROM MenuItems "
                               "WHERE ID = :ID AND Role IS NOT NULL;");
            addparent->bindValue(":ID", parent);
            addparent->exec();
            addparent->next();
            if(!addparent->record().value("title").isNull())
            {
                obtain.push_back(addparent->record().value("title").toString());
                deleteEl(addparent->record().value("title").toString(), notObtain);
            }
            parent = addparent->record().value("parent").toInt();
        }while(!addparent->record().value("parent").isNull());
        showLists();
        ui->additempb->setEnabled(false);
    }
    else
    {
        showLists();
        ui->additempb->setEnabled(false);
    }
}

void RolePage::on_deleteitempb_clicked()
{
    QString a;
    bool b;

    QListWidgetItem *item = ui->addWidget->currentItem();

    a = item->text();
    QSqlQuery *delitems = new QSqlQuery(db);
    delitems->exec("SELECT ID, title, appName "
                   "FROM MenuItems "
                   "WHERE title = '" + item->text() + "';");
    while(delitems->next())
    {
        b = delitems->record().value("appName").isNull();
        a = delitems->record().value("ID").toString();
        if(delitems->record().value("appName").isNull())
        {
            goNextItem(delitems->record().value("ID").toInt());
            notObtain.push_back(delitems->record().value("title").toString());
            deleteEl(delitems->record().value("title").toString(), obtain);
        }
        else
        {
            notObtain.push_back(delitems->record().value("title").toString());
            deleteEl(delitems->record().value("title").toString(), obtain);
        }
    }
    showLists();
    ui->deleteitempb->setEnabled(false);
}

void RolePage::on_addWidget_itemClicked(QListWidgetItem *item)
{
    ui->deleteitempb->setEnabled(true);
}

void RolePage::on_delWidget_itemClicked(QListWidgetItem *item)
{
    ui->additempb->setEnabled(true);
}

void RolePage::showLists()
{
    obtain.removeDuplicates();
    notObtain.removeDuplicates();
    ui->delWidget->clear();
    ui->delWidget->addItems(notObtain);
    ui->addWidget->clear();
    ui->addWidget->addItems(obtain);
}

bool RolePage::goNextItem(int parent)
{
    QSqlQuery *delitems = new QSqlQuery(db);
    delitems->prepare("SELECT ID, title, appName "
                   "FROM MenuItems "
                   "WHERE parent = :ID;");
    delitems->bindValue(":ID", parent);
    delitems->exec();
    while(delitems->next())
    {
        if(delitems->record().value("appName").isNull())
        {
            goNextItem(delitems->record().value("ID").toInt());
            notObtain.push_back(delitems->record().value("title").toString());
            deleteEl(delitems->record().value("title").toString(), obtain);
        }
        else
        {
            notObtain.push_back(delitems->record().value("title").toString());
            deleteEl(delitems->record().value("title").toString(), obtain);
        }
    }
    return 1;
}

