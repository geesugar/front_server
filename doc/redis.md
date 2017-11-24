[TOC]

# Redis

## List
```cpp
typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;
```
