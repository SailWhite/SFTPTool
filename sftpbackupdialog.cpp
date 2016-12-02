#include "sftpbackupdialog.h"
#include "sftpconnector.h"
#include <QTextCodec>

const QString BACKUP_FILE_PATH_99 = "./99";
const QString BACKUP_FILE_PATH_DUMMY = "./dummy";
const QString BACKUP_FILE_PATH_KUNKA = "./kunka";

SftpBackupDialog::SftpBackupDialog(SftpWindow* sfpt_window)
{
    m_sftp_window = sfpt_window;
    m_connector_list = sfpt_window->m_connector_list;

    m_backup_btn = new QPushButton(this);
    m_restore_btn = new QPushButton(this);

    QObject::connect(m_backup_btn, SIGNAL(pressed()), this, SLOT(backup_file()));
    QObject::connect(m_restore_btn, SIGNAL(pressed()), this, SLOT(restore_file()));

    init_update_dialog();
}

SftpBackupDialog::~SftpBackupDialog()
{
    QObject::disconnect(m_backup_btn, SIGNAL(pressed()), this, SLOT(backup_file()));
    QObject::disconnect(m_restore_btn, SIGNAL(pressed()), this, SLOT(restore_file()));

    delete m_backup_btn;
    delete m_restore_btn;
}

void SftpBackupDialog::init_update_dialog()
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    setFixedSize(200, 80);
    m_backup_btn->setFixedSize(80, 40);
    m_backup_btn->setGeometry(10, 20, 100, 60);
    m_backup_btn->setText("备份");
    m_restore_btn->setFixedSize(80, 40);
    m_restore_btn->setGeometry(110, 20, 200, 60);
    m_restore_btn->setText("还原");
}

void SftpBackupDialog::show_backup_dialog()
{
    show();
}

void SftpBackupDialog::hide_backup_dialog()
{
    hide();
}

void SftpBackupDialog::backup_file()
{
    QTreeWidget* server_tree = m_sftp_window->get_current_remote_server_tree();
    QTreeWidget* file_tree = m_sftp_window->get_current_remote_file_tree();


    if (NULL != server_tree && NULL != file_tree)
    {
        QTreeWidgetItem* server_item = server_tree->currentItem();
        QTreeWidgetItem* file_item = file_tree->currentItem();

        QString server_name = server_item->text(0);
        QString file_name = file_item->text(0);

        int index = m_sftp_window->m_ui_context->COMBOX_GAME->currentIndex();

        if (0 == index)
        {
            m_connector_list.value(index)->value(server_name)->download_file(file_name, BACKUP_FILE_PATH_99);
        }
        else if (1 == index)
        {
            m_connector_list.value(index)->value(server_name)->download_file(file_name, BACKUP_FILE_PATH_DUMMY);
        }
        else if (2 == index)
        {
            m_connector_list.value(index)->value(server_name)->download_file(file_name, BACKUP_FILE_PATH_KUNKA);
        }
    }
    hide();
}

void SftpBackupDialog::restore_file()
{
    QTreeWidget* server_tree = m_sftp_window->get_current_remote_server_tree();
    QTreeWidget* file_tree = m_sftp_window->get_current_remote_file_tree();


    if (NULL != server_tree && NULL != file_tree)
    {
        QTreeWidgetItem* server_item = server_tree->currentItem();
        QTreeWidgetItem* file_item = file_tree->currentItem();

        QString server_name = server_item->text(0);
        QString file_name = file_item->text(0);

        int index = m_sftp_window->m_ui_context->COMBOX_GAME->currentIndex();

        if (0 == index)
        {
            m_connector_list.value(index)->value(server_name)->upload_file(file_name, BACKUP_FILE_PATH_99);
        }
        else if (1 == index)
        {
            m_connector_list.value(index)->value(server_name)->upload_file(file_name, BACKUP_FILE_PATH_DUMMY);
        }
        else if (2 == index)
        {
            m_connector_list.value(index)->value(server_name)->upload_file(file_name, BACKUP_FILE_PATH_KUNKA);
        }
    }
    hide();
}
