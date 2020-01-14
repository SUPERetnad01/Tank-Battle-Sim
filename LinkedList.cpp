#include "precomp.h"
#include "LinkedList.h"
LinkedList::LinkedList(){
    head = nullptr;
}

LinkedList::~LinkedList()
{
}

void LinkedList::InsertValue(int tankinput)
{
     auto *new_node = new node(tankinput);
     
    new_node->data = tankinput;
    new_node->next = nullptr;
    if (head == nullptr || tankinput <= head->data)
    {
        new_node->next = head;
        head = new_node;
        return;
    }
    auto current = head;
    while (current->next != nullptr && current->data >= current->next->data)
    {
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
 
}

node::node() :data(0), next(nullptr)
{

}
node::node(int value) : data(value), next(nullptr)
{
}


node::~node()
{
}
