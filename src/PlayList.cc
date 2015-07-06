#include "PlayList.hh"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QUrl>

/////////////////////////////////////////////////
PlayList::PlayList(QWidget *p_parent) : QListWidget(p_parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);

    setAlternatingRowColors(true);
}

/////////////////////////////////////////////////
void PlayList::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QListWidget::dragEnterEvent(event);
    }
}

/////////////////////////////////////////////////
void PlayList::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QListWidget::dragMoveEvent(event);
    }
}

/////////////////////////////////////////////////
void PlayList::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QUrl url;

            foreach (url,urls)
            {
                if( url.toString().contains(".log") )
                    new QListWidgetItem(url.toLocalFile(),this);
            }
        }

        event->acceptProposedAction();
    }

    QListWidget::dropEvent(event);
}
