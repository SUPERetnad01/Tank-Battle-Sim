#include "precomp.h"
#include "LinkedList.h"
LinkedList::LinkedList(){
    head = nullptr;
}

LinkedList::~LinkedList()
{
    delete head;
}

void LinkedList::InsertValue(int tankinput)
{
    auto* new_node = new LinkedListnode(tankinput);
     
    new_node->data = tankinput;
    new_node->next = nullptr;
    if (head == nullptr || tankinput <= head->data)
    {
        new_node->next = head;
        head = new_node;
        return;
    }
    auto current = head;
    while (current->next != nullptr && new_node->data >= current->next->data)
    {
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
 
}

LinkedListnode::LinkedListnode() : data(0), next(nullptr)
{

}
LinkedListnode::LinkedListnode(int value) : data(value), next(nullptr)
{
}


LinkedListnode::~LinkedListnode()
{ 
    delete next;
}
