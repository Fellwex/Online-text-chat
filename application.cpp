#include "application.h"

#include <iostream>

Application::Application()
    : m_chatController()
    , m_chatWindow(std::make_unique<ChatWindow>(m_chatController))
{
}

void Application::ShowMainWindow()
{
    m_chatWindow->show();
    m_chatWindow->ShowConnectionDialog();
}
