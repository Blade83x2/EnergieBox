#ifndef MYSQL_WRAPPER_H
#define MYSQL_WRAPPER_H

#include <mysql/mysql.h>

/* =======================
   Konfiguration
   ======================= */

typedef struct {
    char host[256];
    char user[256];
    char password[256];
    char database[256];
    unsigned int port;
} DBConfig;

/* =======================
   Result / Params
   ======================= */

typedef struct DBResult {
    MYSQL_STMT *stmt;
    MYSQL_BIND *bind;
    unsigned int count;
    unsigned int columns;
    MYSQL_BIND *result_bind;
} DBResult;

typedef struct DBParams {
    MYSQL_BIND *bind;
    unsigned int count;
} DBParams;

/* =======================
   Config / Connection
   ======================= */

int load_db_config(const char *path, DBConfig *config);

MYSQL *db_connect(const DBConfig *config);
void db_close(MYSQL *conn);

/* =======================
   Statements
   ======================= */

MYSQL_STMT *db_prepare(MYSQL *conn, const char *query);
int db_exec_stmt(MYSQL_STMT *stmt);
void db_stmt_close(MYSQL_STMT *stmt);

int db_stmt_bind(MYSQL_STMT *stmt, DBParams *p);
int db_stmt_bind_result(MYSQL_STMT *stmt, DBResult *r);

/* =======================
   SELECT Helper
   ======================= */

DBResult *db_select(MYSQL *conn, const char *query);
int db_select_bind_result(DBResult *res, MYSQL_BIND *bind);
int db_select_fetch(DBResult *res);
void db_select_free(DBResult *res);

/* =======================
   Param Binding (einfach)
   ======================= */

int db_bind_int(MYSQL_STMT *stmt, unsigned int index, int value);
int db_bind_double(MYSQL_STMT *stmt, unsigned int index, double value);
int db_bind_string(MYSQL_STMT *stmt, unsigned int index, const char *value);

/* =======================
   DBParams API
   ======================= */

DBParams *db_params_create(unsigned int count);
void db_params_free(DBParams *p);

void db_params_set_int(DBParams *p, unsigned int idx, int value);
void db_params_set_double(DBParams *p, unsigned int idx, double value);
void db_params_set_string(DBParams *p, unsigned int idx, const char *value);

/* =======================
   DBResult API
   ======================= */

DBResult *db_result_create(unsigned int count);
void db_result_free(DBResult *r);

void db_result_set_int(DBResult *r, unsigned int idx, int *value);
void db_result_set_double(DBResult *r, unsigned int idx, double *value);
void db_result_set_string(DBResult *r, unsigned int idx, char *value, unsigned long size);

int db_get_max_id(MYSQL *conn, const char *table);

#endif /* MYSQL_WRAPPER_H */
