#ifndef ROLEPAGE_H
#define ROLEPAGE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QListWidgetItem>

namespace Ui {
class RolePage;
}

class RolePage : public QWidget
{
    Q_OBJECT

public:
    explicit RolePage(QWidget *parent = nullptr);
    ~RolePage();

signals:
    void statusBar(QString);
    void StartCreateMenu(QList<QString> rolelist);


private slots:
    void on_addWidget_itemClicked(QListWidgetItem *item);

    void on_delWidget_itemClicked(QListWidgetItem *item);

    void on_additempb_clicked();

    void on_deleteitempb_clicked();

    void on_editPB_clicked();

    void on_CancelPB_clicked();

    void on_OkPB_clicked();

    void on_Table_clicked(const QModelIndex &index);

    void on_addRowPB_clicked();

public slots:
    void SetUser(QString user)
    {
        this->user = user;
    };

private:
    Ui::RolePage *ui;
    QSqlDatabase db;
    QSqlQuery *q;
    QSqlTableModel *roletm;
    QString action;
    QString user;
    QList<QString> obtain;
    QList<QString> notObtain;
    QString dbname;
    int row;
    bool activite;
    void clearAction();
    void deleteEl(QString el, QList<QString> &list);
    void showLists();
    bool goNextItem(int parent);
    QString toString(QList<QString> list);
};

#endif // ROLEPAGE_H
