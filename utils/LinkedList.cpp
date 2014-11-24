#ifndef LINKEDLIST_DEF
#define LINKEDLIST_DEF

#include <string>
#include <iostream>

using namespace std;

template <typename T> struct LLNode{
    LLNode<T>* next;
    LLNode<T>* prev;
    T item;
};

template <typename T> class LinkedList{
public:
    LinkedList();
    ~LinkedList();
    void insertFront(T item);
    LLNode<T>* getNode(int index);
    void remove(LLNode<T>* &n);
    void print();
    int length();
private:
    LLNode<T>* head;
    int size;
};

template<typename T> LinkedList<T>::LinkedList(){
    head = NULL;
    size = 0;
}

template<typename T> LinkedList<T>::~LinkedList(){
    while(head != NULL){
        LLNode<T>* cur = head;
        remove(cur);
        // cout << "deleting head" << endl;
    }
}

template<typename T> void LinkedList<T>::insertFront(T item){
    LLNode<T>* newItem = new LLNode<T>;
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

template<typename T> void LinkedList<T>::print(){
    cout << "[ ";
    LLNode<T>* cur = head;
    while(cur != NULL){
        cout << cur->item << ", ";
        cur = cur->next;
    }
    cout << "]" << endl;
}

template<typename T> LLNode<T>* LinkedList<T>::getNode(int index){
    int i = 0;
    LLNode<T>* cur = head;
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

template<typename T> void LinkedList<T>::remove(LLNode<T>* &n){
    if(n == NULL){
        cerr << "item no longer in list" << endl;
        return;
    }
    if(n == head){
        if(size != 1){
            head->next->prev = NULL;
            head = head->next;
        }else{
            head = NULL;
        }
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

template<typename T> int LinkedList<T>::length(){
    return size;
}

// int main(){
//     LinkedList<string>* l = new LinkedList<string>;
//     l->insertFront("1");
//     l->insertFront("2");
//     l->insertFront("3");
//     l->insertFront("4");
//     cout << "size " << l->length() << endl;
//     l->print();
//     LLNode<string>* h = l->getNode(0);
//     LLNode<string>* m = l->getNode(2);
//     l->remove(h);
//     l->print();
//     l->insertFront("5");
//     l->print();
//     l->remove(m);
//     l->print();
//     l->remove(h);
//     l->print();
//     delete l;
//     // cout << "done" << endl;
//     return 0;
// }

#endif