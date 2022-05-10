#include "obj_linked_list.h"

obj_LinkedList::obj_LinkedList()
{
    data.clear();
}
obj_LinkedList::~obj_LinkedList()
{
    data.clear();
}
bool obj_LinkedList::isEmpty() const
{
    return !data.size();
}
int obj_LinkedList::getsize()
{
    return data.size();
}
const Obj_inf& obj_LinkedList::getLast() const
{
    return data.back();
}
void obj_LinkedList::addLast(const Obj_inf& node)
{
    data.push_back(node);

    if(getsize() >= 100)
        remove_front();
}
void obj_LinkedList::remove_front()
{
    data.pop_front();
}
void obj_LinkedList::remove_all()
{
    data.clear();
}
