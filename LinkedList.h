#pragma once
class LinkedListnode
{
  public:
    LinkedListnode();
    LinkedListnode(int value);
    ~LinkedListnode();
    int data;
    LinkedListnode* next;
};
class LinkedList
{
  public:
    LinkedListnode* head;

    LinkedList();
    ~LinkedList();
    void InsertValue(int value);

};
