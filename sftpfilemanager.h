#ifndef SFTPFILEMANAGER_H
#define SFTPFILEMANAGER_H

#include <QString>
#include <QPixmap>
#include <QDir>

class SftpWindow;

class SftpFileManager
{
public:
    SftpFileManager(SftpWindow* sftp_window);

    ~SftpFileManager();

    void flush_local_file(const QString& file_name, bool absolute = false);

    void set_root_path(QString path){m_root_path = path;};

    void flush_local_file();

    void clear_file_record();

    QString get_current_local_path();

private:
    void get_file_icon(QFileInfo& file, QPixmap& icon);

private:
    SftpWindow* m_sftp_window;

    QVector<QString> m_path_record;

    QDir m_local_dir;

    QString m_root_path;
};

#endif // SFTPFILEMANAGER_H
