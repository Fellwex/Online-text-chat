#pragma once

#include "chat_controller.h"
#include "chat_window.h"

class Application
{
public:
    Application();
    void ShowMainWindow();

private:
    ChatController              m_chatController;
    std::unique_ptr<ChatWindow> m_chatWindow;
};
