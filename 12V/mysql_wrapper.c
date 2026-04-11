#include "mysql_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

static void trim_newline(char *s) {
    s[strcspn(s, "\r\n")] = 0;
}

int load_db_config(const char *path, DBConfig *config) {
    FILE *file = fopen(path, "r");
    if (!file) return 0;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        if (strcmp(key, "host") == 0)
            strncpy(config->host, value, sizeof(config->host));
        else if (strcmp(key, "user") == 0)
            strncpy(config->user, value, sizeof(config->user));
        else if (strcmp(key, "password") == 0)
            strncpy(config->password, value, sizeof(config->password));
        else if (strcmp(key, "database") == 0)
            strncpy(config->database, value, sizeof(config->database));
        else if (strcmp(key, "port") == 0)
            config->port = (unsigned int)atoi(value);
    }
    fclose(file);
    return 1;
}

MYSQL *db_connect(const DBConfig *config) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) return NULL;
    if (!mysql_real_connect(conn, config->host, config->user, config->password, config->database, config->port, NULL, 0)) {
        mysql_close(conn);
        return NULL;
    }
    return conn;
}

void db_close(MYSQL *conn) {
    if (conn) mysql_close(conn);
}

MYSQL_STMT *db_prepare(MYSQL *conn, const char *query) {
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) return NULL;
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        mysql_stmt_close(stmt);
        return NULL;
    }
    return stmt;
}

int db_exec_stmt(MYSQL_STMT *stmt) {
    return mysql_stmt_execute(stmt) == 0;
}

void db_stmt_close(MYSQL_STMT *stmt) {
    if (stmt) mysql_stmt_close(stmt);
}

DBResult *db_select(MYSQL *conn, const char *query) {
    DBResult *r = calloc(1, sizeof(DBResult));
    if (!r) return NULL;
    r->stmt = db_prepare(conn, query);
    if (!r->stmt) {
        free(r);
        return NULL;
    }
    if (!db_exec_stmt(r->stmt)) {
        db_stmt_close(r->stmt);
        free(r);
        return NULL;
    }
    r->columns = mysql_stmt_field_count(r->stmt);
    return r;
}

int db_select_bind_result(DBResult *res, MYSQL_BIND *bind) {
    if (!res || !res->stmt) return 0;
    res->result_bind = bind;
    if (mysql_stmt_bind_result(res->stmt, bind)) {
        return 0;
    }
    mysql_stmt_store_result(res->stmt);
    return 1;
}

int db_select_fetch(DBResult *res) {
    if (!res || !res->stmt) return -1;
    return mysql_stmt_fetch(res->stmt);
}

void db_select_free(DBResult *res) {
    if (!res) return;
    if (res->stmt) mysql_stmt_close(res->stmt);
    free(res);
}

int db_bind_int(MYSQL_STMT *stmt, unsigned int index, int value) {
    MYSQL_BIND bind;
    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = MYSQL_TYPE_LONG;
    bind.buffer = &value;
    return mysql_stmt_bind_param(stmt, &bind) == 0;
}

int db_bind_double(MYSQL_STMT *stmt, unsigned int index, double value) {
    MYSQL_BIND bind;
    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = MYSQL_TYPE_DOUBLE;
    bind.buffer = &value;
    return mysql_stmt_bind_param(stmt, &bind) == 0;
}

int db_bind_string(MYSQL_STMT *stmt, unsigned int index, const char *value) {
    MYSQL_BIND bind;
    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = (char *)value;
    bind.buffer_length = strlen(value);
    return mysql_stmt_bind_param(stmt, &bind) == 0;
}

int db_stmt_bind(MYSQL_STMT *stmt, DBParams *p) {
    return mysql_stmt_bind_param(stmt, p->bind) == 0;
}

DBParams *db_params_create(unsigned int count) {
    DBParams *p = calloc(1, sizeof(DBParams));
    if (!p) return NULL;
    p->count = count;
    p->bind = calloc(count, sizeof(MYSQL_BIND));
    return p;
}

void db_params_free(DBParams *p) {
    if (!p) return;
    free(p->bind);
    free(p);
}

void db_params_set_int(DBParams *p, unsigned int idx, int value) {
    MYSQL_BIND *b = &p->bind[idx];
    memset(b, 0, sizeof(MYSQL_BIND));
    b->buffer_type = MYSQL_TYPE_LONG;
    b->buffer = malloc(sizeof(int));
    *(int *)b->buffer = value;
    b->is_null = 0;
}

void db_params_set_double(DBParams *p, unsigned int idx, double value) {
    MYSQL_BIND *b = &p->bind[idx];
    memset(b, 0, sizeof(MYSQL_BIND));
    b->buffer_type = MYSQL_TYPE_DOUBLE;
    b->buffer = malloc(sizeof(double));
    *(double *)b->buffer = value;
    b->is_null = 0;
}

void db_params_set_string(DBParams *p, unsigned int idx, const char *value) {
    MYSQL_BIND *b = &p->bind[idx];
    memset(b, 0, sizeof(MYSQL_BIND));
    b->buffer_type = MYSQL_TYPE_STRING;
    b->buffer = strdup(value);
    b->buffer_length = strlen(value);
    b->is_null = 0;
}

DBResult *db_result_create(unsigned int count) {
    DBResult *r = calloc(1, sizeof(DBResult));
    if (!r) return NULL;
    r->count = count;
    r->bind = calloc(count, sizeof(MYSQL_BIND));
    return r;
}

void db_result_free(DBResult *r) {
    if (!r) return;
    free(r->bind);
    free(r);
}

void db_result_set_int(DBResult *r, unsigned int idx, int *value) {
    MYSQL_BIND *b = &r->bind[idx];
    memset(b, 0, sizeof(MYSQL_BIND));
    b->buffer_type = MYSQL_TYPE_LONG;
    b->buffer = value;
}

void db_result_set_double(DBResult *r, unsigned int idx, double *value) {
    MYSQL_BIND *b = &r->bind[idx];
    memset(b, 0, sizeof(MYSQL_BIND));
    b->buffer_type = MYSQL_TYPE_DOUBLE;
    b->buffer = value;
}

void db_result_set_string(DBResult *r, unsigned int idx, char *value, unsigned long size) {
    MYSQL_BIND *b = &r->bind[idx];
    memset(b, 0, sizeof(MYSQL_BIND));
    b->buffer_type = MYSQL_TYPE_STRING;
    b->buffer = value;
    b->buffer_length = size;
}

int db_stmt_bind_result(MYSQL_STMT *stmt, DBResult *r) {
    return mysql_stmt_bind_result(stmt, r->bind) == 0;
}

int db_get_max_id(MYSQL *conn, const char *table) {
    if (!conn || !table) return -1;
    char query[512];
    // einfache Absicherung gegen kaputte Strings (kein SQL-Injection-Schutz!)
    snprintf(query, sizeof(query), "SELECT MAX(id) FROM %s", table);
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) return -1;
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        mysql_stmt_close(stmt);
        return -1;
    }
    if (mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }
    int max_id = -1;
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &max_id;
    if (mysql_stmt_bind_result(stmt, bind)) {
        mysql_stmt_close(stmt);
        return -1;
    }
    if (mysql_stmt_fetch(stmt) != 0) {
        mysql_stmt_close(stmt);
        return -1;
    }
    mysql_stmt_close(stmt);
    return max_id;
}
