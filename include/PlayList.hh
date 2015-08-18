#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QListWidget>

class PlayList : public QListWidget
{
    Q_OBJECT

    public: PlayList(QWidget *p_parent);

    protected: void addFileOrDirectoryToPlayList( QString p_file );

    protected: void dragEnterEvent(QDragEnterEvent *event);
    protected: void dragMoveEvent(QDragMoveEvent *event);
    protected: void dropEvent(QDropEvent *event);
};

#endif
