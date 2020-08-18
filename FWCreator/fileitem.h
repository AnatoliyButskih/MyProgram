#ifndef FILEITEM_H
#define FILEITEM_H
#include <QList>
#include <QFileInfo>
#include <QVariant>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <stdint.h>

class FileItem
{
public:
  //  FileItem(QString fileName, FileItem * parent=0);
    FileItem(QString fileName, FileItem * parent=0,int fileType=IS_UNKN, const QDateTime & fileDate=QDateTime(),uint64_t fileSize=0);
    FileItem(QFileInfo info, FileItem * parent=0, int fileType=IS_UNKN);
    ~FileItem();
    void addChild(FileItem *child);//добавить дочерний элемент
    FileItem * getChild(int row) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    FileItem * getParent();
    QString getFileName();

    void setConfirmFlag(int flag);//флаг подтверждения записи
    int getConfirmFlag() const;
    bool isFile() const {
        return is_file;
    }
    void setFlagFile(const bool flag) {
        is_file = flag;
    }

    int getFileType() const;
    void setFilePath(const QString path);
    QString getFilePath() const {
        return path;
    }

    void setFileSize(uint64_t fileSize);
    uint64_t getFileSize();

    void setTotalChild(uint64_t totalChild);
    uint64_t getTotalChild() const;

    const QDateTime & getTimeInfo();

    void clear();
    FileItem * isItemExist(QString name);//проверяет существует ли дочерняя нода в данном с именем name

    void lessChild(const int row) {
        childItems.removeAt(row);
        totalChild--;
    }
/*Типы файлов*/
    static const int IS_DIR     = 0x11;//файл является директорией
    static const int IS_BIN     = 0x12;//файл является бинарным
    static const int IS_SCRIPT  = 0x13;//файл является shell скриптом
    static const int IS_CFG     = 0x14;//файл является конфигурационным
    static const int IS_LOG     = 0x15;//файл является логом
    static const int IS_UNKN    = 0x16;//тип файла неизвестен

/*состояние выбора*/
    static const int CHILD_SELECT   = 0x51;//выбран дочерний элемент
    static const int THIS_SELECT    = 0x52;//выбран данный элемент
    static const int THIS_UNSELECT  = 0x53;//элемент не выбран

public slots:
    void removeFromParent(const int row);
    bool isWarning() const {
        return warning;
    }
    void setWarning(const bool warning) {
        this->warning = warning;
    }

private:

    bool is_file;
    QList<FileItem*> childItems;//дочерние ноды
    int fileType;//тип файла
    QString path;   //полный путь
    QString fileName;//имя файла
    int state;//флаг состояния если флаг равен THIS_SELECT то обновление данных файлов необходимо подтвердить
    QDateTime fileDate;//временные прараметры файла

    uint64_t fileSize;//размер файла
    uint64_t totalChild;//общее количество дочерних элементов
    bool warning;

    FileItem *parentItem;//родительская нода
};

#endif // FILEITEM_H
