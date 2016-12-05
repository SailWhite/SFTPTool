#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sftpconfigmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_ftp_window = new SftpWindow(ui);

    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(start_init()));

    show_init_msg();

    m_timer.start(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start_init()
{
    init_window();
    init_config();

    hide_init_msg();

    m_timer.stop();
    show();
}

void MainWindow::init_window()
{
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setFixedSize(1034,724);
    setWindowIcon(QIcon(":/images/SFTPTool.ico"));
}

void MainWindow::init_config()
{
    QVector<SERVER_CFG> server_config = SftpConfigManager::instance()->get_server_config();

    QVector<SERVER_CFG>::iterator it = server_config.begin();
    for (; it != server_config.end(); ++it)
    {
        ui->EDIT_SERVER->setText((*it).host);
        ui->EDIT_USER->setText((*it).user);
        ui->EDIT_PASSWORD->setText((*it).password);

        if (0 == (*it).game)
        {
            ui->COMBOX_GAME->setCurrentIndex(0);
        }
        if (1 == (*it).game)
        {
            ui->COMBOX_GAME->setCurrentIndex(1);
        }
        if (2 == (*it).game)
        {
            ui->COMBOX_GAME->setCurrentIndex(2);
        }

        m_ftp_window->connect_sftp_server();
    }
}

void MainWindow::show_init_msg()
{
    text.setParent(&m_init_msg);
    picture.setParent(&m_init_msg);
    QPixmap pixmap(":/images/init.png");
    pixmap.scaled(40,40);

    QFont font;
    font.setPointSize(10);

    text.setFont(font);
    text.setText("Loading config...");
    text.setFixedSize(200, 100);
    text.setGeometry(70, -10, 50, 50);

    picture.setPixmap(pixmap);
    picture.setFixedSize(40, 40);
    picture.setGeometry(20, 20, 50, 50);

    m_init_msg.setFixedSize(200, 80);
    m_init_msg.setWindowFlags(Qt::Popup);
    m_init_msg.setStyleSheet("background-color: white");

    m_init_msg.show();
}

void MainWindow::hide_init_msg()
{
    m_init_msg.hide();
}
