#include "ipaddresslist.h"
#include <QHeaderView>
IPAddressList::IPAddressList(QWidget * parent):
    QTableView(parent)
{
    contextMenu=new QMenu(this);
    addAction=new QAction(tr("Add IP"), this);
    addAction->setIcon(QIcon(":/icons/add_ip.png"));
    addAction->setIconVisibleInMenu(true);

    deleteAction=new QAction(tr("Remove IP"), this);
    deleteAction->setIcon(QIcon(":/icons/remove_ip.png"));
    deleteAction->setIconVisibleInMenu(true);

    contextMenu->addAction(addAction);
    contextMenu->addAction(deleteAction);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);

    connect(deleteAction,SIGNAL(triggered()),this,SLOT(removeSelectedIP()));
    connect(addAction,SIGNAL(triggered()),this,SIGNAL(addIPField()));
}

void IPAddressList::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu->exec(event->globalPos());
}

void IPAddressList::removeSelectedIP()
{
    QModelIndexList selectedList=this->selectionModel()->selectedRows();
    emit removeSelected(selectedList);
}



