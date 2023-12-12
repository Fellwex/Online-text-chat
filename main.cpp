#include "chat_server.h"
#include "server.h"
#include "settings.h"

#include <common/logger.h>
#include <common/utils/os_utils.h>

#include <iostream>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    std::filesystem::path configPath;
    Settings::Root settings;

    try
    {
        configPath = utils::CurrentExecutableFilePath().
                        replace_filename("config.json");
        settings = Settings::LoadOrCreate(configPath);
    }
    catch (const std::exception& error)
    {
         std::cout << "Failed to load settings file at: " <<
                      configPath  << '\n' <<
                      "Error: " << error.what() << '\n';

         return -1;
    }

    QCoreApplication a(argc, argv);
    Logger::Initialize(settings.m_logger);

    spdlog::info("Starting server...");
    Server server(settings.m_server);
    ChatServer chat(server);
    chat.InitializeDatabase(settings.m_database);
    chat.RegisterRequestHandlers();

    return a.exec();
}
