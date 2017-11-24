Table of Contents
=================

   * [Redis](#redis)
      * [List](#list)

# Redis

## List
```cpp
typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;
```
