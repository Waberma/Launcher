#ifndef PERSONPAGE_H
#define PERSONPAGE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QListWidgetItem>

namespace Ui {
class PersonPage;
}

class PersonPage : public QWidget
{
    Q_OBJECT

public:
    explicit PersonPage(QWidget *parent = nullptr);
    ~PersonPage();

signals:
    void statusBar(QString);
    void StartCreateMenu(QList<QString> rolelist);

private slots:
    void on_addrolepb_clicked();

    void on_deleterolepb_clicked();

    void on_editPB_clicked();

    void on_addRowPB_clicked();

    void on_CancelPB_clicked();

    void on_OkPB_clicked();

    void on_Table_clicked(const QModelIndex &index);

    void on_addWidget_itemClicked(QListWidgetItem *item);

    void on_delWidget_itemClicked(QListWidgetItem *item);

public slots:
    void SetUser(QString user)
    {
        this->user = user;
    };

private:
    Ui::PersonPage *ui;
    QSqlDatabase db;
    QSqlQuery *q;
    QSqlTableModel *persontm;
    QString action;
    QString user;
    QList<QString> obtain;
    QList<QString> notObtain;
    QString dbname;
    int row;
    bool activite = false;
    void clearAction();
    void DeleteEl(QString el, QList<QString> &list);
    QString toString(QList<QString> list);
};

#endif // PERSONPAGE_H
