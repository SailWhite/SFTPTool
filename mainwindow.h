#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QTimer>
#include "sftpwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private:
    void init_window();

    void init_config();

    void show_init_msg();

    void hide_init_msg();

private slots:
    void start_init();

private:
    Ui::MainWindow *ui;

    SftpWindow* m_ftp_window;

    QTimer m_timer;

    QDialog m_init_msg;

    QLabel text;

    QLabel picture;
};

#endif // MAINWINDOW_H
