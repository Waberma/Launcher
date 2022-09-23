#ifndef PAGINATION_H
#define PAGINATION_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QListWidgetItem>
#include <QThread>
#include <thread>



class NewElementsthr : public QObject
{
    Q_OBJECT
public:
    explicit NewElementsthr(QObject *parent = 0) {};
    ~NewElementsthr() {};
signals:
    void Setmap(QMap<int,QString> pages);
public slots:
    void GetNewNextEl(int startindex, int endindex, int step);


private:
    QMap<int,QString> pages;
};

//class PreviousThread : public QObject
//{
//    Q_OBJECT
//public:
//    explicit PreviousThread(QObject *parent = 0);
//public slots:
//    void GetPreviousEl(int startindex, int endindex, int step, QMap<int,QString> &map)
//    {
//        for(startindex; startindex < endindex; --startindex)
//        {
//            QString treadpocketmoel = "SELECT * FROM Pagination "
//                        "LIMIT :startindex, 30;";
//            treadpocketmoel.replace(":startindex", QString::number(startindex*step));
//            map.insert(startindex,treadpocketmoel);
//        }
//    }
//};

namespace Ui {
class Pagination;
}

class Pagination : public QWidget
{
    Q_OBJECT

public:
    explicit Pagination(QWidget *parent = nullptr);
    ~Pagination();
    //void GetNewNextEl(int startindex, int endindex);
    //void GetNewPreviousEl(int startindex, int endindex);

public slots:
    void SetMap(QMap<int,QString> newmap)
    {
        pages = newmap;
    };
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

signals:
    void GetNewNextEl(int startindex, int endindex, int step);
private:
    Ui::Pagination *ui;
    QSqlDatabase db;
    QSqlQueryModel *queryview;
    int total;
    int step;
    int previousindex = 1;
    NewElementsthr *nextpagethr;
//    PreviousThread *previouspagethr;
    QMap<int,QString> pages;
    QThread *thread;
};

#endif // PAGINATION_H
