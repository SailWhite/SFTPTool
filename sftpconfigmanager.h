#ifndef SFTPCONFIGMANAGER_H
#define SFTPCONFIGMANAGER_H

#include <QString>
#include <fstream>
#include <QMessageBox>
#include "json/json.h"

typedef struct server_info
{
    int game;
    QString host;
    QString user;
    QString password;
}SERVER_CFG;

class SftpConfigManager
{
public:
    static SftpConfigManager* instance();

    QString get_local_file_path(int index);

    QString get_remote_file_path(int index);

    QVector<SERVER_CFG>& get_server_config();

private:
    SftpConfigManager();

    bool load_config_file(const QString& file_path);

    void load_server_config();

    void close_config_file();

private:
    static SftpConfigManager* m_instance;

    std::ifstream m_cfg_file;

    Json::Reader m_json_reader;

    Json::Value m_json_root;

    QVector<SERVER_CFG> m_server_config;

    QMessageBox m_console;
};

#endif // SFTPCONFIGMANAGER_H
