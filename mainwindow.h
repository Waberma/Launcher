#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include "tablevier.h"
#include "personpage.h"
#include "rolepage.h"
#include "mainpage.h"
#include "menupage.h"
#include "pagination.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void openDB(QString);
    void MainPagesignal(QString);

public slots:
    void inStatusBar(QString info);


private slots:
    void bd_clicked();

    void on_LogOut_clicked();

    void menuActions();
    void on_tabWidget_tabCloseRequested(int index);

private:
    Ui::MainWindow *ui;
    TableVier *tv;
    //MainPage *mg;
    QSqlDatabase db;
    QSqlTableModel *tm;
    QString proff;
    QString user = "Гость";
    void SetUser(QString user)
    {
        this->user = user;
    };
    QString userpass;
    void SetUserpass(QString userpass)
    {
        this->userpass = userpass;
    };
    QMenuBar *cornerMenu;
    QMenu *menu;
    QMenu *cornermenu1;
    QMenu *cornermenu2;
    QTabWidget *tabwidget;
    PersonPage *personpage;
    MainPage *mainpage;
    RolePage *rolepage;
    Menupage *menupage;
    Pagination *pagin;
    QMenu* createMenu(int ID, QMenu *menu);
    QMenu* createMenu(int ID, QMenu *menu, QList<QString> rolelist);
    void SetCornerMenu();
    void StartCreateMenu();
    void StartCreateMenu(QList<QString> rolelist);
    void ClearWindow();
    QAction *PassChangerAction;
    QAction *RegAction;
};
#endif // MAINWINDOW_H
