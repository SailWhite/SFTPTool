#include "sftpsession.h"
#include "sftpwindow.h"
#include "stdio.h"

SftpSession::SftpSession(SftpWindow* sftp_window):m_libssh2_session(NULL), m_sftp_session(NULL), m_tempstorage(NULL), m_is_download(false), m_is_upload(false)
{
    m_sftp_window = sftp_window;
    m_progress_dialog = new QProgressDialog();
    m_download_timer = new QTimer();
    m_upload_timer = new QTimer();

    QPushButton* btn = new QPushButton;
    btn->setText("waiting...");
    btn->setEnabled(false);
    m_progress_dialog->setCancelButton(btn);

    QObject::connect(m_download_timer, SIGNAL(timeout()), this, SLOT(update_download_progress_dialog()));
    QObject::connect(m_upload_timer, SIGNAL(timeout()), this, SLOT(update_upload_progress_dialog()));
    QObject::connect(this, SIGNAL(download_finish()), this, SLOT(exit_download()));
    QObject::connect(this, SIGNAL(upload_finish()), this, SLOT(exit_upload()));


    if (-1 == init_libssh2())
    {
        m_sftp_window->display_error_code(1);
    }
}

SftpSession::~SftpSession()
{
    QObject::disconnect(m_download_timer, SIGNAL(timeout()), this, SLOT(update_download_progress_dialog()));
    QObject::disconnect(m_upload_timer, SIGNAL(timeout()), this, SLOT(update_upload_progress_dialog()));
    QObject::disconnect(this, SIGNAL(download_finish()), this, SLOT(exit_download()));
    QObject::disconnect(this, SIGNAL(upload_finish()), this, SLOT(exit_upload()));

    delete m_progress_dialog;
    delete m_download_timer;
    delete m_upload_timer;
}

int SftpSession::init_libssh2()
{
    if (0 != libssh2_init(0))
    {
        return -1;
    }

    m_libssh2_session = libssh2_session_init();
    if(!m_libssh2_session)
    {
        return -1;
    }
    return 0;
}

void SftpSession::close_libssh2()
{
    if (NULL != m_sftp_session)
    {
        libssh2_sftp_shutdown(m_sftp_session);
    }
    if (NULL != m_libssh2_session)
    {
        libssh2_session_disconnect(m_libssh2_session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(m_libssh2_session);
    }
    /*if (NULL != m_sftp_handle)
    {
        //libssh2_sftp_close(m_sftp_handle);
    }*/
    libssh2_exit();
}

int SftpSession::create_libssh2_session(int socket)
{
    libssh2_session_set_blocking(m_libssh2_session, 1);

    if(libssh2_session_handshake(m_libssh2_session, socket))
    {
        m_sftp_window->display_error_code(1);
        return -1;
    }

    return 0;
}

int SftpSession::auth_login_info(QString& user, QString& password)
{
    if (libssh2_userauth_password(m_libssh2_session, user.toLatin1().data(), password.toLatin1().data()))
    {
        m_sftp_window->display_error_code(2);
        return -1;
    }

    m_sftp_session = libssh2_sftp_init(m_libssh2_session);
    if (!m_sftp_session)
    {
        m_sftp_window->display_error_code(1);
        return -1;
    }

    return 0;
}

void SftpSession::set_server_name(QString& server_name)
{
    m_server_name = server_name;
}

void SftpSession::read_sftp_file_list(QString path)
{
    if (NULL == m_sftp_session)
    {
        return;
    }

    //path file
    m_sftp_directory_handle = libssh2_sftp_opendir(m_sftp_session, path.toLatin1().data());
    if (!m_sftp_directory_handle)
    {
        m_console.setIcon(QMessageBox::Warning);
        m_console.setText("Permission denied, return home directory!");
        m_console.show();
        read_sftp_file_list(".");
        return;
    }

    m_sftp_window->display_file_path(m_current_path.back());

    char file_name[128];
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    m_is_directory.clear();
    m_is_directory.insert("..", true);
    m_file_size.clear();
    if (m_current_path.size() > 1)
    {
        m_sftp_window->add_parent_path();
    }

    while (0 < libssh2_sftp_readdir(m_sftp_directory_handle, file_name, sizeof(file_name), &attrs))
    {
        if ('.' == file_name[0] || '\0' == file_name)
        {
            continue;
        }

        QString item_name(file_name);
        QPixmap icon;
        QTreeWidgetItem* file_item = new QTreeWidgetItem;

        if (attrs.permissions < 33000 || attrs.permissions > 41000)
        {
            icon.load(":/images/dir.png");
            m_is_directory.insert(file_name, true);
        }
        else
        {
            icon.load(":/images/file.png");
            m_is_directory.insert(file_name, false);
        }
        m_file_size.insert(file_name, attrs.filesize);

        file_item->setIcon(0, icon);
        file_item->setText(0, item_name);

        m_sftp_window->add_file_tree_item(m_sftp_window->get_current_remote_file_tree(), file_item);
    }

    libssh2_sftp_close(m_sftp_directory_handle);
}

void SftpSession::flush_current_path()
{
    read_sftp_file_list(m_current_path.back());
}

QVector<QString>* SftpSession::get_current_path()
{
    return &m_current_path;
}

bool SftpSession::is_directory(QString& file_name)
{
    QHash<QString, bool>::iterator it;
    it = m_is_directory.find(file_name);
    if (it != m_is_directory.end())
    {
        return it.value();
    }
    return false;
}

void SftpSession::download_file(QString& file_name)
{
    if (NULL == m_sftp_session)
    {
        return;
    }

    //path file
    QString file_path = m_current_path.back() + "/" + file_name;
    m_sftp_file_handle = libssh2_sftp_open(m_sftp_session, file_path.toLatin1().data(), LIBSSH2_FXF_READ, 0);
    if (!m_sftp_file_handle)
    {
        m_console.setIcon(QMessageBox::Warning);
        m_console.setText("download file failure!");
        m_console.show();
        return;
    }

    transport_file_data(file_name);
}

void SftpSession::transport_file_data(QString& file_name)
{
    QString save_path = m_sftp_window->get_local_file_path() + "/" + file_name;
    m_tempstorage = fopen(save_path.toLatin1().data(), "wb");
    if (NULL == m_tempstorage)
    {
        m_console.setIcon(QMessageBox::Warning);
        m_console.setText("download file failure!");
        m_console.show();
        return;
    }

    m_progress_dialog->setLabelText(tr("Downloading %1...").arg(file_name));
    m_progress_dialog->setMaximum(m_file_size.find(file_name).value());
    m_progress_dialog->show();

    m_sftp_window->m_ui_context->centralWidget->setEnabled(false);

    m_download_timer->start(100);

    m_is_download = true;

    start();
}

void SftpSession::run()
{
    if (m_is_download)
    {
        char down_load_data[2048];
        int size;
        do
        {
            size = libssh2_sftp_read(m_sftp_file_handle, down_load_data, sizeof(down_load_data));
            if (size > 0)
            {
                m_mutex_lock.lock();
                fwrite(down_load_data, size, 1, m_tempstorage);
                m_mutex_lock.unlock();
            }
            else
            {
                break;
            }
        } while(true);

        m_is_download = false;
        download_finish();
    }
    else if (m_is_upload)
    {
        char read_data[1024*100];
        int read_size, write_size;
        do
        {
            read_size = fread(read_data, 1, sizeof(read_data), m_tempstorage);
            if (read_size <= 0)
            {
                break;
            }

            char* pData = read_data;
            do
            {
                m_mutex_lock.lock();
                write_size = libssh2_sftp_write(m_sftp_file_handle, pData, read_size);
                m_mutex_lock.unlock();

                if(write_size < 0)
                {
                    break;
                }
                pData += write_size;
                read_size -= write_size;
            } while (read_size);

        } while (write_size > 0);

        m_is_upload = false;
        upload_finish();
    }
}

void SftpSession::update_download_progress_dialog()
{
    m_mutex_lock.lock();
    fseek(m_tempstorage, 0L, SEEK_END);
    int filesize = ftell(m_tempstorage);
    m_progress_dialog->setValue(filesize);
    m_mutex_lock.unlock();
}

void SftpSession::update_upload_progress_dialog()
{
    m_mutex_lock.lock();
    int filesize = libssh2_sftp_tell(m_sftp_file_handle);
    //m_progress_dialog->setValue(filesize);
    m_sftp_window->m_mutex_lock.lock();
    m_sftp_window->get_upload_dialog().set_current_value(m_server_name, filesize);
    m_sftp_window->m_mutex_lock.unlock();
    m_mutex_lock.unlock();
}

void SftpSession::upload_file(QString& file_name)
{
    if (NULL == m_sftp_session || file_name.isEmpty())
    {
        return;
    }

    m_tempstorage = fopen((m_sftp_window->get_local_file_path() + "/" + file_name).toLatin1().data(), "rb");
    if (!m_tempstorage)
    {
        m_console.setIcon(QMessageBox::Warning);
        m_console.setText("open file failure!");
        m_console.show();
        return;
    }

    //path file
    QString file_path = m_current_path.back() + "/" + file_name;
    m_sftp_file_handle = libssh2_sftp_open(m_sftp_session, file_path.toLatin1().data(),
                          LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                          LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                          LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

    if (!m_sftp_file_handle)
    {
        m_console.setIcon(QMessageBox::Warning);
        m_console.setText("upload file failure!");
        m_console.show();
        return;
    }

    fseek(m_tempstorage, 0L, SEEK_END);
    int filesize = ftell(m_tempstorage);
    fseek(m_tempstorage, 0L, SEEK_SET);

    /*m_progress_dialog->setLabelText(tr("uploading %1...").arg(file_name));
    m_progress_dialog->setMaximum(filesize);
    m_progress_dialog->show();*/
    m_sftp_window->m_mutex_lock.lock();
    m_sftp_window->get_upload_dialog().add_progress_item(m_server_name);
    m_sftp_window->get_upload_dialog().set_max_value(m_server_name, filesize);
    m_sftp_window->m_ui_context->centralWidget->setEnabled(false);
    m_sftp_window->m_mutex_lock.unlock();

    m_upload_timer->start(100);

    m_is_upload = true;

    start();
}

void SftpSession::exit_download()
{
    m_sftp_window->m_ui_context->centralWidget->setEnabled(true);
    fclose(m_tempstorage);
    m_tempstorage = NULL;
    m_download_timer->stop();
    libssh2_sftp_close(m_sftp_file_handle);
    m_progress_dialog->hide();
    m_sftp_window->flush_file();
}

void SftpSession::exit_upload()
{
    fclose(m_tempstorage);
    m_tempstorage = NULL;
    m_upload_timer->stop();
    libssh2_sftp_close(m_sftp_file_handle);
    //m_progress_dialog->hide();
    m_sftp_window->m_mutex_lock.lock();
    m_sftp_window->m_ui_context->centralWidget->setEnabled(true);
    m_sftp_window->get_upload_dialog().hide_upload_dialog(m_server_name);
    //m_sftp_window->flush_file();
    m_sftp_window->m_mutex_lock.unlock();
}