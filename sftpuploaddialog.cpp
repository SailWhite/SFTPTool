#include "sftpuploaddialog.h"

SftpUploadDialog::SftpUploadDialog()
{
    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
}

void SftpUploadDialog::add_progress_item(QString& file_name)
{
    QLabel* lable = new QLabel(this);
    QString dispaly_name;
    if (file_name.size() > 26)
    {
        dispaly_name = file_name.left(26);
        dispaly_name.append("..:");
    }
    else
    {
        dispaly_name = file_name;
        dispaly_name.append(":");
    }

    lable->setText(dispaly_name);
    lable->setFixedSize(200, 20);
    m_upload_lable.insert(file_name, lable);

    QProgressBar* progressbar = new QProgressBar(this);
    progressbar->setFixedSize(200, 20);
    m_upload_progress.insert(file_name, progressbar);
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
    setFixedSize(400, m_upload_progress.size() * 35);

    QHash<QString, QLabel*>::iterator it_lable = m_upload_lable.begin();
    for (int i = 0; it_lable != m_upload_lable.end(); ++it_lable, ++i)
    {
        (*it_lable)->setGeometry(10, 10 + i * 30, 40, i * 30);
    }

    QHash<QString, QProgressBar*>::iterator it_progress = m_upload_progress.begin();
    for (int i = 0; it_progress != m_upload_progress.end(); ++it_progress, ++i)
    {
        (*it_progress)->setGeometry(200, 10 + i * 30, 40, 20 + i * 30);
    }

    show();
}

void SftpUploadDialog::hide_upload_dialog()
{
    hide();
}

void SftpUploadDialog::clear_item()
{
    QHash<QString, QLabel*>::iterator it_lable = m_upload_lable.begin();
    for (int i = 0; it_lable != m_upload_lable.end(); ++it_lable, ++i)
    {
        delete (*it_lable);
    }
    m_upload_lable.clear();

    QHash<QString, QProgressBar*>::iterator it_progress = m_upload_progress.begin();
    for (int i = 0; it_progress != m_upload_progress.end(); ++it_progress, ++i)
    {
        delete (*it_progress);
    }
    m_upload_progress.clear();
}
