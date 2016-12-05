#include "sftpfilemanager.h"
#include "sftpwindow.h"
#include "Windows.h"
#include "ShellAPI.h"

SftpFileManager::SftpFileManager(SftpWindow* sftp_window)
{
    m_sftp_window = sftp_window;
}

SftpFileManager::~SftpFileManager()
{

}

QString SftpFileManager::get_current_local_path()
{
    return m_path_record.back();
}

void SftpFileManager::clear_file_record()
{
    m_path_record.clear();
}

void SftpFileManager::flush_local_file(const QString& file_name, bool absolute)
{
    QString file_path;
    if ("." == file_name)
    {
        return;
    }

    if (".." == file_name)
    {
        m_path_record.pop_back();
        file_path = m_path_record.back();
    }
    else
    {
        if (absolute)
        {
            file_path = file_name;
        }
        else
        {
            file_path = m_path_record.back() + "\\" + file_name;
        }
        m_path_record.push_back(file_path);
    }

    QFileInfoList file_list;

    QString current_path = m_local_dir.path();
    m_local_dir.setPath(file_path);
    file_list = m_local_dir.entryInfoList();

    if (file_list.isEmpty())
    {
        ShellExecuteW(NULL,QString("open").toStdWString().c_str(),QString(file_path).toStdWString().c_str(),NULL,NULL,SW_SHOW);
        m_path_record.pop_back();
        m_local_dir.setPath(current_path);
        return;
    }

    m_sftp_window->clear_file_tree(m_sftp_window->m_ui_context->TREE_LOCAL);

    QFileInfoList::iterator it = file_list.begin();
    for (; it != file_list.end(); ++it)
    {
        if ((file_path == m_root_path) && (it->fileName() == "." || it->fileName() == ".."))
        {
            continue;
        }
        QString file_name;
        QPixmap icon;
        QTreeWidgetItem* tree_item = new QTreeWidgetItem;

        file_name = it->fileName();
        get_file_icon(*it, icon);

        tree_item->setIcon(0, icon);
        tree_item->setText(0, file_name);

        m_sftp_window->add_file_tree_item(m_sftp_window->m_ui_context->TREE_LOCAL, tree_item);
    }

    m_sftp_window->m_ui_context->EDIT_LOCAL_PATH->setText(m_path_record.back());
}

void SftpFileManager::flush_local_file()
{
    QFileInfoList file_list;

    m_local_dir.setPath(m_local_dir.path());
    file_list = m_local_dir.entryInfoList();

    if (file_list.isEmpty())
    {
        return;
    }

    m_sftp_window->clear_file_tree(m_sftp_window->m_ui_context->TREE_LOCAL);

    QFileInfoList::iterator it = file_list.begin();
    for (; it != file_list.end(); ++it)
    {
        if ((m_path_record.back() == m_root_path) && (it->fileName() == "." || it->fileName() == ".."))
        {
            continue;
        }
        QString file_name;
        QPixmap icon;
        QTreeWidgetItem* tree_item = new QTreeWidgetItem;

        file_name = it->fileName();
        get_file_icon(*it, icon);

        tree_item->setIcon(0, icon);
        tree_item->setText(0, file_name);

        m_sftp_window->add_file_tree_item(m_sftp_window->m_ui_context->TREE_LOCAL, tree_item);
    }
    m_sftp_window->m_ui_context->EDIT_LOCAL_PATH->setText(m_local_dir.path());
}

void SftpFileManager::get_file_icon(QFileInfo& file, QPixmap& icon)
{
    QString file_format = ":/images/";
    if(true == file.isDir())
    {
        file_format.append("dir");
    }
    else
    {
        file_format.append(file.suffix());
    }
    file_format.append(".png");

    icon.load(file_format);
    if (icon.isNull())
    {
        icon.load(":/images/file.png");
    }
}
