#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
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

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = nullptr);
    ~MainPage();

signals:
    void statusBar(QString);
    void setUser(QString);
    void setUserpass(QString);
    void SetCornerMenu();
    //void StartCreateMenu();
    void StartCreateMenu(QList<QString> rolelist);


public slots:
    void on_LoginButt_clicked();

    void on_ChangePass_clicked();

    void on_RegButt_clicked();

    void on_ChangePassOk_clicked();

    void on_AutoOk_clicked();

    void on_RegistOk_clicked();

    void menuActions(QString action);

//    void on_UserChange_clicked();

private:
    Ui::MainPage *ui;
    QSqlDatabase db;
    QString proff;
    QString user = "Гость";
    QString userpass;
    void ClearWindow();
    QAction *PassChangerAction;
    QAction *RegAction;
};

#endif // MAINPAGE_H
