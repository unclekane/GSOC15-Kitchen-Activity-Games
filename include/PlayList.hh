#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QListWidget>

/**
 * The play list extends the QListWidget and implements custom drag and drop events to add and move play list elements.
 */
class PlayList : public QListWidget
{
    Q_OBJECT

    public: PlayList(QWidget *p_parent);

    /**
     * Adds files from string to the play list recursivly. All subfolders will be searched for log files.
     * @param QString with the path to the file or directory in following format: /home/ect/.../file.world
     */
    protected: void addFileOrDirectoryToPlayList( QString p_file );

    protected: void dragEnterEvent(QDragEnterEvent *event);
    protected: void dragMoveEvent(QDragMoveEvent *event);
    protected: void dropEvent(QDropEvent *event);
};

#endif
