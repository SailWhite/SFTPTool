#ifndef SFTPBACKUPDIALOG_H
#define SFTPBACKUPDIALOG_H

#include <QDialog>
#include <QString>
#include <QPushButton>
#include "sftpwindow.h"

class SftpWindow;

class SftpBackupDialog : public QDialog
{
    Q_OBJECT

public:
    SftpBackupDialog(SftpWindow* sfpt_window);

    ~SftpBackupDialog();

    void init_update_dialog();

    void show_backup_dialog();

    void hide_backup_dialog();

private slots:
    void backup_file();

    void restore_file();

private:
    SftpWindow* m_sftp_window;

    QHash<int, SftpList*> m_connector_list;

    QPushButton* m_backup_btn;

    QPushButton* m_restore_btn;
};

#endif // SFTPBACKUPDIALOG_H
