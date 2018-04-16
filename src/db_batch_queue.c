//
// Created by rui on 4/13/18.
//

#include <memory.h>

#include "utils.h"
#include "db_batch_queue.h"

batchQueue* bq;

//refine batch queue
batchQueue* bqr;

bqNode* create_node(DbOperator *query) {
    bqNode* node = (bqNode*) malloc(sizeof(bqNode));
    if(node == NULL) {
        log_err("create batch queue node failed.\n");
        return NULL;
    }
    node->query = query;
    node->next = NULL;
    node->share_query_handle = (char *)malloc((sizeof(query->operator_fields.select_operator.handle)+1)* sizeof(char));
    strcpy(node->share_query_handle, query->operator_fields.select_operator.handle);
    return node;
}

int create_batch_queue() {
    bq = (batchQueue*) malloc(sizeof(batchQueue));
    if(bq == NULL) {
        return 1;
    }
    bqNode* node = (bqNode*) malloc(sizeof(bqNode));
    if(node == NULL){
        free(bq);
        return 1;
    }
    bq->head = node;
    bq->tail = node;
    bq->length = 0;
    return 0;
}

/*
 * when adding query into batch queue, the query will be sorted according to select range
 */
int add_batch_queue(bqNode* node) {
    if(bq == NULL || node == NULL) {
        log_err("The queue or the adding node is null.\n");
        return 1;
    }
    bq->tail->next = node;
    bq->tail = node;
    bq->length++;
    return 0;
}

void show_batch_query() {
    if (is_bq_empty() == 0) {
        return;
    }
    bqNode *node = bq->head->next;
    while (node != NULL) {
        printf("query: %s, %s, %s.\n", node->query->operator_fields.select_operator.select_col,
               node->query->operator_fields.select_operator.pre_range,
               node->query->operator_fields.select_operator.post_range);
        node = node->next;
    }
}

int is_bq_empty() {
    if(bq == NULL) {
        return 0;
    }
    if(bq->head == bq->tail) {
        return 0;
    }
    else {
        return 1;
    }
}