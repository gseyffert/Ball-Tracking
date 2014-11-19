// #include <string>
#include <iostream>
#include <cstddef>
#include "../BallTracking.h"

using namespace std;

struct LLNode{
    LLNode* next;
    LLNode* prev;
    node* item;
};

class LinkedList{
public:
    LinkedList();
    ~LinkedList();
    void insertFront(node* item);
    LLNode* getNode(int index);
    void remove(LLNode* &n);
    void print();
    int length();
private:
    LLNode* head;
    int size;
};

LinkedList::LinkedList(){
    head = NULL;
    size = 0;
}

~LinkedList::LinkedList(){
    while(head != NULL){
        LLNode* cur = head;
        remove(cur);
    }
}

void LinkedList::insertFront(node* item){
    LLNode* newItem = new LLNode;
    newItem->item = item;
    if(size == 0 || head == NULL){
        head = newItem;
        newItem->next = NULL;
        newItem->prev = NULL;
    }
    else{
        head->prev = newItem;
        newItem->next = head;
        head = newItem;
    }
    size++;
}

void LinkedList::print(){
    cout << "[ ";
    LLNode* cur = head;
    while(cur != NULL){
        cout << cur->item->frameNum << ", ";
        cur = cur->next;
    }
    cout << "]" << endl;
}

LLNode* LinkedList::getNode(int index){
    int i = 0;
    LLNode* cur = head;
    while(i < size){
        if(i == index){
            return cur;
        }
        cur = cur->next;
        i++;
    }
    cerr << "Index "<< i << " out of bounds of linked list!" << endl;
    return NULL;
}

void LinkedList::remove(LLNode* &n){
    if(n == NULL){
        cerr << "item no longer in list" << endl;
        return;
    }
    if(n == head){
        head->next->prev = NULL;
        head = head->next;
    }
    else{
        if(n->prev == NULL){
            cerr << "NULL prev pointer, somethings messed up" << endl;
        }
        if(n->next != NULL){
            n->next->prev = n->prev;
            n->prev->next = n->next;
        }
        else{
            n->prev->next = NULL;
        }
    }
    delete n;
    n = NULL;
    size--;
}

int LinkedList::length(){
    return size;
}

// int main(){
//     LinkedList l;
//     l.insertFront("1");
//     l.insertFront("2");
//     l.insertFront("3");
//     l.insertFront("4");
//     cout << "size " << l.length() << endl;
//     l.print();
//     Node* h = l.getItem(0);
//     Node* m = l.getItem(2);
//     l.remove(h);
//     l.print();
//     l.insertFront("5");
//     l.print();
//     l.remove(m);
//     l.print();
//     l.remove(h);
//     l.print();
//     return 0;
// }