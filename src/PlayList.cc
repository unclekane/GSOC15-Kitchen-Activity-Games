#include "PlayList.hh"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QUrl>

#include <QDir>
#include <QFileInfo>

#include <iostream>

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


void PlayList::addFileOrDirectoryToPlayList( QString p_file )
{
    QFileInfo fileInfo = QFileInfo( p_file );

    if( fileInfo.isDir() )
    {
        QDir dir( p_file );

        if( dir.exists() )
        {
           QStringList filesAndDirectories = dir.entryList();

           for( int i = 0; i < filesAndDirectories.size(); i++ )
           {
               if( filesAndDirectories[i].isEmpty()
                || filesAndDirectories[i].compare(".")  == 0
                || filesAndDirectories[i].compare("..") == 0 )
                   continue;

               QString filePath = fileInfo.absoluteFilePath();
               filePath.append( QDir::separator() );
               filePath.append( filesAndDirectories[i] );

               addFileOrDirectoryToPlayList( filePath );
           }
        }
    }
    else if( fileInfo.suffix().contains("log") )
    {
        addItem( p_file );
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
                addFileOrDirectoryToPlayList( url.toString().remove("file://") );
            }
        }

        event->acceptProposedAction();
    }

    QListWidget::dropEvent(event);
}
