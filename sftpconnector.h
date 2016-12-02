#ifndef SFTPCONNECTOR_H
#define SFTPCONNECTOR_H

#include <libssh2.h>

#include <QString>
#include "sftpsession.h"

class SftpWindow;

class SftpConnector
{
public:
    SftpConnector(SftpWindow* sftp_window);

    ~SftpConnector();

    int connect_sftp_server(QString& server);

    int auth_login_info(QString& user, QString& password);

    void disconnect_sftp_server();

    void read_sftp_file_list(QString file);

    void download_file(QString& file_name, QString save_path = NULL);

    void upload_file(QString& file_name, QString absolute_path = NULL);

    void flush_remote_directory();

private:
    int create_socket(QString& server);

private:
    SftpWindow* m_sftp_window;

    SftpSession* m_sftp_session;

    int m_sftp_connect_socket;
};

#endif // SFTPCONNECTOR_H
