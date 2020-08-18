#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QModelIndexList>

class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget * parent=0);

protected:
    void contextMenuEvent ( QContextMenuEvent * event );

signals:
    void removeSelected(const QModelIndexList & removeList);
    void removeSelectedFromHdd(const QModelIndexList & removeList);

private slots:
    void selectDeleteAction();
    void selectDeleteFromDiskAction();

private:
    QMenu * contextMenu;
    QAction * deleteAction;
    QAction * deleteFromDiskAction;

};

#endif // TABLEVIEW_H
