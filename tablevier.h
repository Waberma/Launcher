#ifndef TABLEVIER_H
#define TABLEVIER_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
namespace Ui {
class TableVier;
}

class TableVier : public QDialog
{
    Q_OBJECT

public:
    explicit TableVier(QWidget *parent = nullptr);
    ~TableVier();

private slots:
    void on_Remove_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_Remove_2_clicked();

    void bdChose();

    //void on_pushButton_clicked();

private:
    Ui::TableVier *ui;
    QSqlDatabase db;
    QSqlQuery *q;
    QSqlTableModel *tm;
    int row;
};

#endif // TABLEVIER_H
