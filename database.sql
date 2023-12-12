R"(CREATE TABLE MetaData
(
    ID INTEGER PRIMARY KEY AUTOINCREMENT,
    Key TEXT UNIQUE,
    Value INTEGER
);

INSERT INTO MetaData(Key, Value) VALUES ('DatabaseVersion', '0');

CREATE TABLE Users
(
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Login TEXT,
    Password TEXT
);)"
