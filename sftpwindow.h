#ifndef FTPWINDOW_H
#define FTPWINDOW_H

#include <QHash>
#include <QMutex>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "sftpconfigmanager.h"
#include "sftpuploaddialog.h"

class SftpConnector;
class SftpFileManager;

typedef QHash<QString, SftpConnector*> SftpList;

class SftpWindow : public QObject
{
    Q_OBJECT

    friend class SftpFileManager;
    friend class SftpSession;

public:
    SftpWindow(Ui::MainWindow* pWindow);

    ~SftpWindow();

    void display_error_code(int error);

    QTreeWidget* get_current_remote_file_tree();

    void add_file_tree_item(QTreeWidget* tree, QTreeWidgetItem* file_item);

    void clear_file_tree(QTreeWidget* tree);

    void add_parent_path();

    QString get_local_file_path();

    void display_file_path(QString& path);

    SftpUploadDialog& get_upload_dialog();

    QMutex m_mutex_lock;

private:
    void init_singnals();

    void del_singnals();

    void init_window();

    void init_local_file();

    bool is_login_info_valid(int current_game, QString& server, QString& user, QString& password);

    void add_server_info_item(QTreeWidget* tree, QTreeWidgetItem* server_item);

    void delete_server_info_item(QTreeWidget* tree, QTreeWidgetItem* server_item);

private slots:
    void change_combox_display(int index);

    void download_file();

    void upload_file();

    void disconnect_sftp_server();    

    void change_current_local_path(QTreeWidgetItem* pItem, int column);

    void change_current_remote_path(QTreeWidgetItem* pItem, int column);

    void select_file_path();

public slots:
    void connect_sftp_server();

    void flush_file();

private:
    Ui::MainWindow* m_ui_context;

    QHash<int, SftpFileManager*> m_file_manager;

    QHash<int, SftpList*> m_connector_list;

    SftpUploadDialog m_upload_dialog;

    QMessageBox m_console;

};

#endif // FTPWINDOW_H
