#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QTextStream>
#include <QList>
#include <QFunctionPointer>
#include <QAction>
#include <QSqlRecord>
#include <QTabWidget>
//Модели и интервью
//



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setTabPosition(QTabWidget::South);

    //ui->formLayout->addWidget(tabWidget);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./menu.db");
    if(db.open())
    {
        qDebug("DB Open");
    }
    else
    {
        qDebug("NoOpen");
    }

    ui->UserProfil->setVisible(true);
    ui->ChangePassBox->setVisible(false);
    ui->RegistBox->setVisible(false);
    ui->AutoBox->setVisible(false);
    ui->AddRole->setVisible(false);
    ui->DeleteRole->setVisible(false);
    SetCornerMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_LoginButt_clicked()
{
        ui->AutoBox->setVisible(true);
        ui->RegistBox->setVisible(false);
}

void MainWindow::bd_clicked()
{
    tv = new TableVier;
    tv->show();
}

void MainWindow::on_LogOut_clicked()
{
    ui->DeleteRole->setVisible(false);
    ui->AddRole->setVisible(false);
    ui->ChangePassBox->setVisible(false);
    user = "Гость";
    userpass.clear();
    ui->menubar->clear();
    ui->menubar->cornerWidget()->close();
    SetCornerMenu();
}

void MainWindow::on_ChangePass_clicked()
{
    ui->ChangePassBox->setVisible(true);
    ui->RegistBox->setVisible(false);
    ui->AutoBox->setVisible(false);
}

void MainWindow::on_ChangePassOk_clicked()
{
    db.open();
    if(ui->PreviousPass->text() == userpass && ui->NewPass1->text() == ui->NewPass2->text())
    {
        //Смена данных в базе
        db.transaction();
        QSqlQuery *passChangeq = new QSqlQuery(db);
        passChangeq->prepare("UPDATE Person SET Pass = :PASS "
                      "WHERE Login = :LOGIN");
        passChangeq->bindValue(":LOGIN", user);
        passChangeq->bindValue(":PASS", ui->NewPass2->text());
        passChangeq->exec();

        //Работа с окном
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Пароль изменен");
        userpass = ui->NewPass2->text();
        ui->PreviousPass->clear();
        ui->NewPass1->clear();
        ui->NewPass2->clear();
        db.commit();
    }
}

void MainWindow::on_AutoOk_clicked()
{
    QString login = ui->AutoLog->text();
    QString pass = ui->AutoPass->text();
    QSqlQuery *autoq = new QSqlQuery(db);
    bool loginCheak = false;
    bool passCheak= false;

    //Проверка данных
    autoq->prepare("SELECT * FROM Person "
                 "WHERE LOGIN = :LOGIN;");
    autoq->bindValue(":LOGIN",login);
    autoq->exec();
    autoq->next();
    if(login == autoq->value(autoq->record().indexOf("Login")).toString())
    {
        loginCheak = true;
        if(pass == autoq->value(autoq->record().indexOf("Pass")).toString())
        {
            passCheak = true;
        }
    }

    //Исходы входа
    if(passCheak && loginCheak)
    {
        QSqlQuery *roleq = new QSqlQuery(db);
        roleq->prepare("SELECT RoleID "
                   "FROM RolePerson "
                   "WHERE PersonID = :ID;");
        roleq->bindValue(":ID", autoq->value(autoq->record().indexOf("ID")).toInt());
        roleq->exec();
        QList<QString> listRoles;
        while(roleq->next())
        {
            listRoles.push_back(roleq->value(roleq->record().indexOf("RoleID")).toString());
        }
        StartCreateMenu(listRoles);
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Вход выполнен");
        user = login;
        userpass = pass;
        SetCornerMenu();
        ui->AutoBox->setVisible(false);
        ui->UserProfil->setVisible(true);
        ui->AutoLog->clear();
        ui->AutoPass->clear();

    }else if(!passCheak && loginCheak)
    {
        ui->statusbar->showMessage("Неверный пароль");
    }else if(!loginCheak)
    {
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Пользователя с таким логином не существует");
    }
}

void MainWindow::on_RegButt_clicked()
{
    ui->AutoBox->setVisible(false);
    ui->RegistBox->setVisible(true);
    ui->ChangePassBox->setVisible(false);
}

void MainWindow::on_RegistOk_clicked()
{
    db.open();
    QSqlQuery *registq = new QSqlQuery(db);
    bool loginCheak = false;
    registq->exec("SELECT Login FROM Person");
    while(registq->next())
    {
        QString blogin = registq->value(registq->record().indexOf("Login")).toString();
        if(blogin == ui->Registlog->text())
        {
            loginCheak = true;
            break;
        }
    }


    if(!loginCheak && ui->RegPass1->text() == ui->RegPass2->text())
    {
        registq->prepare("INSERT INTO Person(Login, Pass) "
                      "VALUES(:LOGIN, :PASS); ");
        registq->bindValue(":LOGIN", ui->Registlog->text());
        registq->bindValue(":PASS", ui->RegPass2->text());
        registq->exec();
        ui->RegPass1->clear();
        ui->Registlog->clear();
        ui->RegPass2->clear();
        ui->AutoBox->setVisible(true);
        ui->RegistBox->setVisible(false);
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Регистрация завершена");
    }else if(loginCheak)
    {
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Логин занят");
    }else if(ui->RegPass1->text() != ui->RegPass2->text())
    {
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Пароли не совпадают");
    }
}

//void MainWindow::calendar_clicked()
//{
//    if(ui->calendar->isVisible())
//    {
//        ui->calendar->setVisible(false);
//    }
//    else
//    {
//        ui->calendar->setVisible(true);
//    }

//}

void MainWindow::menuActions()
{
    QAction *action = (QAction *) sender();
    ClearWindow();
    if(action->text() == "Календарь")
    {
        //calendar_clicked();
    }
    if(action->text() == "Автор")
    {
        bd_clicked();
    }
    if(action->text() == "Регистрация")
    {
        on_RegButt_clicked();
    }
    if(action->text() == "Регистрация")
    {
        on_RegButt_clicked();
    }
    if(action->text() == "Выход")
    {
        on_LogOut_clicked();
    }
    if(action->text() == "Сменить пароль")
    {
            on_ChangePass_clicked();
    }
    if(action->text() == "Сменить пользователя")
    {
            on_LoginButt_clicked();
    }
    if(action->text() == "Добавить роль")
    {
        ui->DeleteRole->setVisible(false);
        ui->ChangePassBox->setVisible(false);
        ui->AddRole->setVisible(true);
    }
    if(action->text() == "Удалить роль")
    {
            ui->DeleteRole->setVisible(true);
    }
}

void MainWindow::on_UserChange_clicked()
{
    on_LoginButt_clicked();
}

QMenu* MainWindow::createMenu(int ID, QMenu *menu)
{
    QSqlQuery *menuq = new QSqlQuery(db);
    menuq->prepare("SELECT ID, title, parent, appName "
                    "FROM MenuItems "
                    "WHERE parent = :ID AND (Role IS NULL OR Role = '');");
    menuq->bindValue(":ID",ID);
    menuq->exec();

    QSqlRecord rec = menuq->record();
    const int titleIndex = rec.indexOf("title");
    const int appNameIndex = rec.indexOf("appName");
    const int IDIndex = rec.indexOf("ID");

    while(menuq->next())
    {
        if(menuq->value(appNameIndex).isNull() || menuq->value(appNameIndex).toString() == "")
        {
                QMenu *menu2 = new QMenu(menuq->value(titleIndex).toString());
                menu->addMenu(createMenu(menuq->value(IDIndex).toInt(), menu2));
        }
        else
        {
                QAction *action = new QAction(menuq->value(titleIndex).toString(), this);
                connect(action, SIGNAL(triggered()), this, SLOT(menuActions()));
                menu->addAction(action);
        }
    }
    return menu;
}

void MainWindow::SetCornerMenu()
{

    if(user != "Гость")
    {
        ui->menubar->cornerWidget()->close();
        QSqlQuery *cornerMenuq = new QSqlQuery(db);
        cornerMenuq->exec("SELECT * FROM CornerMenu "
                          "WHERE Active IS NOT NULL;");
        cornerMenu = new QMenuBar(ui->menubar);
        cornermenu1 = new QMenu("Добро пожаловать, "+user,this);
        while(cornerMenuq->next())
        {
            QAction *userAction = new QAction(cornerMenuq->value(cornerMenuq->record().indexOf("Name")).toString(),this);
            connect(userAction, SIGNAL(triggered()), this, SLOT(menuActions()));
            cornermenu1->addAction(userAction);
        }
        cornerMenu->addMenu(cornermenu1);
        ui->menubar->setCornerWidget(cornerMenu);
    }
    else
    {
        QSqlQuery *cornerMenuq = new QSqlQuery(db);
        cornerMenuq->exec("SELECT * FROM CornerMenu "
                          "WHERE Active IS NULL;");
        cornerMenu = new QMenuBar(ui->menubar);
        cornermenu1 = new QMenu("Добро пожаловать, "+user,this);
        while(cornerMenuq->next())
        {
            QAction *userAction = new QAction(cornerMenuq->value(cornerMenuq->record().indexOf("Name")).toString(),this);
            connect(userAction, SIGNAL(triggered()), this, SLOT(menuActions()));
            cornermenu1->addAction(userAction);
        }
        cornerMenu->addMenu(cornermenu1);
        ui->menubar->setCornerWidget(cornerMenu);
    }
    ui->menubar->cornerWidget()->show();
}

QMenu* MainWindow::createMenu(int ID, QMenu *menu, QList<QString> rolelist)
{
    createMenu(ID,menu);
    QSqlQuery *roleMenuq = new QSqlQuery(db);
    QString role;
    QString comma;
    for(auto i = rolelist.begin(); i != rolelist.end(); ++i)
    {
        role += comma + *i;
        comma = ",";
    }
    roleMenuq->prepare("Select DISTINCT MenuItems.ID, MenuItems.Title, MenuItems.parent, MenuItems.AppName "
              "FROM MenuItems "
              "LEFT JOIN RoleItems "
                "ON MenuItems.ID = RoleItems.ItemID "
              "WHERE RoleItems.RoleID IN (" + role + ") AND parent = :ID;");
    roleMenuq->bindValue(":ID", ID);
    roleMenuq->exec();
    QSqlRecord rec = roleMenuq->record();
    int titleIndex = rec.indexOf("title");
    int appNameIndex = rec.indexOf("appName");
    int IDIndex = rec.indexOf("ID");
    while(roleMenuq->next())
    {
        if(roleMenuq->value(roleMenuq->record().indexOf("appName")).isNull() || roleMenuq->value(appNameIndex).toString() == "")
        {
                QMenu *menu2 = new QMenu(roleMenuq->value(titleIndex).toString());
                menu->addMenu(createMenu(roleMenuq->value(IDIndex).toInt(), menu2, rolelist));
        }
        else
        {
                QAction *action = new QAction(roleMenuq->value(titleIndex).toString(), this);
                connect(action, SIGNAL(triggered()), this, SLOT(menuActions()));
                menu->addAction(action);
        }
    }
    return menu;
}

void MainWindow::StartCreateMenu()
{
    QSqlQuery *menubarq = new QSqlQuery(db);
    menubarq->exec("SELECT ID, title FROM MenuItems "
             "WHERE (parent IS NULL or parent = '') AND Role IS NULL");

    QSqlRecord rec = menubarq->record();
    const int titleIndex = rec.indexOf("title");
    const int IDIndex = rec.indexOf("ID");
    while(menubarq->next())
    {
        menu = menuBar()->addMenu(menubarq->value(titleIndex).toString());
        createMenu(menubarq->value(IDIndex).toInt(),menu);
    }
}

void MainWindow::StartCreateMenu(QList<QString> rolelist)
{
    QSqlQuery *menubarq = new QSqlQuery(db);
    menubarq->exec("SELECT ID, title FROM MenuItems "
                   "WHERE (parent IS NULL or parent = '') AND Role IS NULL");
    ui->menubar->clear();
    while(menubarq->next())
    {
        menu = menuBar()->addMenu(menubarq->value(menubarq->record().indexOf("title")).toString());
        createMenu(menubarq->value(menubarq->record().indexOf("ID")).toInt(),menu, rolelist);
    }

    QSqlQuery *additationMenuq = new QSqlQuery(db);
    QString role;
    QString comma;
    for(auto element :rolelist)
    {
        role += comma + element;
        comma = ",";
    }
    additationMenuq->exec("Select DISTINCT MenuItems.ID, MenuItems.Title, MenuItems.parent, MenuItems.AppName "
              "FROM MenuItems "
              "LEFT JOIN RoleItems "
                "ON MenuItems.ID = RoleItems.ItemID "
              "WHERE RoleItems.RoleID IN ("+role+") AND parent IS NULL;");
    while(additationMenuq->next())
    {
        menu = menuBar()->addMenu(additationMenuq->value(additationMenuq->record().indexOf("title")).toString());
        createMenu(additationMenuq->value(additationMenuq->record().indexOf("ID")).toInt(), menu, rolelist);
    }

}

void MainWindow::on_AddRoleOk_clicked()
{
    QSqlQuery *personIDq = new QSqlQuery(db);
    personIDq->prepare("SELECT ID "
                      "FROM Person "
                      "WHERE Login = :login");
    personIDq->bindValue(":login", ui->addRoleLog->text());
    personIDq->exec();
    personIDq->next();
    if(personIDq->value(personIDq->record().indexOf("ID")).toInt() <= 0)
    {
        QString a = ui->addRoleLog->text();
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Пользователя не существует");
    }
    else
    {
        QSqlQuery *roleIDq = new QSqlQuery(db);
        roleIDq->prepare("SELECT ID "
                          "FROM Role "
                          "WHERE name = :rolename");
        roleIDq->bindValue(":rolename", ui->addRoleName->text());
        roleIDq->exec();
        roleIDq->next();
        if(roleIDq->value(roleIDq->record().indexOf("ID")).toInt() <= 0)
        {
            ui->statusbar->setVisible(true);
            ui->statusbar->showMessage("Роли не существует");
        }
        else
        {
            QSqlQuery *personRolesq = new QSqlQuery(db);
            personRolesq->prepare("SELECT PersonID, RoleID "
                                  "FROM RolePerson "
                                  "WHERE PersonID = :PersonID AND RoleID = :RoleID;");
            personRolesq->bindValue(":PersonID",personIDq->value(personIDq->record().indexOf("ID")).toInt());
            personRolesq->bindValue(":RoleID", roleIDq->value(roleIDq->record().indexOf("ID")).toInt());
            personRolesq->exec();
            personRolesq->next();
            if(personRolesq->value(personRolesq->record().indexOf("Person")).toInt() > 0)
            {
                ui->statusbar->setVisible(true);
                ui->statusbar->showMessage("У пользователя уже есть эта роль");
            }
            else
            {
            QSqlQuery *addRoleIDq = new QSqlQuery(db);
            addRoleIDq->prepare("INSERT INTO RolePerson (PersonID, RoleID) "
                                "VALUES (:PersonID, :RoleID);");
            addRoleIDq->bindValue(":PersonID",personIDq->value(personIDq->record().indexOf("ID")).toInt());
            addRoleIDq->bindValue(":RoleID", roleIDq->value(roleIDq->record().indexOf("ID")).toInt());
            addRoleIDq->exec();
            ui->statusbar->setVisible(true);
            ui->statusbar->showMessage("Роль добавлена");
            }
        }
    }
    if(user == personIDq->value(personIDq->record().indexOf("Login")).toString())
    {
        QSqlQuery *roleq = new QSqlQuery(db);
        roleq->prepare("SELECT RoleID "
                   "FROM RolePerson "
                   "WHERE PersonID = :ID;");
        roleq->bindValue(":ID", personIDq->value(personIDq->record().indexOf("ID")).toInt());
        roleq->exec();
        QList<QString> listRoles;
        while(roleq->next())
        {
            listRoles.push_back(roleq->value(roleq->record().indexOf("RoleID")).toString());
        }
        StartCreateMenu(listRoles);
    }
}

void MainWindow::on_DeleteRoleOk_clicked()
{
    QSqlQuery *personIDq = new QSqlQuery(db);
    personIDq->prepare("SELECT ID, Login "
                      "FROM Person "
                      "WHERE Login = :login");
    personIDq->bindValue(":login", ui->deleteRoleLogin->text());
    personIDq->exec();
    personIDq->next();
    if(personIDq->value(personIDq->record().indexOf("ID")).toInt() <= 0)
    {
        ui->statusbar->setVisible(true);
        ui->statusbar->showMessage("Пользователя не существует");
    }
    else
    {
        QSqlQuery *roleIDq = new QSqlQuery(db);
        roleIDq->prepare("SELECT ID "
                          "FROM Role "
                          "WHERE name = :rolename");
        roleIDq->bindValue(":rolename", ui->deleteRoleName->text());
        roleIDq->exec();
        roleIDq->next();
        if(roleIDq->value(roleIDq->record().indexOf("ID")).toInt() <= 0)
        {
            ui->statusbar->setVisible(true);
            ui->statusbar->showMessage("Роли не существует");
        }
        else
        {
            QSqlQuery *personRolesq = new QSqlQuery(db);
            personRolesq->prepare("SELECT PersonID, RoleID "
                                  "FROM RolePerson "
                                  "WHERE PersonID = :PersonID AND RoleID = :RoleID;");
            personRolesq->bindValue(":PersonID",personIDq->value(personIDq->record().indexOf("ID")).toInt());
            personRolesq->bindValue(":RoleID", roleIDq->value(roleIDq->record().indexOf("ID")).toInt());
            personRolesq->exec();
            personRolesq->next();
            if(personRolesq->value(personRolesq->record().indexOf("Person")).toInt() <= 0)
            {
                ui->statusbar->setVisible(true);
                ui->statusbar->showMessage("У пользователя нет этой роли");
            }
            else
            {
            QSqlQuery *deleteRoleq = new QSqlQuery(db);
            deleteRoleq->prepare("DELETE FROM RolePerson "
                                "WHERE PersonID = :PersonID AND RoleID = :RoleID;");
            deleteRoleq->bindValue(":PersonID",personIDq->value(personIDq->record().indexOf("ID")).toInt());
            deleteRoleq->bindValue(":RoleID", roleIDq->value(roleIDq->record().indexOf("ID")).toInt());
            deleteRoleq->exec();
            ui->statusbar->setVisible(true);
            ui->statusbar->showMessage("Роль удалена");
            }
        }
    }
    if(user == personIDq->value(personIDq->record().indexOf("Login")).toString())
    {
        QSqlQuery *roleq = new QSqlQuery(db);
        roleq->prepare("SELECT RoleID "
                   "FROM RolePerson "
                   "WHERE PersonID = :ID;");
        roleq->bindValue(":ID", personIDq->value(personIDq->record().indexOf("ID")).toInt());
        roleq->exec();
        QList<QString> listRoles;
        while(roleq->next())
        {
            listRoles.push_back(roleq->value(roleq->record().indexOf("RoleID")).toString());
        }
        StartCreateMenu(listRoles);
    }
}

void MainWindow::ClearWindow()
{
    ui->AddRole->setVisible(false);
    ui->addRoleName->clear();
    ui->addRoleLog->clear();

    ui->DeleteRole->setVisible(false);
    ui->deleteRoleName->clear();
    ui->deleteRoleLogin->clear();

    ui->AutoBox->setVisible(false);
    ui->AutoLog->clear();
    ui->AutoPass->clear();

    ui->RegistBox->setVisible(false);
    ui->Registlog->clear();
    ui->RegPass1->clear();
    ui->RegPass2->clear();

    ui->ChangePassBox->setVisible(false);
    ui->PreviousPass->clear();
    ui->NewPass1->clear();
    ui->NewPass2->clear();
}
