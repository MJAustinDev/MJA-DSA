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

#ifndef MJA_HASHTABLE_H
#define MJA_HASHTABLE_H

#include <MJA_LinkedList.h>

//keeps all error codes for the hash table together, allows for derived classes to use the same codes
class mja_ErrorCode_HashTable {

protected:

    //operation success indicator constants
    const int SUCCESS = 0;
    const int KEY_OVERWRITTEN = 1;
    const int NON_EXIST_KEY = 2;

};

//TODO -- B TREE VERSION AS WELL AS LINKED LIST VERSION
//chained hash table data structure
template<typename Tobj, typename Tkey>
class mja_HashTable : mja_ErrorCode_HashTable {

private:

    //Hash Table Entry Class should only be manipulated only by the hash table or the linked list that stores the entries
    class HashTableEntry {

    friend class mja_HashTable<Tobj, Tkey>;
    friend class mja_NodeLL<HashTableEntry>;

    private:

        HashTableEntry(Tobj* obj, Tkey key){
            this->obj = obj;
            this->key = key;
        };
        ~HashTableEntry(){
            if (obj != nullptr){
                delete obj;
            }
        };

        Tobj* obj;
        Tkey key;
    }; //END -- HASH TABLE ENTRY CLASS


    mja_LinkedList<HashTableEntry>* table; //dynamically assigned table of linked lists to store chained objects
    int length; //size of table
    bool safeDestruction; //if true then stored items are not destroyed when the table is destroyed
    int (*hashFunction)(Tkey); //user set hash function
    int keyCount = 0;

    //forces a safety catch to the hash function
    int hashFunc(Tkey key){
        int index = hashFunction(key);
        if (index < 0 || index >= length){
            index = index % length; //force index within the tables range
        }
        return index;
    };

    //returns reference to specific linked list node that the object is stored at
    mja_NodeLL<HashTableEntry>* getTableNode(mja_LinkedList<HashTableEntry>* ptr, Tkey key){
        if(ptr->resetCycleUp()){
            do {
                if (ptr->cycle->obj->key == key){
                    return ptr->cycle;
                }
            } while (ptr->cycleUp());
        }
        return nullptr; //not found in the table so return nullptr
    };


public:

    //base constructor
    mja_HashTable(int length, int(*hashFunction)(Tkey), bool safeDestruction){
        this->safeDestruction = safeDestruction; //if stored items are to be deallocated when the table is
        this->length = length; //size of table
        this->hashFunction = hashFunction; //set the hash function
        this->table = new mja_LinkedList<HashTableEntry>[length]; //define table
    };
    mja_HashTable(int length, int(*hashFunction)(Tkey)) : mja_HashTable(length, hashFunction, false) {}; //default setting is to destruct all stored items on exit
    mja_HashTable(mja_HashTable<Tobj, Tkey> & oldTable); //copy constructor

    ~mja_HashTable(){
        if (safeDestruction){ //allows for hash table to be destroyed without deallocating all contents (e.g., if stored objects are used else where via ptrs)
            for (int i=0; i < length;i++){
                while(!table[i].isEmpty()){
                    table[i].popFront();
                }
            }
        }
        delete[] table;
    };

    int getKeyCount(){return keyCount;};
    Tkey* getKeys(); //returns all the stored keys, clunky for really large tables when all items are stored towards the last indicies

    Tobj* get(Tkey key); //get an object stored with a given key

    int add(Tobj* obj, Tkey key); //adds object to that key, if the key already exists then the old object is overwritten
    int rem(Tkey key);
    Tobj* pop(Tkey key);
};

/*
more lengthy/looping functions are defined below to prevent above main header getting spammed
*/

//copy constructor
template <typename Tobj,typename Tkey>
mja_HashTable<Tobj, Tkey> :: mja_HashTable(mja_HashTable<Tobj, Tkey> &oldTable) : mja_HashTable(oldTable.length, oldTable.hashFunction, oldTable.safeDestruction) {

    Tkey* keys = oldTable.getKeys(); //get all keys stored in the old table
    for (int i=0; i<oldTable.getKeyCount();i++){
        this->add(new Tobj(*(oldTable.get(keys[i]))), keys[i]); //add them one by one into the new table
    }
    delete[] keys;
};

//access a stored object given the key
template <typename Tobj,typename Tkey>
Tobj* mja_HashTable<Tobj, Tkey> :: get(Tkey key){
    mja_NodeLL<HashTableEntry>* nodePtr = getTableNode(&(table[hashFunc(key)]), key);
    if (nodePtr != nullptr){
        return nodePtr->obj->obj; //NodeLL -> HashTableEntry -> Tobj
    } else {
        return nullptr; //not found return nullptr
    }
};

//returns all the stored keys
template <typename Tobj, typename Tkey>
Tkey* mja_HashTable<Tobj, Tkey> :: getKeys(){
    if (keyCount == 0){
        return nullptr; //no keys stored to return
    }
    Tkey* output = new Tkey[keyCount];
    int kCount = 0;
    for (int i=0;i<length;i++){
        mja_LinkedList<HashTableEntry>* index = &(table[i]); //get table index at i
        //attempt to scan for entries
        if (index->resetCycleUp()){
            do {
                output[kCount] = index->cycle->obj->key;
                kCount++;
            } while (index->cycleUp());
        }
        if(kCount >= keyCount) {
            break; //found all items, no point scanning down the rest of the table
        }
    }
    return output;
};

//add given a new entry into the table, if an item is already bound to said key then the item is overwritten
template <typename Tobj,typename Tkey>
int mja_HashTable<Tobj, Tkey> :: add(Tobj* obj, Tkey key){
    int flag = SUCCESS;
    mja_LinkedList<HashTableEntry>* index = &(table[hashFunc(key)]);
    mja_NodeLL<HashTableEntry>* nodePtr = getTableNode(index, key);
    if (nodePtr != nullptr){ //item is already bound to that key, replace it destructively -- can do so non-destructively by manually poping key first
        index->remNode(nodePtr);
        flag = KEY_OVERWRITTEN;
    } else {
        keyCount++; //inc stored key count
    }
    index->addEnd(new HashTableEntry(obj, key));
    return flag; //indicate whether the key was occupied or not
};

//remove and deallocate an object if stored inside of the table
template <typename Tobj,typename Tkey>
int mja_HashTable<Tobj, Tkey> :: rem(Tkey key){
    mja_LinkedList<HashTableEntry>* index = &(table[hashFunc(key)]);
    mja_NodeLL<HashTableEntry>* nodePtr = getTableNode(index, key);
    if (nodePtr != nullptr){
        index->remNode(nodePtr);
        keyCount--; //decrement key count
        return SUCCESS; //indicate that item was removed successfully
    }
    return NON_EXIST_KEY; //indicate that item wasn't found
};

//remove an object from the table and return a pointer to it
template <typename Tobj,typename Tkey>
Tobj* mja_HashTable<Tobj, Tkey> :: pop(Tkey key){
    mja_LinkedList<HashTableEntry>* index = &(table[hashFunc(key)]);
    mja_NodeLL<HashTableEntry>* nodePtr = getTableNode(index, key);
    if (nodePtr != nullptr){
        HashTableEntry* entry = index->popNode(nodePtr); //pop entry off the linked list, deallocating the linked list node
        Tobj* output = entry->obj; //get object to return
        entry->obj = nullptr; //clear entry's ptr so object isn't destroyed
        delete entry; //deallocate memory for entry
        keyCount--; //decrement key count
        return output; //return object pointer
    } else {
        return nullptr; //not found return nullptr
    }
};


#endif
