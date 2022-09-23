#include "mainpage.h"
#include "ui_mainpage.h"
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

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
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

    ui->setupUi(this);
    ui->ChangePassBox->setVisible(false);
    ui->RegistBox->setVisible(false);
    ui->AutoBox->setVisible(false);
    ui->AddRole->setVisible(false);
    ui->DeleteRole->setVisible(false);
}

MainPage::~MainPage()
{
    delete ui;
}

void MainPage::on_LoginButt_clicked()
{
    ClearWindow();
    ui->AutoBox->setVisible(true);
}

void MainPage::on_AutoOk_clicked()
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
            listRoles.push_back(roleq->record().value("RoleID").toString());
        }
        emit StartCreateMenu(listRoles);
        emit statusBar("Вход выполнен");
        emit setUser(login);
        emit setUserpass(pass);
        emit SetCornerMenu();
        ui->AutoBox->setVisible(false);
        ui->AutoLog->clear();
        ui->AutoPass->clear();
    }else if(!passCheak && loginCheak)
    {
        emit statusBar("Неверный пароль");
    }else if(!loginCheak)
    {
        emit statusBar("Пользователя с таким логином не существует");
    }
}

void MainPage::on_ChangePass_clicked()
{
    ClearWindow();
    ui->ChangePassBox->setVisible(true);
}

void MainPage::on_ChangePassOk_clicked()
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
        emit statusBar("Пароль изменен");
        userpass = ui->NewPass2->text();
        ui->PreviousPass->clear();
        ui->NewPass1->clear();
        ui->NewPass2->clear();
        db.commit();
    }
}

void MainPage::on_RegButt_clicked()
{
    ClearWindow();
    ui->RegistBox->setVisible(true);
}

void MainPage::on_RegistOk_clicked()
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
        emit statusBar("Регистрация завершена");
    }else if(loginCheak)
    {
        emit statusBar("Логин занят");
    }else if(ui->RegPass1->text() != ui->RegPass2->text())
    {
        emit statusBar("Пароли не совпадают");
    }
}

void MainPage::menuActions(QString action)
{
    if(action == "Регистрация")
    {
        on_RegButt_clicked();
    }
    if(action == "Выход")
    {
        ClearWindow();
    }
    if(action == "Сменить пароль")
    {
            on_ChangePass_clicked();
    }
    if(action == "Сменить пользователя")
    {
            on_LoginButt_clicked();
    }
}

void MainPage::ClearWindow()
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
