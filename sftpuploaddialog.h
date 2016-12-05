#ifndef SFTPUPLOADDIALOG_H
#define SFTPUPLOADDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QHash>

class SftpUploadDialog : public QDialog
{
public:
    SftpUploadDialog();

    void add_progress_item(QString& file_name);

    void set_max_value(QString& file_name, int value);

    void set_current_value(QString& file_name, int value);

    void show_upload_dialog();

    void hide_upload_dialog();

    void clear_item();

private:


private:

    QHash<QString, QLabel*> m_upload_lable;

    QHash<QString, QProgressBar*> m_upload_progress;
};

#endif // SFTPUPLOADDIALOG_H
