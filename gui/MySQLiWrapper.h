#ifndef MYSQLIWRAPPER_H
#define MYSQLIWRAPPER_H

#include <string>
#include <vector>
#include <map>
#include <mysql/mysql.h>

class MySQLiWrapper {
   public:
    MySQLiWrapper(const std::string& configFile);
    ~MySQLiWrapper();

    bool query(const std::string& sql);
    bool del(const std::string& sql);
    std::map<std::string, std::string> fetchArray();
    std::vector<std::map<std::string, std::string>> fetchAll();
    void close();
    unsigned long long lastInsertID();
    unsigned int numRows();

    bool prepare(const std::string& sql);
    bool bindParam(unsigned int index, const std::string& value);
    bool execute();
    std::vector<std::map<std::string, std::string>> fetchPreparedAll();

   private:
    bool loadConfig(const std::string& path);
    void clearResult();

    MYSQL* conn;
    MYSQL_RES* result;
    MYSQL_ROW row;

    MYSQL_STMT* stmt;
    std::vector<MYSQL_BIND> bindParams;
    std::vector<std::string> paramStorage;

    std::vector<std::string> fieldNames;

    std::string host, user, password, database;
};

#endif
