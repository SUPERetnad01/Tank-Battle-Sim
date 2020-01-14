#pragma once
class node
{
  public:
    node();
    node(int value);
    ~node();
    int data;
    node* next;
};
class LinkedList
{
  public:
    node* head;

    LinkedList();
    ~LinkedList();
    void InsertValue(int value);

};
