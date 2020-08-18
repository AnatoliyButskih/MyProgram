#ifndef IPADDRESSLIST_H
#define IPADDRESSLIST_H

#include <QTableView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>


class IPAddressList : public QTableView
{
    Q_OBJECT
public:
    IPAddressList(QWidget * parent=0);
signals:
    void removeSelected(const QModelIndexList & removeList);
    void addIPField();
protected:
    void contextMenuEvent (QContextMenuEvent * event);
private slots:
    void removeSelectedIP();
private:
    QMenu * contextMenu;    //меню по правой кнопки мыши
    QAction * addAction;    //добавить запись IP
    QAction * deleteAction; //удалить запись IP
};

#endif // IPADDRESSLIST_H
