#pragma once
#include <vector>
#include <string>

using DatabaseVersion = uint16_t;
const DatabaseVersion headDatabaseVersion = 1;

const std::string createDatabaseScript =
      #include "sql/database.sql"
      ;

const std::vector<std::string> upgradeScripts =
{

};
