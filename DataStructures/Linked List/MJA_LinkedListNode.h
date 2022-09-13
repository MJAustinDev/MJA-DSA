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

#ifndef MJA_LINKEDLISTNODE_H
#define MJA_LINKEDLISTNODE_H

template <typename T>
class MJA_LinkedList; //so NodeLL<T> can access it

//node that stores objects inside of the linked list
template <typename T>
class MJA_NodeLL {

friend class MJA_LinkedList<T>;

public:
    T* obj; //public so stored object can be easily accessed and used

private:

    //constructor used for empty lists
    MJA_NodeLL(T* obj){this->obj = obj;};
    //constructor, sets own pointers, then sets neighbouring node pointers
    MJA_NodeLL(T* obj, MJA_NodeLL<T>* prev, MJA_NodeLL<T>* next) : MJA_NodeLL(obj) {
        if (prev != nullptr){
            this->prev = prev;
            this->prev->next = this;
        }
        if (next != nullptr){
            this->next = next;
            this->next->prev = this;
        }
    };

    //destructor removes object and adjusts neighbouring node pointers if needed
    ~MJA_NodeLL(){
        if (prev != nullptr){
            prev->next = next;
        }
        if (next != nullptr){
            next->prev = prev;
        }
        if (obj!=nullptr){ //allow for items to be popped off the list
            delete obj;
        }
        prev = nullptr;
        next = nullptr;
    };

    MJA_NodeLL* prev = nullptr;
    MJA_NodeLL* next = nullptr;
};



#endif
