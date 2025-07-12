#include "MySQLiWrapper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <mysql/errmsg.h>

// Beispiele:

// 1. Einfache SELECT-Abfrage (query + fetchArray)
/*
MySQLiWrapper db("/home/pi/.mysql_energiebox.cfg");
if (db.query("SELECT * FROM tracer ORDER BY id DESC LIMIT 1")) {
    auto row = db.fetchArray();
    for (const auto& [key, val] : row) {
        std::cout << key << ": " << val << std::endl;
    }
}
*/

// 2. Mehrere Zeilen holen (query + fetchAll)
/*
if (db.query("SELECT * FROM tracer ORDER BY id DESC LIMIT 5")) {
    auto rows = db.fetchAll();
    for (const auto& row : rows) {
        std::cout << "--- Datensatz ---\n";
        for (const auto& [key, val] : row) {
            std::cout << key << ": " << val << "\n";
        }
    }
}
*/

// 3. Anzahl der Zeilen (numRows)
/*
 * if (db.query("SELECT * FROM tracer")) {
    std::cout << "Anzahl Zeilen: " << db.numRows() << std::endl;
}
*/

// 4. Einfacher Löschbefehl (delete)
/*
if (db.del("DELETE FROM tracer WHERE id < 1000")) {
    std::cout << "Alte Einträge gelöscht!" << std::endl;
}
*/

// 5. Letzte ID nach einem INSERT (lastInsertID)
/*
if (db.del("INSERT INTO tracer (spannung, strom, leistung) VALUES (12.1, 3.4, 41.1)")) {
    std::cout << "Neue ID: " << db.lastInsertID() << std::endl;
}
*/

// 6. Prepared Statement mit 1 Parameter (SELECT WHERE)
/*
if (db.prepare("SELECT * FROM tracer WHERE spannung > ?")) {
    db.bindParam(0, "11.5");
    db.execute();
    auto rows = db.fetchPreparedAll();

    for (auto& row : rows) {
        std::cout << "Messung über 11.5V:\n";
        for (auto& [k, v] : row) {
            std::cout << k << ": " << v << "\n";
        }
        std::cout << "---\n";
    }
}
*/

// 7. Prepared INSERT mit mehreren Parametern
/*
if (db.prepare("INSERT INTO tracer (spannung, strom, leistung) VALUES (?, ?, ?)")) {
    db.bindParam(0, "13.7");
    db.bindParam(1, "5.1");
    db.bindParam(2, "69.9");
    if (db.execute()) {
        std::cout << "Neue Messung gespeichert mit ID " << db.lastInsertID() << std::endl;
    }
}
*/

// 8. Kombiniert: INSERT + anschließend SELECT der letzten Zeile
/*
db.prepare("INSERT INTO tracer (spannung, strom, leistung) VALUES (?, ?, ?)");
db.bindParam(0, "14.2");
db.bindParam(1, "4.0");
db.bindParam(2, "56.8");
db.execute();

// SELECT zuletzt eingefügte Zeile
db.query("SELECT * FROM tracer ORDER BY id DESC LIMIT 1");
auto row = db.fetchArray();
std::cout << "Letzter Eintrag:\n";
for (auto& [key, val] : row) {
    std::cout << key << ": " << val << std::endl;
}
*/

// 9. Prepared SELECT mit mehreren Parametern
/*
if (db.prepare("SELECT * FROM tracer WHERE spannung > ? AND leistung < ?")) {
    db.bindParam(0, "12.0");
    db.bindParam(1, "100");
    db.execute();

    auto rows = db.fetchPreparedAll();
    for (auto& row : rows) {
        std::cout << "Treffer:\n";
        for (auto& [key, val] : row)
            std::cout << key << ": " << val << "\n";
        std::cout << "-----\n";
    }
}
*/

MySQLiWrapper::MySQLiWrapper(const std::string& configFile) : conn(nullptr), result(nullptr), row(nullptr), stmt(nullptr) {
    if (!loadConfig(configFile)) {
        throw std::runtime_error("Konnte MySQL-Config nicht laden");
    }
    conn = mysql_init(nullptr);
    if (!conn) {
        throw std::runtime_error("mysql_init() fehlgeschlagen");
    }
    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, nullptr, 0)) {
        throw std::runtime_error(mysql_error(conn));
    }
}

MySQLiWrapper::~MySQLiWrapper() {
    close();
    if (stmt) {
        mysql_stmt_close(stmt);
    }
}

bool MySQLiWrapper::loadConfig(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string key, val;
        if (getline(iss, key, '=') && getline(iss, val)) {
            if (key == "host")
                host = val;
            else if (key == "user")
                user = val;
            else if (key == "password")
                password = val;
            else if (key == "database")
                database = val;
        }
    }
    return !(host.empty() || user.empty() || database.empty());
}

bool MySQLiWrapper::query(const std::string& sql) {
    clearResult();
    if (mysql_query(conn, sql.c_str()) != 0) {
        std::cerr << "MySQL Query Fehler: " << mysql_error(conn) << std::endl;
        return false;
    }
    result = mysql_store_result(conn);
    if (result) {
        unsigned int num_fields = mysql_num_fields(result);
        MYSQL_FIELD* fields = mysql_fetch_fields(result);
        fieldNames.clear();
        for (unsigned int i = 0; i < num_fields; ++i) {
            fieldNames.push_back(fields[i].name);
        }
    }
    return true;
}

bool MySQLiWrapper::del(const std::string& sql) {
    clearResult();
    return mysql_query(conn, sql.c_str()) == 0;
}

std::map<std::string, std::string> MySQLiWrapper::fetchArray() {
    std::map<std::string, std::string> rowMap;
    if (!result) return rowMap;
    row = mysql_fetch_row(result);
    if (!row) return rowMap;
    unsigned int num_fields = mysql_num_fields(result);
    for (unsigned int i = 0; i < num_fields; ++i) {
        rowMap[fieldNames[i]] = row[i] ? row[i] : "NULL";
    }
    return rowMap;
}

std::vector<std::map<std::string, std::string>> MySQLiWrapper::fetchAll() {
    std::vector<std::map<std::string, std::string>> allRows;
    std::map<std::string, std::string> rowMap;
    while (!(rowMap = fetchArray()).empty()) {
        allRows.push_back(rowMap);
    }
    return allRows;
}

void MySQLiWrapper::close() {
    clearResult();
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
    }
}

unsigned long long MySQLiWrapper::lastInsertID() {
    return mysql_insert_id(conn);
}

unsigned int MySQLiWrapper::numRows() {
    return result ? mysql_num_rows(result) : 0;
}

void MySQLiWrapper::clearResult() {
    if (result) {
        mysql_free_result(result);
        result = nullptr;
    }
}

// -------- PREPARE / BIND / EXECUTE --------

bool MySQLiWrapper::prepare(const std::string& sql) {
    if (stmt) {
        mysql_stmt_close(stmt);
        stmt = nullptr;
    }
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "mysql_stmt_init() failed\n";
        return false;
    }
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length()) != 0) {
        std::cerr << "Prepare failed: " << mysql_stmt_error(stmt) << std::endl;
        return false;
    }
    unsigned long param_count = mysql_stmt_param_count(stmt);
    bindParams.resize(param_count);
    paramStorage.resize(param_count);
    for (auto& bind : bindParams) {
        memset(&bind, 0, sizeof(MYSQL_BIND));
        bind.buffer_type = MYSQL_TYPE_STRING;
    }
    return true;
}

bool MySQLiWrapper::bindParam(unsigned int index, const std::string& value) {
    if (index >= bindParams.size()) return false;
    paramStorage[index] = value;
    bindParams[index].buffer = (void*)paramStorage[index].c_str();
    bindParams[index].buffer_length = paramStorage[index].length();
    return true;
}

bool MySQLiWrapper::execute() {
    if (!stmt) return false;
    if (!bindParams.empty() && mysql_stmt_bind_param(stmt, bindParams.data()) != 0) {
        std::cerr << "Bind failed: " << mysql_stmt_error(stmt) << std::endl;
        return false;
    }
    if (mysql_stmt_execute(stmt) != 0) {
        std::cerr << "Execute failed: " << mysql_stmt_error(stmt) << std::endl;
        return false;
    }
    return true;
}

std::vector<std::map<std::string, std::string>> MySQLiWrapper::fetchPreparedAll() {
    std::vector<std::map<std::string, std::string>> allRows;
    MYSQL_RES* meta = mysql_stmt_result_metadata(stmt);
    if (!meta) return allRows;
    int num_fields = mysql_num_fields(meta);
    std::vector<MYSQL_BIND> resultBinds(num_fields);
    std::vector<std::string> resultBuffers(num_fields);
    std::vector<unsigned long> lengths(num_fields);
    std::vector<my_bool> is_null(num_fields);
    for (int i = 0; i < num_fields; ++i) {
        resultBuffers[i].resize(1024);  // Max buffer size
        memset(&resultBinds[i], 0, sizeof(MYSQL_BIND));
        resultBinds[i].buffer_type = MYSQL_TYPE_STRING;
        resultBinds[i].buffer = &resultBuffers[i][0];
        resultBinds[i].buffer_length = resultBuffers[i].size();
        resultBinds[i].length = &lengths[i];
        resultBinds[i].is_null = &is_null[i];
    }
    if (mysql_stmt_bind_result(stmt, resultBinds.data()) != 0) {
        std::cerr << "Result bind failed\n";
        mysql_free_result(meta);
        return allRows;
    }
    mysql_stmt_store_result(stmt);
    MYSQL_FIELD* fields = mysql_fetch_fields(meta);
    while (mysql_stmt_fetch(stmt) == 0) {
        std::map<std::string, std::string> row;
        for (int i = 0; i < num_fields; ++i) {
            row[fields[i].name] = is_null[i] ? "NULL" : resultBuffers[i].substr(0, lengths[i]);
        }
        allRows.push_back(row);
    }
    mysql_free_result(meta);
    return allRows;
}
