#include "menupage.h"
#include "ui_menupage.h"
#include "qlistmodel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QAction>
#include <QSqlRecord>
#include <QCommonStyle>
#include <QTreeWidget>

QString Menupage::toString(QList<QString> list)
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

void Menupage::deleteEl(QString el, QList<QString> &list)
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

Menupage::Menupage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menupage)
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

/*    menutm = new QSqlQueryModel;
    menutm->setQuery("SELECT ID, title FROM MenuItems "
                     "WHERE Role = 1;");
    ui->Table->setModel(menutm);*/
    on_CancelPB_clicked();

    QSqlQuery *treeq = new QSqlQuery(db);
    treeq->exec("SELECT ID, parent, title, AppName FROM MenuItems "
            "WHERE parent IS NULL;");
    QTreeWidgetItem *menu = new QTreeWidgetItem(ui->Table);
    while(treeq->next())
    {
        QString a = treeq->record().value("title").toString();
        menu = new QTreeWidgetItem(ui->Table);
        menu->setText(0,treeq->record().value("title").toString());
        Next(menu, treeq->record().value("ID").toInt());
    }

    QCommonStyle style;
    ui->additempb->setIcon(style.standardIcon(QStyle::SP_ArrowUp));
    ui->additempb->setEnabled(false);

    ui->deleteitempb->setIcon(style.standardIcon(QStyle::SP_ArrowDown));
    ui->deleteitempb->setEnabled(false);


}

Menupage::~Menupage()
{
    ui->addWidget->clear();
    ui->delWidget->clear();
    delete ui;
}

//void Menupage::on_Table_clicked(const QModelIndex &index)
//{
//    if(!activite)
//    {
//        clearAction();
//        ui->addWidget->clear();
//        firstobtain.clear();
//        row = index.row();
//        ui->editPB->setVisible(true);
//        QSqlQuery *updateq = new QSqlQuery(db);
//        updateq->prepare("Update RoleActions "
//                         "SET Active = true "
//                         "WHERE Role IN(Select name "
//                         "FROM Role "
//                         "LEFT JOIN RoleItems "
//                         "  ON Role.ID = RoleItems.RoleID "
//                         "WHERE ItemID = :ID); ");
//        updateq->bindValue(":ID",takeID());
//        updateq->exec();


//        QSqlQuery *addroleq = new QSqlQuery(db);
//        addroleq->exec("SELECT Role FROM RoleActions "
//                       "WHERE Active = 1");

//        while(addroleq->next())
//        {
//            firstobtain.push_back(addroleq->record().value("Role").toString());
//        }
//        ui->addWidget->addItems(firstobtain);
//    }
//}

void Menupage::on_addRowPB_clicked()
{
    menutm->insertRow(menutm->rowCount());
}

void Menupage::on_editPB_clicked()
{
    ui->verticalSpacer->changeSize(0,0);
    activite = true;
    ui->editPB->setVisible(false);
    ui->OkPB->setVisible(true);
    ui->delWidget->setVisible(true);
    QString b;
//    QSqlQuery q(db);
//    q.prepare("SELECT ID, title FROM MenuItems "
//              "WHERE Role = 1 AND ID = :ID");
//    q.bindValue(":ID", menutm->record(row).value("ID").toInt());
//    q.exec();
//    menutm->setQuery(q);
    ui->editPB->setVisible(false);
    ui->CancelPB->setVisible(true);

    QSqlQuery *deleteroleq = new QSqlQuery(db);
    deleteroleq->exec("SELECT Role FROM RoleActions "
                   "WHERE Active = 0");

    while(deleteroleq->next())
    {
        firstnotObtain.push_back(deleteroleq->record().value("Role").toString());
    }
    ui->delWidget->addItems(firstnotObtain);
    ui->addRowPB->setVisible(false);
    ui->additempb->setVisible(true);
    ui->deleteitempb->setVisible(true);
    statusBar("Выберите элемент меню для начала работы!");
}

void Menupage::on_OkPB_clicked()
{
    activite = false;
    ui->verticalSpacer->changeSize(20,175);
    activite = false;

    QSqlQuery *editrolesq = new QSqlQuery(db);
    QSqlQuery *roleIDq = new QSqlQuery(db);
    while(!secondobtain.isEmpty())
    {
        QList<int> newItems;
        newItems = menuObtain;
        roleIDq->exec("SELECT ID FROM Role "
                      "WHERE name = '" + *secondobtain.begin() +"';");
        roleIDq->next();

        QSqlQuery *menuitemq = new QSqlQuery(db);
        menuitemq->prepare("SELECT ItemID FROM RoleItems "
                           "WHERE RoleID = :RoleID;");
        menuitemq->bindValue(":RoleID", roleIDq->record().value("ID").toInt());
        menuitemq->exec();

        while(menuitemq->next())
        {
            newItems << menuitemq->record().value("ItemID").toInt();
        }

        newItems = newItems.toSet().toList();
        editrolesq->prepare("DELETE FROM RoleItems "
                        "WHERE RoleID = :RoleID;");
        editrolesq->bindValue(":RoleID", roleIDq->record().value("ID"));
        editrolesq->exec();
        while(!newItems.isEmpty())
        {
            editrolesq->prepare("INSERT INTO RoleItems (ItemID, RoleID) "
                                "VALUES (:ItemID, :RoleID);");
            editrolesq->bindValue(":RoleID", roleIDq->record().value("ID").toInt());
            editrolesq->bindValue(":ItemID", *newItems.begin());
            editrolesq->exec();
            newItems.pop_front();
        }
        secondobtain.pop_front();

    }

    while(!secondnotObtain.isEmpty())
    {
        roleIDq->exec("SELECT ID FROM Role "
                      "WHERE name = '" + *secondnotObtain.begin() +"';");
        roleIDq->next();
        for(auto el : menunotObtain)
        {
            QSqlQuery *editq = new QSqlQuery(db);

            editq->prepare("DELETE FROM RoleItems "
                                "WHERE RoleID = :RoleID AND ItemID = :ItemID;");
            editq->bindValue(":RoleID", roleIDq->record().value("ID").toInt());
            editq->bindValue(":ItemID", el);
            editq->exec();
        }
        secondnotObtain.pop_front();
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
    firstobtain.clear();
    firstnotObtain.clear();
    on_CancelPB_clicked();
}

void Menupage::on_CancelPB_clicked()
{
    ui->verticalSpacer->changeSize(20,175);
    activite = false;
    clearAction();
    row = 0;
//    menutm->setQuery("SELECT ID, title FROM MenuItems "
//                     "WHERE Role = 1;");
    ui->editPB->setVisible(false);
    ui->additempb->setVisible(false);
    ui->deleteitempb->setVisible(false);
    ui->addRowPB->setVisible(true);
    ui->CancelPB->setVisible(false);
    ui->delWidget->setVisible(false);
    ui->addWidget->setVisible(true);
    ui->addWidget->clear();
    ui->delWidget->clear();
    firstobtain.clear();
    firstnotObtain.clear();
    ui->line->setVisible(false);
    ui->line_2->setVisible(false);
    ui->OkPB->setVisible(false);
}

void Menupage::clearAction()
{
    QSqlQuery *updateq = new QSqlQuery(db);
    updateq->exec("Update RoleActions "
                  "SET Active = false;");
}

void Menupage::on_additempb_clicked()
{
    ui->delWidget->setVisible(true);
    QListWidgetItem *item = ui->delWidget->currentItem();

    firstobtain.push_back(item->text());
    secondobtain.push_back(item->text());

    deleteEl(item->text(), firstnotObtain);
    deleteEl(item->text(), secondnotObtain);
    showLists();

    QSqlQuery *additems = new QSqlQuery(db);
    additems->exec("SELECT ID, parent, title "
                   "FROM MenuItems "
                   "WHERE title = '" + this->item + "';");
    additems->next();
    menuObtain.push_back(additems->record().value("ID").toInt());
    int parent = additems->record().value("parent").toInt();
    if(!additems->record().value("parent").isNull())
    {
        QSqlQuery *addparent = new QSqlQuery(db);
        do{
            addparent = new QSqlQuery(db);
            addparent->prepare("Select ID, parent "
                               "FROM MenuItems "
                               "WHERE ID = :ID AND Role IS NOT NULL;");
            addparent->bindValue(":ID", parent);
            addparent->exec();
            addparent->next();
            if(!addparent->record().value("ID").isNull())
            {
                menuObtain.push_back(addparent->record().value("ID").toInt());
            }
            parent = addparent->record().value("parent").toInt();
        }while(!addparent->record().value("parent").isNull());
        ui->additempb->setEnabled(false);
    }
}

void Menupage::on_deleteitempb_clicked()
{
    QListWidgetItem *item = ui->addWidget->currentItem();
    firstnotObtain.push_back(item->text());
    secondnotObtain.push_back(item->text());

    deleteEl(item->text(), firstobtain);
    deleteEl(item->text(), secondobtain);

    QSqlQuery *delitems = new QSqlQuery(db);
    delitems->exec("SELECT ID, title, appName "
                   "FROM MenuItems "
                   "WHERE title = '" + this->item + "';");
    while(delitems->next())
    {
        if(delitems->record().value("appName").isNull())
        {
            goNextItem(delitems->record().value("ID").toInt());
            menunotObtain.push_back(delitems->record().value("ID").toInt());
        }
        else
        {
            menunotObtain.push_back(delitems->record().value("ID").toInt());
        }
    }
    showLists();

    ui->deleteitempb->setEnabled(false);
}

void Menupage::on_addWidget_itemClicked(QListWidgetItem *item)
{
    ui->deleteitempb->setEnabled(true);
}

void Menupage::on_delWidget_itemClicked(QListWidgetItem *item)
{
    ui->additempb->setEnabled(true);
}

void Menupage::showLists()
{
    firstobtain.removeDuplicates();
    firstnotObtain.removeDuplicates();
    ui->delWidget->clear();
    ui->delWidget->addItems(firstnotObtain);
    ui->addWidget->clear();
    ui->addWidget->addItems(firstobtain);
}

bool Menupage::goNextItem(int parent)
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
            menunotObtain.push_back(delitems->record().value("ID").toInt());
        }
        else
        {
            menunotObtain.push_back(delitems->record().value("ID").toInt());
        }
    }
    return 1;
}

void Menupage::Next(QTreeWidgetItem *twitem, int ID)
{
    QSqlQuery *treeq = new QSqlQuery(db);
    treeq->prepare("SELECT ID, title, parent, AppName "
                "FROM MenuItems "
                "WHERE Role = 1 AND parent = :ID");
    treeq->bindValue(":ID", ID);
    treeq->exec();

    while(treeq->next())
    {
        QString a = treeq->record().value("title").toString();
        if(treeq->record().value("AppName").isNull())
        {
            QTreeWidgetItem *action = new QTreeWidgetItem(twitem);
            action->setText(0, treeq->record().value("title").toString());
            Next(action,treeq->record().value("ID").toInt());
        }
        else
        {
            QTreeWidgetItem *action = new QTreeWidgetItem(twitem);
            action->setText(0, treeq->record().value("title").toString());
        }
    }
    return;
}

void Menupage::on_Table_itemClicked(QTreeWidgetItem *item, int column)
{
    this->item = item->text(column);
    if(!activite)
    {
        clearAction();
        ui->addWidget->clear();
        firstobtain.clear();
        ui->editPB->setVisible(true);
        QSqlQuery *updateq = new QSqlQuery(db);
        updateq->prepare("Update RoleActions "
                         "SET Active = true "
                         "WHERE Role IN(Select name "
                         "FROM Role "
                         "LEFT JOIN RoleItems "
                         "  ON Role.ID = RoleItems.RoleID "
                         "WHERE ItemID = :ID); ");
        updateq->bindValue(":ID",takeID());
        updateq->exec();


        QSqlQuery *addroleq = new QSqlQuery(db);
        addroleq->exec("SELECT Role FROM RoleActions "
                       "WHERE Active = 1");

        while(addroleq->next())
        {
            firstobtain.push_back(addroleq->record().value("Role").toString());
        }
        ui->addWidget->addItems(firstobtain);
    }
}

int Menupage::takeID()
{
    QSqlQuery *titleq = new QSqlQuery(db);
    QString a = item;
    titleq->exec("SELECT ID "
                 "FROM MenuItems "
                 "WHERE title = '" + item + "';");
    titleq->next();

    return titleq->record().value("ID").toInt();

}
