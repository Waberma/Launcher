#include "pagination.h"
#include "ui_pagination.h"
#include <QStandardItemModel>
#include <QSqlRecord>
#include <thread>

//void Pagination::GetNewNextEl(int startindex, int endindex)
//{
//    for(startindex; startindex < endindex; ++startindex)
//    {
//        QString treadpocketmoel = "SELECT * FROM Pagination "
//                    "LIMIT :startindex, 30;";
//        treadpocketmoel.replace(":startindex", QString::number(startindex*step));
//        nextpages.insert(startindex,treadpocketmoel);
//    }
//}

//void Pagination::GetNewPreviousEl(int startindex, int endindex)
//{
//    for(startindex; startindex < endindex; --startindex)
//    {
//        QString treadpocketmoel = "SELECT * FROM Pagination "
//                    "LIMIT :startindex, 30;";
//        treadpocketmoel.replace(":startindex", QString::number(startindex*step));
//        nextpages.insert(startindex,treadpocketmoel);
//        previouspages.insert(startindex,treadpocketmoel);
//    }
//}

Pagination::Pagination(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Pagination)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./menu.db");
    if(db.open())
    {
        qDebug("Open");
    }
    else
    {
        qDebug("NoOpen");
    }

    nextpagethr = new NewElementsthr();



    thread = new QThread(this);
    connect(this, SIGNAL(destroyed()), thread, SLOT(quit()));

    connect(this, &Pagination::GetNewNextEl, nextpagethr, &NewElementsthr::GetNewNextEl);
    connect(nextpagethr,&NewElementsthr::Setmap, this, &Pagination::SetMap);

    nextpagethr->moveToThread(thread);

    queryview = new QSqlTableModel(this, db);
    queryview->setQuery("SELECT * FROM Pagination;");
    ui->tableView->setModel(queryview);

    step = ui->tableView->rowHeight(0);
    total = ui->tableView->model()->rowCount();

    for(int totalpage = 1; totalpage*step < total; totalpage++)
    {
        ui->comboBox->addItem(QString::number(totalpage));
    }


//    QStandardItemModel* model = new QStandardItemModel();
//    QStringList labels = QObject::trUtf8("ID, Заказчик, Лот, Название").simplified().split(",");
//    model->setHorizontalHeaderLabels(labels);
//    Определить товар
//    QStandardItem* item = 0;
//    QSqlQuery* itemq = new QSqlQuery(db);
//    itemq->exec("SELECT * FROM Pagination");
//    for (int i = 0; itemq->next() ; ++i)
//    {
//        item = new QStandardItem(QString("%1").arg(itemq->record().value("ID").toString()));
//        model->setItem(i,0,item);
//        item = new QStandardItem(QString("%1").arg(itemq->record().value("Hoster").toString()));
//        model->setItem(i,1,item);
//        item = new QStandardItem(QString("%1").arg(itemq->record().value("Lot").toString()));
//        model->setItem(i,2,item);
//        item = new QStandardItem(QString("%1").arg(itemq->record().value("Name").toString()));
//        model->setItem(i,3,item);
//    }
//    ui->tableView->setModel(model);
//    int totalRow = ui->tableView->model()->rowCount();    //the total row
//    int rowHeight = ui->tableView->rowHeight(0);           //the row's height
//    int tableViewHeight = ui->tableView->height();                //every row's height
//    int row1page = tableViewHeight / rowHeight - 1;
//    int totalPage = totalRow / row1page;
//    if(totalRow%row1page != 0) totalPage++;
    thread->start();
}

Pagination::~Pagination()
{
    delete ui;
}

void Pagination::on_comboBox_currentIndexChanged(int index)
{
    QSqlQueryModel *pocketqueryview = new QSqlTableModel(this, db);
    if(previousindex == 0)
    {
        QString a = "SELECT * FROM Pagination "
                    "LIMIT :startindex, 30;";
        a.replace(":startindex", QString::number(index*step));
        pocketqueryview->setQuery(a);
        ui->tableView_2->setModel(pocketqueryview);
        previousindex = index;
        emit GetNewNextEl(previousindex, index+3, step);
    }
    else if(pages.count(index))
    {
        pocketqueryview->setQuery(pages.value(index));
        emit GetNewNextEl(previousindex, index, step );
    }
    else
    {
        QString a = "SELECT * FROM Pagination "
                    "LIMIT :startindex, 30;";
        a.replace(":startindex", QString::number(index*step));
        pocketqueryview->setQuery(a);
        ui->tableView_2->setModel(pocketqueryview);
        previousindex = index;
        emit GetNewNextEl(index, index+2, step);
    }
}

void NewElementsthr::GetNewNextEl(int startindex, int endindex, int step)
{
    QMap<int, QString> map;
    for(int i = startindex; startindex <= endindex; ++startindex)
    {
        QString treadpocketmoel = "SELECT * FROM Pagination "
                                  "LIMIT :startindex, 30;";
        treadpocketmoel.replace(":startindex", QString::number(i*step));
        map.insert(startindex,treadpocketmoel);
    }
    for(int i = startindex; startindex > endindex - 4; --startindex)
    {
        QString treadpocketmoel = "SELECT * FROM Pagination "
                                  "LIMIT :startindex, 30;";
        treadpocketmoel.replace(":startindex", QString::number(i*step));
        map.insert(startindex,treadpocketmoel);
    }
    emit Setmap(map);
}

void Pagination::on_pushButton_clicked()
{
    if(ui->comboBox->currentIndex() < 7)
        ui->comboBox->setCurrentIndex(ui->comboBox->currentIndex()+1);
}

void Pagination::on_pushButton_2_clicked()
{
    if(ui->comboBox->currentIndex() > 0)
        ui->comboBox->setCurrentIndex(ui->comboBox->currentIndex()-1);
}
