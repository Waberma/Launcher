#include "qlistmodel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QMenuBar>
#include <QSqlRecord>

QListModel::QListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    NewTable();
}

QVariant QListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool QListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int QListModel::rowCount(const QModelIndex &parent) const
{
    return infolist.count();
}

QVariant QListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch (role) {
    case Qt::DisplayRole:
            return infolist.at(index.row());
    default: return QVariant();

    }

}

bool QListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags QListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable; // FIXME: Implement me!
}

bool QListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool QListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

void QListModel::setTable(QString tablename)
{
    this->tablename = tablename;   
    NewTable();
}

void QListModel::NewTable()
{
    infolist.clear();
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
    QSqlQuery *q = new QSqlQuery(db);
    q->exec("SELECT name FROM " + tablename);
    while(q->next())
    {
        infolist << q->value(0).toString();
    }
}
