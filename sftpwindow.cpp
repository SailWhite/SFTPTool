#include "mainwindow.h"
#include "sftpwindow.h"
#include "sftpconnector.h"
#include "sftpfilemanager.h"
#include "sftpbackupdialog.h"
#include "ShellAPI.h"
#include <QFileDialog>

SftpWindow::SftpWindow(Ui::MainWindow* pWindow)
{
    m_ui_context = pWindow;

    int game_num = m_ui_context->COMBOX_GAME->count();
    for (int i = 0; i < game_num; i++)
    {
        m_connector_list.insert(i, new SftpList);
        m_file_manager.insert(i, new SftpFileManager(this));
    }

    m_backup_dialog = new SftpBackupDialog(this);

    init_singnals();
    init_window();
    init_local_file();
}

SftpWindow::~SftpWindow()
{
    int game_num = m_ui_context->COMBOX_GAME->count();
    for (int i = 0; i < game_num; i++)
    {
        delete m_connector_list.value(i);
        delete m_file_manager.value(i);
    }

    delete m_backup_dialog;

    del_singnals();
}

void SftpWindow::init_singnals()
{
    QObject::connect(m_ui_context->COMBOX_GAME, SIGNAL(currentIndexChanged(int)), this, SLOT(change_combox_display(int)));
    QObject::connect(m_ui_context->BTN_DOWNLOAD, SIGNAL(pressed()), this, SLOT(download_file()));
    QObject::connect(m_ui_context->BTN_UPLOAD, SIGNAL(pressed()), this, SLOT(upload_file()));
    QObject::connect(m_ui_context->BTN_FLUSH_FILE, SIGNAL(pressed()), this, SLOT(flush_file()));
    QObject::connect(m_ui_context->BTN_BACKUP, SIGNAL(pressed()), this, SLOT(backup_file()));
    QObject::connect(m_ui_context->TREE_LOCAL, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_local_path(QTreeWidgetItem*, int)));
    QObject::connect(m_ui_context->BTN_LOGIN, SIGNAL(pressed()), this, SLOT(connect_sftp_server()));
    QObject::connect(m_ui_context->BTN_LOGOUT, SIGNAL(pressed()), this, SLOT(disconnect_sftp_server()));
    QObject::connect(m_ui_context->BTN_SELECT_PATH, SIGNAL(pressed()), this, SLOT(select_file_path()));
    QObject::connect(m_ui_context->TREE_99_REMOTE_FILE, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_remote_path(QTreeWidgetItem*, int)));
    QObject::connect(m_ui_context->TREE_DUMMY_REMOTE_FILE, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_remote_path(QTreeWidgetItem*, int)));
    QObject::connect(m_ui_context->TREE_KUNKA_REMOTE_FILE, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_remote_path(QTreeWidgetItem*, int)));
    QObject::connect(m_ui_context->TREE_99_SERVER, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(flush_file()));
    QObject::connect(m_ui_context->TREE_DUMMY_SERVER, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(flush_file()));
    QObject::connect(m_ui_context->TREE_KUNKA_SERVER, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(flush_file()));
    QObject::connect(m_ui_context->BTN_UPLOAD_LOG, SIGNAL(pressed()), this, SLOT(open_upload_log()));
}

void SftpWindow::del_singnals()
{
    QObject::disconnect(m_ui_context->COMBOX_GAME, SIGNAL(currentIndexChanged(int)), this, SLOT(change_combox_display(int)));
    QObject::disconnect(m_ui_context->BTN_DOWNLOAD, SIGNAL(pressed()), this, SLOT(download_file()));
    QObject::disconnect(m_ui_context->BTN_UPLOAD, SIGNAL(pressed()), this, SLOT(upload_file()));
    QObject::disconnect(m_ui_context->BTN_FLUSH_FILE, SIGNAL(pressed()), this, SLOT(flush_file()));
    QObject::disconnect(m_ui_context->BTN_BACKUP, SIGNAL(pressed()), this, SLOT(backup_file()));
    QObject::disconnect(m_ui_context->TREE_LOCAL, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_local_path(QTreeWidgetItem*, int)));
    QObject::disconnect(m_ui_context->BTN_LOGIN, SIGNAL(pressed()), this, SLOT(connect_sftp_server()));
    QObject::disconnect(m_ui_context->BTN_LOGOUT, SIGNAL(pressed()), this, SLOT(disconnect_sftp_server()));
    QObject::disconnect(m_ui_context->BTN_SELECT_PATH, SIGNAL(pressed()), this, SLOT(select_file_path()));
    QObject::disconnect(m_ui_context->TREE_99_REMOTE_FILE, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_remote_path(QTreeWidgetItem*, int)));
    QObject::disconnect(m_ui_context->TREE_DUMMY_REMOTE_FILE, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_remote_path(QTreeWidgetItem*, int)));
    QObject::disconnect(m_ui_context->TREE_KUNKA_REMOTE_FILE, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(change_current_remote_path(QTreeWidgetItem*, int)));
    QObject::disconnect(m_ui_context->TREE_99_SERVER, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(flush_file()));
    QObject::disconnect(m_ui_context->TREE_DUMMY_SERVER, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(flush_file()));
    QObject::disconnect(m_ui_context->TREE_KUNKA_SERVER, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(flush_file()));
    QObject::disconnect(m_ui_context->BTN_UPLOAD_LOG, SIGNAL(pressed()), this, SLOT(open_upload_log()));
}

void SftpWindow::init_window()
{
    m_ui_context->COMBOX_GAME->setCurrentIndex(0);
    m_ui_context->GRPBOX_DUMMY->hide();
    m_ui_context->GRPBOX_KUNKA->hide();
    m_ui_context->EDIT_PASSWORD->setEchoMode(QLineEdit::Password);
    m_ui_context->TREE_99_SERVER->setColumnWidth(0, 280);
    m_ui_context->TREE_DUMMY_SERVER->setColumnWidth(0, 280);
    m_ui_context->TREE_KUNKA_SERVER->setColumnWidth(0, 280);
    m_ui_context->TREE_99_REMOTE_FILE->setColumnWidth(0, 280);
    m_ui_context->TREE_DUMMY_REMOTE_FILE->setColumnWidth(0, 280);
    m_ui_context->TREE_KUNKA_REMOTE_FILE->setColumnWidth(0, 280);
    m_ui_context->TREE_99_REMOTE_FILE->setColumnWidth(1, 150);
    m_ui_context->TREE_DUMMY_REMOTE_FILE->setColumnWidth(1, 150);
    m_ui_context->TREE_KUNKA_REMOTE_FILE->setColumnWidth(1, 150);
    m_ui_context->TREE_LOCAL->setColumnWidth(0, 280);
    m_ui_context->TREE_LOCAL->setSelectionMode(QTreeWidget::ExtendedSelection);
    m_ui_context->TREE_LOCAL->setSortingEnabled(true);
    m_ui_context->TREE_99_REMOTE_FILE->setSortingEnabled(true);
    m_ui_context->TREE_DUMMY_REMOTE_FILE->setSortingEnabled(true);
    m_ui_context->TREE_KUNKA_REMOTE_FILE->setSortingEnabled(true);
}

void SftpWindow::init_local_file()
{
    int size = m_file_manager.size();
    for (int i = 0; i < size; ++i)
    {
        m_file_manager.value(i)->set_root_path(SftpConfigManager::instance()->get_local_file_path(i));
        m_file_manager.value(i)->flush_local_file(SftpConfigManager::instance()->get_local_file_path(i), true);
    }
}

void SftpWindow::flush_file()
{
    int game_index = m_ui_context->COMBOX_GAME->currentIndex();

    m_file_manager.value(game_index)->flush_local_file();

    if (0 == game_index)
    {
        QTreeWidgetItem* server_item = m_ui_context->TREE_99_SERVER->currentItem();
        if (server_item != NULL)
        {
            QString server_mame = server_item->text(0);
            m_connector_list.value(game_index)->value(server_mame)->flush_remote_directory();
            m_ui_context->EDIT_SERVER->setText(server_mame);
        }
    }
    if (1 == game_index)
    {
        QTreeWidgetItem* server_item = m_ui_context->TREE_DUMMY_SERVER->currentItem();
        if (server_item != NULL)
        {
            QString server_mame = server_item->text(0);
            m_connector_list.value(game_index)->value(server_mame)->flush_remote_directory();
            m_ui_context->EDIT_SERVER->setText(server_mame);
        }
    }
    if (2 == game_index)
    {
        QTreeWidgetItem* server_item = m_ui_context->TREE_KUNKA_SERVER->currentItem();
        if (server_item != NULL)
        {
            QString server_mame = server_item->text(0);
            m_connector_list.value(game_index)->value(server_mame)->flush_remote_directory();
            m_ui_context->EDIT_SERVER->setText(server_mame);
        }
    }
}

void SftpWindow::add_file_tree_item(QTreeWidget* tree, QTreeWidgetItem* file_item)
{
    tree->addTopLevelItem(file_item);
}

void SftpWindow::clear_file_tree(QTreeWidget* tree)
{
    tree->clear();
}

void SftpWindow::add_parent_path()
{
    QTreeWidgetItem* parent_item = new QTreeWidgetItem();
    QPixmap pix(":/images/dir.png");

    parent_item->setIcon(0, pix);
    parent_item->setText(0, "..");
    get_current_remote_file_tree()->addTopLevelItem(parent_item);
}

void SftpWindow::add_server_info_item(QTreeWidget* tree, QTreeWidgetItem* server_item)
{
    tree->addTopLevelItem(server_item);
}

void SftpWindow::delete_server_info_item(QTreeWidget* tree, QTreeWidgetItem* server_item)
{
    tree->removeItemWidget(server_item, 0);
    delete server_item;
}

void SftpWindow::change_current_local_path(QTreeWidgetItem* pItem, int column)
{
    int game_index = m_ui_context->COMBOX_GAME->currentIndex();
    change_current_remote_path(pItem, 0);
    m_file_manager.value(game_index)->flush_local_file(pItem->text(column));
}

void SftpWindow::change_current_remote_path(QTreeWidgetItem* pItem, int column)
{
    int index = m_ui_context->COMBOX_GAME->currentIndex();
    QString server;
    if (0 == index)
    {
        QString file_name = pItem->text(column);
        server = m_ui_context->TREE_99_SERVER->currentItem()->text(0);

        QList<QTreeWidgetItem*> list = m_ui_context->TREE_99_REMOTE_FILE->findItems(file_name, Qt::MatchExactly);
        if (0 == list.count())
        {
            return;
        }

        QHash<QString, SftpConnector*>::iterator it = m_connector_list.value(index)->begin();
        for (; it != m_connector_list.value(index)->end(); ++it)
        {
            if (server == it.key())
            {
                continue;
            }
            (*it)->read_sftp_file_list(file_name);
        }
        m_connector_list.value(index)->value(server)->read_sftp_file_list(file_name);
    }
    else if (1 == index)
    {      
        QString file_name = pItem->text(column);
        server = m_ui_context->TREE_DUMMY_SERVER->currentItem()->text(0);

        QList<QTreeWidgetItem*> list = m_ui_context->TREE_DUMMY_REMOTE_FILE->findItems(file_name, Qt::MatchExactly);
        if (0 == list.count())
        {
            return;
        }

        QHash<QString, SftpConnector*>::iterator it = m_connector_list.value(index)->begin();
        for (; it != m_connector_list.value(index)->end(); ++it)
        {
            if (server == it.key())
            {
                continue;
            }
            (*it)->read_sftp_file_list(file_name);
        }
        m_connector_list.value(index)->value(server)->read_sftp_file_list(file_name);
    }
    else if (2 == index)
    {      
        QString file_name = pItem->text(column);
        server = m_ui_context->TREE_KUNKA_SERVER->currentItem()->text(0);

        QList<QTreeWidgetItem*> list = m_ui_context->TREE_KUNKA_REMOTE_FILE->findItems(file_name, Qt::MatchExactly);
        if (0 == list.count())
        {
            return;
        }

        QHash<QString, SftpConnector*>::iterator it = m_connector_list.value(index)->begin();
        for (; it != m_connector_list.value(index)->end(); ++it)
        {
            if (server == it.key())
            {
                continue;
            }
            (*it)->read_sftp_file_list(file_name);
        }
        m_connector_list.value(index)->value(server)->read_sftp_file_list(file_name);
    }
}

void SftpWindow::change_combox_display(int index)
{
    if (0 == index)
    {
        m_ui_context->GRPBOX_99->show();
        m_ui_context->GRPBOX_DUMMY->hide();
        m_ui_context->GRPBOX_KUNKA->hide();
    }
    else if (1 == index)
    {
        m_ui_context->GRPBOX_99->hide();
        m_ui_context->GRPBOX_DUMMY->show();
        m_ui_context->GRPBOX_KUNKA->hide();
    }
    else if (2 == index)
    {
        m_ui_context->GRPBOX_99->hide();
        m_ui_context->GRPBOX_DUMMY->hide();
        m_ui_context->GRPBOX_KUNKA->show();
    }
    m_file_manager.value(index)->flush_local_file();
}

void SftpWindow::download_file()
{
    int game_index = m_ui_context->COMBOX_GAME->currentIndex();

    if (0 == game_index)
    {
        QTreeWidgetItem* server_item = m_ui_context->TREE_99_SERVER->currentItem();
        QTreeWidgetItem* file_item = m_ui_context->TREE_99_REMOTE_FILE->currentItem();
        if (file_item != NULL && server_item != NULL)
        {
            QString file_name = file_item->text(0);
            QString server_name = server_item->text(0);
            m_connector_list.value(game_index)->value(server_name)->download_file(file_name);
        }
    }
    if (1 == game_index)
    {
        QTreeWidgetItem* server_item = m_ui_context->TREE_DUMMY_SERVER->currentItem();
        QTreeWidgetItem* file_item = m_ui_context->TREE_DUMMY_REMOTE_FILE->currentItem();
        if (file_item != NULL)
        {
            QString file_name = file_item->text(0);
            QString server_name = server_item->text(0);
            m_connector_list.value(game_index)->value(server_name)->download_file(file_name);
        }
    }
    if (2 == game_index)
    {
        QTreeWidgetItem* server_item = m_ui_context->TREE_KUNKA_SERVER->currentItem();
        QTreeWidgetItem* file_item = m_ui_context->TREE_KUNKA_REMOTE_FILE->currentItem();
        if (file_item != NULL)
        {
            QString file_name = file_item->text(0);
            QString server_name = server_item->text(0);
            m_connector_list.value(game_index)->value(server_name)->download_file(file_name);
        }
    }
}

void SftpWindow::upload_file()
{
    int game_index = m_ui_context->COMBOX_GAME->currentIndex();
    QList<QTreeWidgetItem*> items = m_ui_context->TREE_LOCAL->selectedItems();
    if (0 == items.count())
    {
        return;
    }

    QSet<SftpConnector*> sftp_session;
    for (int i = 0; i < items.count(); ++i)
    {
        QString file_name = items.at(i)->text(0);

        if (0 == game_index)
        {
            QTreeWidgetItemIterator it(m_ui_context->TREE_99_SERVER);
            while (*it)
            {
                if (Qt::Checked == (*it)->checkState(1))
                {
                    QString server_name = (*it)->text(0);
                    if (-1 == m_connector_list.value(game_index)->value(server_name)->upload_file(file_name))
                    {
                        return;
                    }
                    sftp_session.insert(m_connector_list.value(game_index)->value(server_name));
                }
                ++it ;
            }
        }
        if (1 == game_index)
        {
            QTreeWidgetItemIterator it(m_ui_context->TREE_DUMMY_SERVER);
            while (*it)
            {
                if (Qt::Checked == (*it)->checkState(1))
                {
                    QString server_name = (*it)->text(0);
                    if (-1 == m_connector_list.value(game_index)->value(server_name)->upload_file(file_name))
                    {
                        return;
                    }
                    sftp_session.insert(m_connector_list.value(game_index)->value(server_name));
                }
                ++it ;
            }
        }
        if (2 == game_index)
        {
            QTreeWidgetItemIterator it(m_ui_context->TREE_KUNKA_SERVER);
            while (*it)
            {
                if (Qt::Checked == (*it)->checkState(1))
                {
                    QString server_name = (*it)->text(0);
                    if (-1 == m_connector_list.value(game_index)->value(server_name)->upload_file(file_name))
                    {
                        return;
                    }
                    sftp_session.insert(m_connector_list.value(game_index)->value(server_name));
                }
                ++it ;
            }
        }
    }

    QSet<SftpConnector*>::iterator it = sftp_session.begin();
    for (; it != sftp_session.end(); ++it)
    {
        (*it)->run_upload();
    }
}

void SftpWindow::connect_sftp_server()
{
    QString server = m_ui_context->EDIT_SERVER->text();
    QString user = m_ui_context->EDIT_USER->text();
    QString password = m_ui_context->EDIT_PASSWORD->text();

    int current_game = m_ui_context->COMBOX_GAME->currentIndex();

    //check para
    if (!is_login_info_valid(current_game, server, user, password))
    {
        return;
    }

    //connect
    SftpConnector* sftp_connector = new SftpConnector(this);
    if (-1 == sftp_connector->connect_sftp_server(server))
    {
        delete sftp_connector;
        return;
    }

    //auth
    if (-1 == sftp_connector->auth_login_info(user, password))
    {
        delete sftp_connector;
        return;
    }

    //add to server list
    QTreeWidgetItem* server_item = new QTreeWidgetItem;
    QPixmap pix_server(":/images/server.png");
    server_item->setText(0, server);
    server_item->setIcon(0, pix_server);
    server_item->setCheckState (1, Qt::Checked);

    if (0 == current_game)
    {
        add_server_info_item(m_ui_context->TREE_99_SERVER, server_item);
        m_ui_context->TREE_99_SERVER->setCurrentItem(server_item);
    }
    else if (1 == current_game)
    {
        add_server_info_item(m_ui_context->TREE_DUMMY_SERVER, server_item);
        m_ui_context->TREE_DUMMY_SERVER->setCurrentItem(server_item);
    }
    else if (2 == current_game)
    {
        add_server_info_item(m_ui_context->TREE_KUNKA_SERVER, server_item);
        m_ui_context->TREE_KUNKA_SERVER->setCurrentItem(server_item);
    }

    //get remote file
    QString remote_root_path = SftpConfigManager::instance()->get_remote_file_path(current_game);
    QStringList file = remote_root_path.split("/");
    QStringList::iterator it = file.begin();
    for (; it != file.end(); ++it)
    {
        sftp_connector->read_sftp_file_list((*it));
    }

    //success
    m_connector_list.value(current_game)->insert(server, sftp_connector);
    display_error_code(0);
}

void SftpWindow::reconnect_sftp_server()
{
    QVector<SERVER_CFG> server_config = SftpConfigManager::instance()->get_server_config();

    QVector<SERVER_CFG>::iterator it = server_config.begin();
    for (; it != server_config.end(); ++it)
    {
        m_ui_context->EDIT_SERVER->setText((*it).host);
        m_ui_context->EDIT_USER->setText((*it).user);
        m_ui_context->EDIT_PASSWORD->setText((*it).password);

        if (0 == (*it).game)
        {
            m_ui_context->COMBOX_GAME->setCurrentIndex(0);
        }
        if (1 == (*it).game)
        {
            m_ui_context->COMBOX_GAME->setCurrentIndex(1);
        }
        if (2 == (*it).game)
        {
            m_ui_context->COMBOX_GAME->setCurrentIndex(2);
        }

        connect_sftp_server();
    }
}

void SftpWindow::disconnect_sftp_server()
{
    QString server = m_ui_context->EDIT_SERVER->text();
    int current_game = m_ui_context->COMBOX_GAME->currentIndex();

    if (NULL != m_connector_list.value(current_game)->value(server))
    {
        m_connector_list.value(current_game)->value(server)->disconnect_sftp_server();

        delete m_connector_list.value(current_game)->value(server);
        m_connector_list.value(current_game)->remove(server);
        m_console.setText("disconnect!");
        m_console.show();

        int current_game = m_ui_context->COMBOX_GAME->currentIndex();

        if (0 == current_game)
        {
            delete_server_info_item(m_ui_context->TREE_99_SERVER, m_ui_context->TREE_99_SERVER->findItems(server, Qt::MatchExactly).at(0));
            clear_file_tree(m_ui_context->TREE_99_REMOTE_FILE);
        }
        else if (1 == current_game)
        {
            delete_server_info_item(m_ui_context->TREE_DUMMY_SERVER, m_ui_context->TREE_DUMMY_SERVER->findItems(server, Qt::MatchExactly).at(0));
            clear_file_tree(m_ui_context->TREE_DUMMY_REMOTE_FILE);
        }
        else if (2 == current_game)
        {
            delete_server_info_item(m_ui_context->TREE_KUNKA_SERVER, m_ui_context->TREE_KUNKA_SERVER->findItems(server, Qt::MatchExactly).at(0));
            clear_file_tree(m_ui_context->TREE_KUNKA_REMOTE_FILE);
        }
    }
}

bool SftpWindow::is_login_info_valid(int current_game, QString& server, QString& user, QString& password)
{
    if (server.isEmpty() || user.isEmpty() || password.isEmpty())
    {
        m_console.setText("input your login info please!");
        m_console.show();
        return false;
    }

    if (0 == current_game)
    {
        QList<QTreeWidgetItem*> item = m_ui_context->TREE_99_SERVER->findItems(server, Qt::MatchExactly);
        if (item.size() > 0 )
        {
            m_console.setText("sftp server has already connected!");
            m_console.show();
            return false;
        }
    }
    else if (1 == current_game)
    {
        QList<QTreeWidgetItem*> item = m_ui_context->TREE_DUMMY_SERVER->findItems(server, Qt::MatchExactly);
        if (item.size() > 0 )
        {
            m_console.setText("sftp server has already connected!");
            m_console.show();
            return false;
        }
    }
    else if (2 == current_game)
    {
        QList<QTreeWidgetItem*> item = m_ui_context->TREE_KUNKA_SERVER->findItems(server, Qt::MatchExactly);
        if (item.size() > 0 )
        {
            m_console.setText("sftp server has already connected!");
            m_console.show();
            return false;
        }
    }
    return true;
}

QTreeWidget* SftpWindow::get_current_remote_file_tree()
{
    int current_game = m_ui_context->COMBOX_GAME->currentIndex();
    if (0 == current_game)
    {
        return m_ui_context->TREE_99_REMOTE_FILE;
    }
    else if (1 == current_game)
    {
        return m_ui_context->TREE_DUMMY_REMOTE_FILE;
    }
    else if (2 == current_game)
    {
        return m_ui_context->TREE_KUNKA_REMOTE_FILE;
    }

    return NULL;
}

QTreeWidget* SftpWindow::get_current_remote_server_tree()
{
    int current_game = m_ui_context->COMBOX_GAME->currentIndex();
    if (0 == current_game)
    {
        return m_ui_context->TREE_99_SERVER;
    }
    else if (1 == current_game)
    {
        return m_ui_context->TREE_DUMMY_SERVER;
    }
    else if (2 == current_game)
    {
        return m_ui_context->TREE_KUNKA_SERVER;
    }

    return NULL;
}

void SftpWindow::display_error_code(int error)
{
    if (0 == error)
    {
        //m_console.setText("Connect sftp server success!");
    }
    else if (1 == error)
    {
        m_console.setText("Connect sftp server failed, reconnect!");
        m_console.show();
        m_ui_context->TREE_99_SERVER->clear();
        m_ui_context->TREE_DUMMY_SERVER->clear();
        m_ui_context->TREE_KUNKA_SERVER->clear();
        reconnect_sftp_server();
    }
    else if (2 == error)
    {
        m_console.setText("Auth failure!");
        m_console.show();
    }
}

QString SftpWindow::get_local_file_path()
{
    int index = m_ui_context->COMBOX_GAME->currentIndex();
    return m_file_manager.value(index)->get_current_local_path();
}

void SftpWindow::display_file_path(QString& path)
{
    int index = m_ui_context->COMBOX_GAME->currentIndex();
    if (0 == index)
    {
        m_ui_context->LAB_99_PATH->setText(path);
    }
    else if (1 == index)
    {
        m_ui_context->LAB_DUMMY_PATH->setText(path);
    }
    else if (2 == index)
    {
        m_ui_context->LAB_KUNKA_PATH->setText(path);
    }
}

void SftpWindow::select_file_path()
{
    QString path = m_ui_context->EDIT_LOCAL_PATH->text();
    int index = m_ui_context->COMBOX_GAME->currentIndex();

    if (path.isEmpty())
    {
        QString dir = QFileDialog::getExistingDirectory(NULL, NULL, NULL);
        if (dir.isNull())
        {
            return;
        }
        m_ui_context->EDIT_LOCAL_PATH->setText(dir);
        select_file_path();
    }

    if (path == m_file_manager.value(index)->get_current_local_path())
    {
        return;
    }

    if (!path.isEmpty())
    {
        m_file_manager.value(index)->flush_local_file(path, true);
    }
}

void SftpWindow::backup_file()
{
    if (0 == m_ui_context->COMBOX_GAME->currentIndex())
    {
        QTreeWidgetItem* item = m_ui_context->TREE_99_REMOTE_FILE->currentItem();
        if (NULL == item)
        {
            return;
        }
    }
    else if(1 == m_ui_context->COMBOX_GAME->currentIndex())
    {
        QTreeWidgetItem* item = m_ui_context->TREE_DUMMY_REMOTE_FILE->currentItem();
        if (NULL == item)
        {
            return;
        }
    }
    else if(2 == m_ui_context->COMBOX_GAME->currentIndex())
    {
        QTreeWidgetItem* item = m_ui_context->TREE_KUNKA_REMOTE_FILE->currentItem();
        if (NULL == item)
        {
            return;
        }
    }
    m_backup_dialog->show();
}

void SftpWindow::open_upload_log()
{
    ShellExecuteW(NULL,QString("open").toStdWString().c_str(),QString("uploadlog.txt").toStdWString().c_str(),NULL,NULL,SW_SHOW);
}
