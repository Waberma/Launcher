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
//Пагинация


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


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
    ui->tabWidget->setTabBarAutoHide(true);
    ui->tabWidget->setTabsClosable(true);

    mainpage = new MainPage;
    personpage = new PersonPage;
    rolepage = new RolePage;
    menupage = new Menupage;
    pagin = new Pagination;

    //Связи с RolePage
    connect(rolepage, &RolePage::statusBar, this, &MainWindow::inStatusBar);
    connect(rolepage, &RolePage::StartCreateMenu, this,  static_cast<void (MainWindow::*)(QList<QString> rolelist)> (&MainWindow::StartCreateMenu));
    connect(mainpage,  &MainPage::setUser, rolepage, &RolePage::SetUser);

    //Связи с PersonPage
    connect(personpage, &PersonPage::statusBar, this, &MainWindow::inStatusBar);
    connect(personpage, &PersonPage::StartCreateMenu, this,  static_cast<void (MainWindow::*)(QList<QString> rolelist)> (&MainWindow::StartCreateMenu));
    connect(mainpage,  &MainPage::setUser, personpage, &PersonPage::SetUser);

    //Связи с MainPage
    connect(this, &MainWindow::MainPagesignal, mainpage, &MainPage::menuActions);
    connect(mainpage, &MainPage::statusBar, this, &MainWindow::inStatusBar);
    connect(mainpage, &MainPage::setUser, this, &MainWindow::SetUser);
    connect(mainpage, &MainPage::setUserpass, this, &MainWindow::SetUserpass);
    connect(mainpage, &MainPage::SetCornerMenu, this, &MainWindow::SetCornerMenu);
    connect(mainpage, &MainPage::StartCreateMenu, this,  static_cast<void (MainWindow::*)(QList<QString> rolelist)> (&MainWindow::StartCreateMenu));

    //Связи с MenuPage
    connect(mainpage,  &MainPage::setUser, menupage, &Menupage::SetUser);
    connect(menupage, &Menupage::statusBar, this, &MainWindow::inStatusBar);
    connect(menupage, &Menupage::StartCreateMenu, this,  static_cast<void (MainWindow::*)(QList<QString> rolelist)> (&MainWindow::StartCreateMenu));

    ui->tabWidget->addTab(mainpage,"Главная страница");

    SetCornerMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bd_clicked()
{
    tv = new TableVier;
    tv->show();
}

void MainWindow::on_LogOut_clicked()
{
    user = "Гость";
    ui->menubar->clear();
    ui->menubar->cornerWidget()->close();
    SetCornerMenu();
}

void MainWindow::menuActions()
{
    QAction *action = (QAction *) sender();
    emit MainPagesignal(action->text());
    if(action->text() == "Автор")
    {
        bd_clicked();
    }
    if(action->text() == "Выход")
    {
        on_LogOut_clicked();
    }
    if(action->text() == "Пользователи")
    {
        ui->tabWidget->addTab(personpage,"Пользователи");
        ui->tabWidget->setCurrentWidget(personpage);
    }
    if(action->text() == "Роли")
    {
        ui->tabWidget->addTab(rolepage,"Роли");
        ui->tabWidget->setCurrentWidget(rolepage);
    }
    if(action->text() == "Объекты меню")
    {
        ui->tabWidget->addTab(menupage,"Объекты меню");
        ui->tabWidget->setCurrentWidget(menupage);
    }
    if(action->text() == "Таблица")
    {
        ui->tabWidget->addTab(pagin, "Таблица закупок");
        ui->tabWidget->setCurrentWidget(pagin);
    }
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

void MainWindow::inStatusBar(QString info)
{
    ui->statusbar->setVisible(true);
    ui->statusbar->showMessage(info);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}
