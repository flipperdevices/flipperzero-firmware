#include <stdlib.h>

#include "linked_list.h"

int getLength(const struct ListNode_t* root) {
    int count = 0;
    while(root) {
        root = root->next;
        count += 1;
    }

    return count;
}

LinkedListStatus
    createNode(struct ListNode_t** emptyNode, struct ListNode_t* previousNode, void* data) {
    *emptyNode = malloc(sizeof(struct ListNode_t));
    if(*emptyNode == 0) {
        return LIST_CANT_ALLOCATE;
    }
    (*emptyNode)->data = data;
    (*emptyNode)->previous = previousNode;
    return LIST_OK;
}

LinkedListStatus addNode(struct ListNode_t** root, void* data) {
    // If there is no root node, add that first
    if(*root == 0) {
        return createNode(root, 0, data);
    }

    // Iterate until we find an empty node
    struct ListNode_t* base = *root;
    while(base->next) {
        base = base->next;
    }
    return createNode(&base->next, base, data);
}

LinkedListStatus removeNode(struct ListNode_t** root, const void* data) {
    if(*root == 0) {
        return LIST_NO_NODE;
    }

    struct ListNode_t* base = *root;
    while(base->data != data && base->next) {
        base = base->next;
    }

    // Delete node if data is matching
    if(base->data == data) {
        (base->previous)->next = base->next;
        (base->next)->previous = base->previous;
        free(base);
        return LIST_OK;
    }
    return LIST_NO_NODE;
}

LinkedListStatus removeAllNodes(struct ListNode_t** root) {
    struct ListNode_t* base = *root;
    struct ListNode_t* temp;
    while(base) {
        temp = base;
        base = base->next;
        free(temp);
    }
    (*root) = NULL;

    return LIST_OK;
}
