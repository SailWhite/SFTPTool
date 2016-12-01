#include "sftpconfigmanager.h"

const QString CFG_FILE_PATH = "./SFTPTool.json";

SftpConfigManager* SftpConfigManager::m_instance = NULL;

SftpConfigManager* SftpConfigManager::instance()
{
    if (NULL == m_instance)
    {
        m_instance = new SftpConfigManager();
    }
    return m_instance;
}

SftpConfigManager::SftpConfigManager()
{
    load_config_file(CFG_FILE_PATH);
    load_server_config();
    close_config_file();

}

bool SftpConfigManager::load_config_file(const QString& file_path)
{
    m_cfg_file.open(file_path.toLatin1().data(), std::ios::binary);

    if (!m_cfg_file.is_open())
    {
        m_console.setIcon(QMessageBox::Warning);
        m_console.setText("open config file failure!");
        m_console.show();
        return false;
    }

    if (!m_json_reader.parse(m_cfg_file, m_json_root))
    {
        m_console.setIcon(QMessageBox::Warning);
        m_console.setText("parse config file failure!");
        m_console.show();
    }

    return true;
}

void SftpConfigManager::close_config_file()
{
    m_cfg_file.close();
}

QString SftpConfigManager::get_local_file_path(int index)
{
    std::string value;
    if (0 == index)
    {
        value = m_json_root["99_local_root_path"].asString();
    }
    else if (1 == index)
    {
        value = m_json_root["dummy_local_root_path"].asString();
    }
    else if (2 == index)
    {
        value = m_json_root["kunka_local_root_path"].asString();
    }

    QString qvalue(value.c_str());
    return qvalue;
}

void SftpConfigManager::load_server_config()
{
    Json::Value game_id = m_json_root["99"];

    for (unsigned int i = 0; i < game_id.size(); ++i)
    {
        std::string host = game_id[i]["host"].asString();
        std::string user = game_id[i]["user"].asString();
        std::string password = game_id[i]["password"].asString();

        SERVER_CFG info;
        info.game = 0;
        info.host.append(host.c_str());
        info.user.append(user.c_str());
        info.password.append(password.c_str());
        m_server_config.push_back(info);
    }

    game_id = m_json_root["dummy"];

    for (unsigned int i = 0; i < game_id.size(); ++i)
    {
        std::string host = game_id[i]["host"].asString();
        std::string user = game_id[i]["user"].asString();
        std::string password = game_id[i]["password"].asString();

        SERVER_CFG info;
        info.game = 1;
        info.host.append(host.c_str());
        info.user.append(user.c_str());
        info.password.append(password.c_str());
        m_server_config.push_back(info);
    }

    game_id = m_json_root["kunka"];

    for (unsigned int i = 0; i < game_id.size(); ++i)
    {
        std::string host = game_id[i]["host"].asString();
        std::string user = game_id[i]["user"].asString();
        std::string password = game_id[i]["password"].asString();

        SERVER_CFG info;
        info.game = 2;
        info.host.append(host.c_str());
        info.user.append(user.c_str());
        info.password.append(password.c_str());
        m_server_config.push_back(info);
    }
}

QVector<SERVER_CFG>& SftpConfigManager::get_server_config()
{
    return m_server_config;
}
