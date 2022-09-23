#ifndef ADMINROLE_H
#define ADMINROLE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
namespace Ui {
class AdminRole;
}

class AdminRole : public QWidget
{
    Q_OBJECT

public:
    explicit AdminRole(QWidget *parent = nullptr);
    ~AdminRole();

private:
    Ui::AdminRole *ui;
    QSqlDatabase db;
    QSqlQuery *q;
    QSqlQueryModel *model;
    int row;
};

#endif // ADMINROLE_H
