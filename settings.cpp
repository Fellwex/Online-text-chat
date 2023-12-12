#include "settings.h"

#include <common/utils/error_handling_utils.h>

#include <fstream>

namespace Settings {

static void WriteToFile(nlohmann::json& json, const std::filesystem::path& filePath);

Root LoadOrCreate(const std::filesystem::path& filePath)
{
    if(std::filesystem::exists(filePath))
        return Load(filePath);

    Root settings;
    settings.SaveOrCreate(filePath);
    return settings;
}

static void WriteToFile(nlohmann::json& config,
                        const std::filesystem::path& filePath)
{
    std::ofstream configFile(filePath);

    if(configFile)
    {
        configFile << config.dump(1);
        configFile.close();
    }
    else
        LogAndThrow<std::runtime_error>("Cannot create the config file");

}

Root Load(const std::filesystem::path& filePath)
{
    Root settings;
    std::ifstream configFile(filePath);

    if(configFile)
    {
        nlohmann::json config;
        configFile >> config;
        settings.m_server.Load(config.at("Server"));
        settings.m_logger.Load(config.at("Logger"));
        settings.m_database.Load(config.at("Database"));
    }
    else
        LogAndThrow<std::runtime_error>("Cannot open the config file");

    return settings;
}

void Root::SaveOrCreate(const std::filesystem::path& filePath)
{
    nlohmann::json config;
    m_server.Save(config["Server"]);
    m_logger.Save(config["Logger"]);
    m_database.Save(config["Database"]);
    WriteToFile(config, filePath);
}

void Server::Load(const nlohmann::json& element)
{
    m_port = element.at("port");
}

void Server::Save(nlohmann::json& element)
{
    element["port"] = m_port;
}

void Database::Load(const nlohmann::json& element)
{
    m_pathToDatabase = element.at("PathToDatabase");
}

void Database::Save(nlohmann::json& element)
{
    element["PathToDatabase"] = m_pathToDatabase;
}

}
