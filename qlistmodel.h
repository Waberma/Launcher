#ifndef QLISTMODEL_H
#define QLISTMODEL_H

#include <QAbstractListModel>
#include <QBrush>
#include <QFont>
#include <QSize>
#include <QStringList>
#include <QAction>
#include <QSqlDatabase>

class QListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit QListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void NewTable();
private:
    QList<QString> infolist;
    QString tablename = "ActionList";
    //QString tablename = "Role";
    QSqlDatabase db;

public slots:
    void setTable(QString tablename);
};

#endif // QLISTMODEL_H
