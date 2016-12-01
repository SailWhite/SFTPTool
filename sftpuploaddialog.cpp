#include "sftpuploaddialog.h"

SftpUploadDialog::SftpUploadDialog()
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
}

void SftpUploadDialog::add_progress_item(QString& server_name)
{
    QLabel* lable = new QLabel(this);
    lable->setText(server_name);
    lable->setFixedSize(100, 20);
    m_upload_lable.insert(server_name, lable);

    QProgressBar* progressbar = new QProgressBar(this);
    progressbar->setFixedSize(200, 20);
    m_upload_progress.insert(server_name, progressbar);
}

void SftpUploadDialog::set_max_value(QString& file_name, int value)
{
    m_upload_progress.value(file_name)->setMaximum(value);
}

void SftpUploadDialog::set_current_value(QString& file_name, int value)
{
    m_upload_progress.value(file_name)->setValue(value);
}

void SftpUploadDialog::show_upload_dialog()
{
    setFixedSize(300, m_upload_progress.size() * 35);

    QHash<QString, QLabel*>::iterator it_lable = m_upload_lable.begin();
    for (int i = 0; it_lable != m_upload_lable.end(); ++it_lable, ++i)
    {
        (*it_lable)->setGeometry(10, 10 + i * 30, 40, i * 30);
    }

    QHash<QString, QProgressBar*>::iterator it_progress = m_upload_progress.begin();
    for (int i = 0; it_progress != m_upload_progress.end(); ++it_progress, ++i)
    {
        (*it_progress)->setGeometry(100, 10 + i * 30, 40, 20 + i * 30);
    }

    show();
}

void SftpUploadDialog::hide_upload_dialog(QString& server_name)
{
    clear_item(server_name);
    hide();
}

void SftpUploadDialog::clear_item(QString& server_name)
{
    delete m_upload_lable.value(server_name);
    m_upload_lable.remove(server_name);
    delete m_upload_progress.value(server_name);
    m_upload_progress.remove(server_name);
}
