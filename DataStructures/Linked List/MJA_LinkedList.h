/*

MIT License

Copyright (c) 2022 Matthew James Austin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef MJA_LINKEDLIST_H
#define MJA_LINKEDLIST_H

#include "MJA_LinkedListNode.h"

//linked list data structure -- based vaguely off the one I used in my Mars Volcano Dash game
template <typename T>
class MJA_LinkedList {

public:

    MJA_LinkedList(){};
    MJA_LinkedList(MJA_LinkedList<T> &oldList); //copy constructor
    ~MJA_LinkedList(){clearAll();};
    void clearAll(); //clears the list, destroying all stored objects
    void popAll(); //clears the list, without destroying all stored objects

    bool isEmpty(){return (nodeCount==0);};
    int getNodeCount(){return nodeCount;};

    //returns a pointer to stored objects, can be dangerous if object is deleted externally, use at your own risk!
    T* getFirst(){return getAt(0);};
    T* getAt(int i, bool safe){MJA_NodeLL<T>* ptr = getNode(i, safe); if (ptr!=nullptr){return ptr->obj;} else {return nullptr;}};
    T* getAt(int i) {return getAt(i, false);};
    T* getLast(){return getAt(nodeCount);};

    //stores new objects
    void addFront(T* obj){add(obj, 0);};
    bool addAt(T* obj, int i){return add(obj, i);};
    void addEnd(T* obj){add(obj, nodeCount);};

    //destroys stored objects
    void remFront(){rem(first);};
    void remAt(int i){rem(getNode(i));};
    void remNode(MJA_NodeLL<T>* node) {rem(node);};
    void remEnd(){rem(last);};

    //pops off stored objects, safer than get
    T* popFront(){return pop(first);};
    T* popAt(int i){return pop(getNode(i));};
    T* popNode(MJA_NodeLL<T>* node) {return pop(node);};
    T* popEnd(){return pop(last);};

    //apply function to all items in an upward cycle
    template <typename Tret> void cycleUpFunc(Tret(*func)(T*)){cycleFunc(first, func);};
    template <typename Tret, typename Tpara> void cycleUpFunc(Tret(*func)(T*, Tpara*), Tpara* para){cycleFunc(first, func, para);};
    template <typename Tret, typename Tpara> void cycleUpFunc(Tret(*func)(T*, Tpara), Tpara para){cycleFunc(first, func, para);};

    //apply function to all items in a downward cycle
    template <typename Tret> void cycleDownFunc(Tret(*func)(T*)){cycleFunc(last, func);};
    template <typename Tret, typename Tpara> void cycleDownFunc(Tret(*func)(T*, Tpara*), Tpara* para){cycleFunc(last, func, para);};
    template <typename Tret, typename Tpara> void cycleDownFunc(Tret(*func)(T*, Tpara), Tpara para){cycleFunc(last, func, para);};

    //manual access cycle system, greater manual control use at your own risk!
    MJA_NodeLL<T>* cycle = nullptr;
    bool resetCycleUp(){cycle = first; return (cycle != nullptr);};
    bool cycleUp(){cycle = cycle->next; return (cycle != nullptr);};
    bool resetCycleDown(){cycle = last; return (cycle != nullptr);};
    bool cycleDown(){cycle = cycle->prev; return (cycle != nullptr);};
    /* example
    if (resetCycleUp()) {
        do {
            cycle->obj->......
        } while(cycleUp());
    }
    */

private:

    bool add(T* obj, int i); //adds new object at given index
    void rem(MJA_NodeLL<T>* node); //removes an object given the node
    T* pop(MJA_NodeLL<T>* node); //pops an object given the node
    MJA_NodeLL<T>* getNode(int i, bool safe); //gets the node from corresponding index (safety returns first/last node if index is out of range)
    MJA_NodeLL<T>* getNode(int i){return getNode(i, false);}; //default safety off, so errors are more apparent to programmer

    //apply function to all objects in linked list
    template <typename Tret> void cycleFunc(MJA_NodeLL<T>* start, Tret(*func)(T*)); //no parameters
    template <typename Tret, typename Tpara> void cycleFunc(MJA_NodeLL<T>* start, Tret(*func)(T*, Tpara), Tpara para); //single parameter
    template <typename Tret, typename Tpara> void cycleFunc(MJA_NodeLL<T>* start, Tret(*func)(T*, Tpara*), Tpara* para); //multiple parameters

    MJA_NodeLL<T>* first = nullptr;
    MJA_NodeLL<T>* last = nullptr;
    int nodeCount = 0;

};

/*
more lengthy/looping functions are defined below to prevent above main header getting spammed
*/

//copy constructor
template <typename T>
MJA_LinkedList<T> :: MJA_LinkedList(MJA_LinkedList<T> &oldList) : MJA_LinkedList<T>(){
    //use reset cycle to copy over each item in the same position
    if (oldList.resetCycleUp()){
        do {
            this->addEnd(new T(*(oldList.cycle->obj)));
        } while (oldList.cycleUp());
    }
}

//removes all items from the linked list
template <typename T>
void MJA_LinkedList<T> :: clearAll(){
    while (first != nullptr){
        remFront();
    }
}
//pops all items from the linked list, allows for lists to be destroyed without destroying all stored items
template<typename T>
void MJA_LinkedList<T> :: popAll(){
    while (first != nullptr){
        popFront();
    }
}

//adds an object to the linked list at a given index
template <typename T>
bool MJA_LinkedList<T> :: add(T* obj, int i){

    bool flagDyn = false;
    MJA_NodeLL<T>** ptr; //points to where the new node is defined (first, last, or new (in-between)
    MJA_NodeLL<T>* para[2] = {nullptr, nullptr}; //parameters define the prev and next of the new node
    if (i<=0){ //add item to the front of the list
        ptr = &first;
        para[1] = first;
    } else if (i>=nodeCount){ //add item to the end of the list
        ptr = &last;
        para[0] = last;
    } else { //add inside the list
        ptr = new MJA_NodeLL<T>*;
        para[0] = getNode(i-1); //get previous node
        para[1] = para[0]->next; //get next node
        flagDyn = true; //ptr has been created dynamically so must destroy
    }
    *ptr = new MJA_NodeLL<T>(obj, para[0], para[1]);
    if (flagDyn){
        delete ptr; //remove new pointer
        ptr = nullptr;
    } else {
        //catch for adding to empty lists
        if (first == nullptr){
            first = *ptr;
        }
        if (last == nullptr){
            last = *ptr;
        }
    }
    nodeCount++;
    return (0 <= i && i <= (nodeCount-1)); //indicate if added at the desired position or if added at the ends via catch
}

//returns a stored item from the linked list, removing the node in the process
template <typename T>
T* MJA_LinkedList<T> :: pop(MJA_NodeLL<T>* node){
    if (node != nullptr){
        T* ptr = node->obj;
        node->obj = nullptr; //prevent object being destroyed with the node
        rem(node); //destroy node object
        return ptr;
    }
    return nullptr; //non-existent node so non-existent object
}

//remove item from the linked list
template<typename T>
void MJA_LinkedList<T> :: rem(MJA_NodeLL<T>* node){
    if(node != nullptr){
        if (node==first){ //account for front of list being removed
            first = node->next;
        }
        if (node==last){ //account for end of list being removed
            last = node->prev;
        }
        delete node;
        nodeCount--;
    }
}

//returns node given it's index
template<typename T>
MJA_NodeLL<T>* MJA_LinkedList<T> :: getNode(int i, bool safe){
    if (safe){
        if (i <= 0){
            return first;
        }
        if (i > nodeCount){
            return last;
        }
    } else {
        if (i<0 || i>=nodeCount){
            return nullptr;
        }
    }

    MJA_NodeLL<T>* ptr;
    if (i < nodeCount-i){ //if (i - 0) is less than (nodeCount - i) then i is closer to 0 than nodeCount
        ptr = first;
        for (int c=0;c<i;c++){
            ptr = ptr->next;
        }
    } else {
        ptr = last;
        for (int c=nodeCount-1; c>i; c--){
            ptr = ptr->prev;
        }
    }
    return ptr;
}

//cycle functions
//apply a function with no extra parameters than the stored object
template <typename T>
template <typename Tret>
void MJA_LinkedList<T> :: cycleFunc(MJA_NodeLL<T>* start, Tret(*func)(T*)){
    if (start == nullptr){
        return;
    }
    MJA_NodeLL<T>* locCyc = start;
    while (locCyc!=nullptr){
        func(locCyc->obj);
        if (start==first){
            locCyc = locCyc->next;
        } else {
            locCyc = locCyc->prev;
        }
    }
}

//apply a function with a single parameter to the stored objects
template <typename T>
template <typename Tret, typename Tpara>
void MJA_LinkedList<T> :: cycleFunc(MJA_NodeLL<T>* start, Tret(*func)(T*, Tpara), Tpara para){
    if (start == nullptr){
        return;
    }
    MJA_NodeLL<T>* locCyc = start;
    while (locCyc!=nullptr){
        func(locCyc->obj, para);
        if (start==first){
            locCyc = locCyc->next;
        } else {
            locCyc = locCyc->prev;
        }
    }
}

//apply a function with multiple parameters (e.g. array of parameters, pointer to structure, etc)
template <typename T>
template <typename Tret, typename Tpara>
void MJA_LinkedList<T> :: cycleFunc(MJA_NodeLL<T>* start, Tret(*func)(T*, Tpara*), Tpara* para){
    if (start == nullptr){
        return;
    }
    MJA_NodeLL<T>* locCyc = start;
    while (locCyc!=nullptr){
        func(locCyc->obj, para);
        if (start==first){
            locCyc = locCyc->next;
        } else {
            locCyc = locCyc->prev;
        }
    }
}



#endif
