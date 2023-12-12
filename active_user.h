# pragma once

#include <string>
#include <common/connection.h>

struct ActiveUser
{
    Connection::Ptr connection;
    std::string nickname = "";
    bool authorized = false;
};
