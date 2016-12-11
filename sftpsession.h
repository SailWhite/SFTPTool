#ifndef SFTPSESSION_H
#define SFTPSESSION_H

#include <QString>
#include <QHash>
#include <QFile>
#include <QProgressDialog>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QMessageBox>

#include "libssh2.h"
#include "libssh2_sftp.h"
#include "ui_mainwindow.h"
#include "sftpuploaddialog.h"

#include <fcntl.h>

class SftpWindow;

class SftpSession : public QThread
{
    Q_OBJECT

public:
    SftpSession(SftpWindow* sftp_window);

    ~SftpSession();

    int create_libssh2_session(int socket);

    int auth_login_info(QString& user, QString& password);

    void close_libssh2();

    void read_sftp_file_list(QString path);

    void flush_current_path();

    QVector<QString>* get_current_path();

    bool is_directory(QString& file_name);

    void download_file(QString& file_name, QString save_path);

    int upload_file(QString& file_name, QString& absolute_path);

    void set_server_name(QString& server_name);

    void run_upload();

private:
    int init_libssh2();

    void transport_file_data(QString& file_name, QString save_path);

private slots:
    void update_download_progress_dialog();

    void update_upload_progress_dialog();

    void exit_download();

    void exit_upload();

Q_SIGNALS:
    void download_finish();

    void upload_finish();

protected:
     void run();

private:
    SftpWindow* m_sftp_window;

    LIBSSH2_SESSION *m_libssh2_session;

    LIBSSH2_SFTP *m_sftp_session;

    LIBSSH2_SFTP_HANDLE *m_sftp_directory_handle;

    LIBSSH2_SFTP_HANDLE *m_sftp_download_handle;

    QHash<QString, LIBSSH2_SFTP_HANDLE*> m_sftp_upload_handles;

    QHash<QString, bool> m_is_directory;

    QHash<QString, int> m_file_size;

    QProgressDialog* m_progress_dialog;

    SftpUploadDialog m_upload_dialog;

    QTimer* m_download_timer;

    QTimer* m_upload_timer;

    FILE* m_tempstorage;

    QHash<QString, FILE*> m_upload_files;

    bool m_is_download;

    bool m_is_upload;

    QMessageBox m_console;

    QVector<QString> m_current_path;

    QHash<QString, QString> m_upload_file_path;

    QString m_server_name;

    QMutex m_mutex_lock;
};

#endif // SFTPSESSION_H
