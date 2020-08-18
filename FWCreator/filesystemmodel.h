#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QIcon>
#include <QFile>
#include <QDebug>
#include "fileitem.h"
#include "zipfileitem.h"

/*структура содержащая информацию о файлах*/
struct DirInfo{
    DirInfo(quint64 folder_size = 0, quint64 total_elements = 0) : folder_size(folder_size), total_elements(total_elements){}
    uint64_t folder_size;//размер
    uint64_t total_elements;//общее количество элементов
};

extern int m3;

class FileSystemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    FileSystemModel();
    QVariant data(const QModelIndex &index, int role) const;
    bool setData( const QModelIndex& index, const QVariant& value, int role );
    Qt::ItemFlags flags( const QModelIndex & index ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    //void addFile(QString filePath);//в модель добавляется файл
    void addDir(QString dirPath);
    void addZipStructure(ZipFileItem * zip_item);
    void addConfirmList(QList<QString> confirm);
    void addConfirmFile(QString file_path);
    void clear();

    quint64 getTotalSize();
    quint64 getTotalFile();

    static const int NAME_COLUMN=0;
    static const int COUNT_COLUMN=1;
    static const int SIZE_COLUMN=2;
    static const int DATE_COLUMN=3;
    static const int COLUMN=4;


signals:
    void warningFile(const QString &filename);
    void incorrectFilePath(QString name);
    void selectFile(qint64 id,const FileItem * fileItem);
    void selectFile(const QModelIndex & index);

public slots:
    void setCheckState(const QModelIndex& index, const QVariant& value);
    void removeSelectFromHdd(const QModelIndex &index);
    QString checkWarnings();

private slots:
#ifdef HAVE_QT5
    void reset();
#endif
    DirInfo recursiveAddDir(QString dir_path, FileItem *parent);
    bool isMarked(const QString text) const;
    bool isWarning(const QString filename) const;
    QString containsWarning(FileItem *item) const;

private:
    void setConfirmToParent(const QModelIndex& parent, int flag);//просталение  флагов подтвержения на родителя
    void setConfirmToBranch(const QModelIndex& parent, int flag);//проставление флагов подтверждения на дочерние ноды
    FileItem* rootItem;
    QIcon folder_ico;
    QIcon binary_ico;
    QIcon script_ico;
    QIcon config_ico;
    QIcon log_ico;
    QIcon unknown_ico;
};

#endif // FILESYSTEMMODEL_H
