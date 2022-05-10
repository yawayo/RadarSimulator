#ifndef OBJ_LINKED_LIST_H
#define OBJ_LINKED_LIST_H
#include "cody_Msg_Format.h"
#include <list>

#define MAX_NODE_NUM 100

struct obj_LinkedList
{
    std::list<Obj_inf> data;

    obj_LinkedList();
    ~obj_LinkedList();

    bool isEmpty() const;
    int getsize();
    const Obj_inf& getLast() const;
    void addLast(const Obj_inf& node);
    void remove_front();
    void remove_all();
};

#endif // OBJ_LINKED_LIST_H
