#pragma once

#include <common/logger_settings.h>

#include <nlohmann/json.hpp>
#include <filesystem>

namespace Settings {

struct Server
{
    uint16_t m_port = 5555;

    void Load(const nlohmann::json& element);
    void Save(nlohmann::json& element);
};

struct Database
{
    std::string m_pathToDatabase = "database.db";

    void Load(const nlohmann::json& element);
    void Save(nlohmann::json& element);
};

struct Root
{
    Server m_server;
    Logger m_logger;
    Database m_database;

    void SaveOrCreate(const std::filesystem::path& filePath);
};

Root LoadOrCreate(const std::filesystem::path& filePath);
Root Load(const std::filesystem::path& filePath);
}
