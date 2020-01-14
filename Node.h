#pragma once
class Node
{
  public:
    Node() : value(0), next(nullptr) {}
    Node(int value) : value(value), next(nullptr) {}

    int value;
    Node* next;
};
