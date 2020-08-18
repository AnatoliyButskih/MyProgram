#include "tableview.h"
#include <QModelIndexList>

TableView::TableView(QWidget * parent):
    QTableView(parent)
{
    contextMenu=new QMenu(this);
    deleteAction=new QAction(tr("Remove from list"), this);
    deleteAction->setIcon(QIcon(":/icons/delete.png"));
    deleteAction->setIconVisibleInMenu(true);

    deleteFromDiskAction=new QAction(tr("Remove from disk"), this);
    deleteFromDiskAction->setIcon(QIcon(":/icons/delete.png"));
    deleteFromDiskAction->setIconVisibleInMenu(true);

    contextMenu->addAction(deleteAction);
    contextMenu->addAction(deleteFromDiskAction);
    connect(deleteAction,SIGNAL(triggered()),this,SLOT(selectDeleteAction()));
    connect(deleteFromDiskAction,SIGNAL(triggered()),this,SLOT(selectDeleteFromDiskAction()));
}

void TableView::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu->exec(event->globalPos());
}

void TableView::selectDeleteAction()
{
    QModelIndexList selectedList=this->selectionModel()->selectedRows();
    emit removeSelected(selectedList);
}

void TableView::selectDeleteFromDiskAction()
{
    QModelIndexList selectedList=this->selectionModel()->selectedRows();
    emit removeSelectedFromHdd(selectedList);
}


