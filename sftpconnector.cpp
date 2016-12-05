#include "sftpconnector.h"
#include "sftpwindow.h"
#include "stdio.h"

SftpConnector::SftpConnector(SftpWindow* sftp_window):m_sftp_connect_socket(-1)
{
    m_sftp_window = sftp_window;
    m_sftp_session = new SftpSession(sftp_window);
}

SftpConnector::~SftpConnector()
{
    delete m_sftp_session;
}

int SftpConnector::connect_sftp_server(QString& server)
{
    m_sftp_connect_socket = create_socket(server);
    if (-1 == m_sftp_connect_socket)
    {
        return -1;
    }

    m_sftp_session->set_server_name(server);

    return 0;
}

void SftpConnector::disconnect_sftp_server()
{
    m_sftp_session->close_libssh2();
    if (-1 != m_sftp_connect_socket)
    {
        closesocket(m_sftp_connect_socket);
    }
}

int SftpConnector::auth_login_info(QString& user, QString& password)
{
    if (-1 == m_sftp_session->create_libssh2_session(m_sftp_connect_socket))
    {
        return -1;
    }

    if (-1 == m_sftp_session->auth_login_info(user, password))
    {
        return -1;
    }

    m_sftp_window->display_error_code(0);
    return 0;
}

int SftpConnector::create_socket(QString& server)
{
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2,0), &wsadata);
    if (err != 0)
    {
        m_sftp_window->display_error_code(1);
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = inet_addr(server.toLatin1().data());

    if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0)
    {
        m_sftp_window->display_error_code(1);
        return -1;
    }
    return sock;
}

void SftpConnector::read_sftp_file_list(QString file)
{
    if (!m_sftp_session->is_directory(file) && file != ".")
    {
        return;
    }
    else
    {
        m_sftp_window->clear_file_tree(m_sftp_window->get_current_remote_file_tree());
    }

    QVector<QString>* current_path = m_sftp_session->get_current_path();
    if (current_path->isEmpty())
    {
        current_path->push_back(".");
        m_sftp_session->read_sftp_file_list(current_path->back());
    }
    else
    {
        if (".." == file)
        {
            current_path->pop_back();
            m_sftp_session->read_sftp_file_list(current_path->back());
        }
        else
        {
            current_path->push_back(current_path->back() + "/" + file);
            m_sftp_session->read_sftp_file_list(current_path->back());
        }
    }
}

void SftpConnector::flush_remote_directory()
{
    m_sftp_window->clear_file_tree(m_sftp_window->get_current_remote_file_tree());
    m_sftp_session->flush_current_path();
}

void SftpConnector::download_file(QString& file_name, QString save_path)
{
    m_sftp_session->download_file(file_name, save_path);
}

int SftpConnector::upload_file(QString& file_name, QString absolute_path)
{
    return m_sftp_session->upload_file(file_name, absolute_path);
}

void SftpConnector::run_upload()
{
    m_sftp_session->run_upload();
}
