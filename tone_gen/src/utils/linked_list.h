#ifndef _LINKED_LIST_H_

#define _LINKED_LIST_H_

typedef enum {
    LIST_OK = 0,
    LIST_CANT_ALLOCATE = -1,
    LIST_NO_NODE = -2,
} LinkedListStatus;

struct ListNode_t {
    struct ListNode_t* previous;
    struct ListNode_t* next;

    void* data;
};

int getLength(const struct ListNode_t* root);
LinkedListStatus addNode(struct ListNode_t** root, void* data);
LinkedListStatus removeNode(struct ListNode_t** root, const void* data);
LinkedListStatus removeAllNodes(struct ListNode_t** root);

#endif
