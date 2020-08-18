#ifndef CONFIRMLISTMODEL_H
#define CONFIRMLISTMODEL_H

#include <QAbstractTableModel>
#include "fileitem.h"
#include <QIcon>

struct ConfirmFile{
    qint64 fileID;
    QString filePath;
    FileItem * fileItem;
};
class ConfirmListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ConfirmListModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    const QList<ConfirmFile> & getConfirmList();
    void clear();//очистка от данных

    const static int COLUMN=1;
    const static int NAME_COLUMN=0;

signals:
    void changeCheckState(const QModelIndex& index, const QVariant& value);
    void sendRemoveSelectFromHdd(const QModelIndex& index);
    void countConfirmList(const int count);

public slots:
    void addSelectFile(const QModelIndex & index);
    void removeSelected(const QModelIndexList & removeList);
    void removeSelectedFromHdd(const QModelIndexList &removeList);

private slots:
#ifdef HAVE_QT5
    void reset();
#endif

private:
    QList<ConfirmFile>::Iterator isRecordExist(qint64 id);
    QList<ConfirmFile> fileList;
    QIcon binary_ico;
    QIcon script_ico;
    QIcon config_ico;
    QIcon log_ico;
    QIcon unknown_ico;
};

#endif // CONFIRMLISTMODEL_H
