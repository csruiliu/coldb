/* 
 * This file contains methods necessary to parse input from the client.
 * Mostly, functions in parse.c will take in string input and map these
 * strings into database operators. This will require checking that the
 * input from the client is in the correct format and maps to a valid
 * database operator.
 */

#define _DEFAULT_SOURCE

#include <string.h>

#include "db_fds.h"
#include "db_kvs.h"
#include "parse.h"
#include "utils.h"

char* next_token_comma(char **tokenizer, message_status *status) {
    char* token = strsep(tokenizer, ",");
    if (token == NULL) {
        *status= INCORRECT_FORMAT;
    }
    return token;
}

char* next_token_period(char **tokenizer, message_status *status) {
    char* token = strsep(tokenizer, ".");
    if (token == NULL) {
        *status= INCORRECT_FORMAT;
    }
    return token;
}

DbOperator* error_dbo(char* error_info) {
    DbOperator* dbo = malloc(sizeof(DbOperator));
    dbo->type = ERROR_CMD;
    dbo->operator_fields.err_cmd_operator.err_info = malloc((strlen(error_info)+1)* sizeof(char));
    strcpy(dbo->operator_fields.err_cmd_operator.err_info, error_info);
    return dbo;
}

DbOperator* parse_create_col(char* query_command) {
    message_status status = OK_DONE;
    DbOperator* dbo;
    char** create_arguments_index = &query_command;
    char* col_name = next_token_comma(create_arguments_index, &status);
    col_name = trim_quotes(col_name);
    char* full_tbl_name = next_token_comma(create_arguments_index,&status);
    if (status == INCORRECT_FORMAT) {
        log_err("create column command is error\n");
        dbo = error_dbo("create column command is error, use command like [create(col,\"col_name\",full_tbl_name)]");
        return dbo;
    }
    int last_char = (int)strlen(full_tbl_name) - 1;
    if (full_tbl_name[last_char] != ')') {
        log_err("create column command is error\n");
        dbo = error_dbo("create column command is error, use command like [create(col,\"col_name\",full_tbl_name)]");
        return dbo;
    }
    full_tbl_name[last_char] = '\0';
    dbo = malloc(sizeof(DbOperator));
    dbo->type = CREATE_COL;
    dbo->operator_fields.create_col_operator.tbl_name = malloc((strlen(full_tbl_name)+1)* sizeof(char));
    strcpy(dbo->operator_fields.create_col_operator.tbl_name,full_tbl_name);
    dbo->operator_fields.create_col_operator.col_name = malloc((strlen(col_name)+strlen(full_tbl_name)+2)* sizeof(char));
    strcpy(dbo->operator_fields.create_col_operator.col_name,full_tbl_name);
    strcat(dbo->operator_fields.create_col_operator.col_name,".");
    strcat(dbo->operator_fields.create_col_operator.col_name,col_name);
    return dbo;
}

DbOperator* parse_create_tbl(char* query_command) {
    message_status status = OK_DONE;
    DbOperator* dbo;
    char* tbl_name = next_token_comma(&query_command, &status);
    char* db_name = next_token_comma(&query_command, &status);
    char* col_cnt = next_token_comma(&query_command, &status);
    if (status == INCORRECT_FORMAT) {
        log_err("create table command is error\n");
        dbo = error_dbo("create table command is error, use command like [create(tbl,\"grades\",name,2)]");
        return dbo;
    }
    tbl_name = trim_quotes(tbl_name);
    int last_char = (int)strlen(col_cnt) - 1;
    if (col_cnt[last_char] != ')') {
        log_err("create table command is error\n");
        dbo = error_dbo("create table command is error, use command like [create(tbl,\"grades\",name,2)]");
        return dbo;
    }
    col_cnt[last_char] = '\0';
    size_t column_cnt = atoi(col_cnt);
    if (column_cnt < 1) {
        log_err("query unsupported, wrong column number\n");
        dbo = error_dbo("query unsupported, wrong column number");
        return dbo;
    }
    dbo = malloc(sizeof(DbOperator));
    dbo->type = CREATE_TBL;
    dbo->operator_fields.create_tbl_operator.db_name = malloc((strlen(db_name)+1)* sizeof(char));
    strcpy(dbo->operator_fields.create_tbl_operator.db_name,db_name);
    dbo->operator_fields.create_tbl_operator.tbl_name = malloc((strlen(tbl_name)+strlen(db_name)+2)* sizeof(char));
    strcpy(dbo->operator_fields.create_tbl_operator.tbl_name,db_name);
    strcat(dbo->operator_fields.create_tbl_operator.tbl_name,".");
    strcat(dbo->operator_fields.create_tbl_operator.tbl_name,tbl_name);
    dbo->operator_fields.create_tbl_operator.col_count = column_cnt;
    return dbo;
}

/**
 * parse_create parses a create statement and then passes the necessary arguments off to the next function
 **/
DbOperator* parse_create_db(char* query_command) {
    DbOperator* dbo;
    char* db_name = trim_quotes(query_command);
    int last_char = (int)strlen(db_name) - 1;
    if (last_char < 0 || db_name[last_char] != ')') {
        log_err("create database command is error.\n");
        dbo = error_dbo("create database command is error, use command like [create(db,\"name\")]");
        return dbo;
    }
    db_name[last_char] = '\0';
    dbo = malloc(sizeof(DbOperator));
    dbo->type = CREATE_DB;
    dbo->operator_fields.create_db_operator.db_name = malloc((strlen(db_name)+1)*sizeof(char));
    strcpy(dbo->operator_fields.create_db_operator.db_name,db_name);
    return dbo;
}

DbOperator* parse_load(char* query_command) {
    DbOperator* dbo;
    char* data_path = trim_quotes(query_command);
    int last_char = (int)strlen(data_path) - 1;
    if (last_char < 0 || data_path[last_char] != ')') {
        log_err("load data command is error.\n");
        dbo = error_dbo("load data command is error, use command like [load(\"data_path\")]");
        return dbo;
    }
    data_path[last_char] = '\0';
    dbo = malloc(sizeof(DbOperator));
    dbo->type = LOAD;
    dbo->operator_fields.load_operator.data_path = malloc((strlen(data_path)+1)*sizeof(char));
    strcpy(dbo->operator_fields.load_operator.data_path,data_path);
    return dbo;
}


/**
 * parse_insert reads in the arguments for a create statement and 
 * then passes these arguments to a database function to insert a row.
 **/

DbOperator* parse_insert(char* query_command, message* send_message) {
    /*
    unsigned int columns_inserted = 0;
    char* token = NULL;
    // check for leading '('
    if (strncmp(query_command, "(", 1) == 0) {
        query_command++;
        char** command_index = &query_command;
        // parse table input
        char* table_name = next_token(command_index, &send_message->status);
        if (send_message->status == INCORRECT_FORMAT) {
            return NULL;
        }
        // lookup the table and make sure it exists.
        Table* insert_table = lookup_table(table_name);
        if (insert_table == NULL) {
            send_message->status = OBJECT_NOT_FOUND;
            return NULL;
        }
        // make insert operator.
        DbOperator* dbo = malloc(sizeof(DbOperator));
        dbo->type = INSERT;
        dbo->operator_fields.insert_operator.table = insert_table;
        dbo->operator_fields.insert_operator.values = malloc(sizeof(int) * insert_table->col_count);
        // parse inputs until we reach the end. Turn each given string into an integer.
        while ((token = strsep(command_index, ",")) != NULL) {
            int insert_val = atoi(token);
            dbo->operator_fields.insert_operator.values[columns_inserted] = insert_val;
            columns_inserted++;
        }
        // check that we received the correct number of input values
        if (columns_inserted != insert_table->col_count) {
            send_message->status = INCORRECT_FORMAT;
            free (dbo);
            return NULL;
        }
        return dbo;
    } else {
        send_message->status = UNKNOWN_COMMAND;
        return NULL;
    }
     */
}

/**
 * parse_command takes as input the send_message from the client and then
 * parses it into the appropriate query. Stores into send_message the
 * status to send back.
 * Returns a db_operator.
 **/
DbOperator* parse_command(char* query_command, message* send_message, int client_socket, ClientContext* context) {
    DbOperator *dbo = NULL;

    if (strncmp(query_command, "--", 2) == 0) {
        send_message->status = OK_DONE;
        // The -- signifies a comment line, no operator needed.  
        return NULL;
    }

    char *equals_pointer = strchr(query_command, '=');
    char *handle = query_command;
    if (equals_pointer != NULL) {
        // handle exists, store here. 
        *equals_pointer = '\0';
        cs165_log(stdout, "FILE HANDLE: %s\n", handle);
        query_command = ++equals_pointer;
    } else {
        handle = NULL;
    }
    cs165_log(stdout, "QUERY: %s\n", query_command);
    send_message->status = OK_WAIT_FOR_RESPONSE;
    query_command = trim_whitespace(query_command);
    // check what command is given. 
    if (strncmp(query_command, "create(db,", 10) == 0) {
        query_command += 10;
        dbo = parse_create_db(query_command);
    }
    else if (strncmp(query_command, "create(tbl,", 11) == 0) {
        query_command += 11;
        dbo = parse_create_tbl(query_command);
    }
    else if(strncmp(query_command, "create(col,", 11) == 0) {
        query_command += 11;
        dbo = parse_create_col(query_command);
    }
    else if (strncmp(query_command, "load(", 5) == 0) {
        query_command += 5;
        dbo = parse_load(query_command);
    }
    else if (strncmp(query_command, "relational_insert", 17) == 0) {
        query_command += 17;
        dbo = parse_insert(query_command, send_message);
    }
    else if (strncmp(query_command, "shutdown", 8) == 0) {
        dbo = malloc(sizeof(DbOperator));
        dbo->type = SHUTDOWN;
    }
    else {
        log_err("[parse.c/parse_command] error command.\n");
        dbo = error_dbo("error command, please try again.\n");
    }
    dbo->client_fd = client_socket;
    dbo->context = context;
    return dbo;
}