#ifndef MENUPAGE_H
#define MENUPAGE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

namespace Ui {
class Menupage;
}

class Menupage : public QWidget
{
    Q_OBJECT

public:
    explicit Menupage(QWidget *parent = nullptr);
    ~Menupage();

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

    //void on_Table_clicked(const QModelIndex &index);

    void on_addRowPB_clicked();


    void on_Table_itemClicked(QTreeWidgetItem *item, int column);

public slots:
    void SetUser(QString user)
    {
        this->user = user;
    };

private:
    Ui::Menupage *ui;
    QSqlDatabase db;
    QSqlQueryModel *menutm;
    QString action;
    QString user;
    QList<QString> firstobtain;
    QList<QString> secondobtain;
    QList<QString> firstnotObtain;
    QList<QString> secondnotObtain;
    QList<int> menuObtain;
    QList<int> menunotObtain;
    QString dbname;
    int row;
    bool activite;
    void clearAction();
    void deleteEl(QString el, QList<QString> &list);
    void showLists();
    bool goNextItem(int parent);
    void Next(QTreeWidgetItem *twitem, int ID);
    QString item;
    QString toString(QList<QString> list);
    int takeID();
};

#endif // MENUPAGE_H
