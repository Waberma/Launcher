//#ifndef ADMINPAGE_H
//#define ADMINPAGE_H

//#include <QWidget>
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include <QSqlTableModel>
//#include <QListWidgetItem>

//namespace Ui {
//class AdminPage;
//}

//class AdminPage : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit AdminPage(QWidget *parent = nullptr);
//    ~AdminPage();

//signals:
//    void statusBar(QString);
//    void StartCreateMenu(QList<QString> rolelist);

//private slots:
//    void on_addRowPB_clicked();

//    void on_editPB_clicked();

//    void on_OkPB_clicked();

//    void on_CancelPB_clicked();

//    void on_addrolepb_clicked();

//    void on_deleterolepb_clicked();

//    void on_addWidget_itemClicked(QListWidgetItem *item);

//    void on_delWidget_itemClicked(QListWidgetItem *item);

//    void on_Table_clicked(const QModelIndex &index);



//public slots:
//    void SetUser(QString user)
//    {
//        this->user = user;
//    };
//private:
//    Ui::AdminPage *ui;
//    QSqlDatabase db;
//    QSqlQuery *q;
//    QSqlTableModel *persontm;
//    QString action;
//    QString user;
//    QList<QString> obtain;
//    QList<QString> notObtain;
//    QString dbname;
//    int row;
//    void clearAction();
//};

//#endif // ADMINPAGE_H
