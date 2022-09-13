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


//TODO -- B TREE VERSION AS WELL AS LINKED LIST VERSION
//chained hash table data structure
template<typename Tobj, typename Tkey>
class MJA_HashTable {

private:

    //Hash Table Entry Class should only be manipulated only by the hash table or link list that stores the entries
    class HashTableEntry {

    friend class MJA_HashTable<Tobj, Tkey>;
    friend class MJA_NodeLL<HashTableEntry>;

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


    MJA_LinkedList<HashTableEntry>* table; //dynamically assigned table of linked lists to store chained objects
    int length; //size of table
    bool safeDestruction; //if true then stored items are not destroyed when the table is destroyed
    int (*hashFunction)(Tkey); //user set hash function
    static int defaultHash(unsigned int key){return ((int)key)%1033;}; //default hash function if user doesn't enter one

    //forces a safety catch to the hash function
    int hashFunc(Tkey key){
        int index = hashFunction(key);
        if (index < 0 || index >= length){
            index = index % length; //force index within the tables range
        }
        return index;
    };

    //returns reference to specific linked list node that the object is stored at
    MJA_NodeLL<HashTableEntry>* getTableNode(MJA_LinkedList<HashTableEntry>* ptr, Tkey key){
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
    MJA_HashTable(int length, int(*hashFunction)(Tkey), bool safeDestruction){
        this->safeDestruction = safeDestruction; //if stored items are to be deallocated when the table is
        this->length = length; //size of table
        this->hashFunction = hashFunction; //set the hash function
        this->table = new MJA_LinkedList<HashTableEntry>[length]; //define table
    };
    MJA_HashTable(int length, int(*hashFunction)(Tkey)) : MJA_HashTable(length, hashFunction, false) {}; //default setting is to destruct all stored items on exit
    MJA_HashTable() : MJA_HashTable(1033, &defaultHash, false){}; // default setting when no hash function is given (1033 is large enough prime for the kinds of implementations that I'll be using)
    MJA_HashTable(bool safeDestruction) : MJA_HashTable(1033, &defaultHash, safeDestruction) {}; //use default hash function, but can decide if to use safe destruction or not

    ~MJA_HashTable(){
        if (safeDestruction){ //allows for hash table to be destroyed without deallocating all contents (e.g., if stored objects are used else where via ptrs)
            for (int i=0; i < length;i++){
                while(!table[i].isEmpty()){
                    table[i].popFront();
                }
            }
        }
        delete[] table;
    };

    //add given a new entry into the table, if item is already bound to said key then the item is overwritten
    int add(Tobj* obj, Tkey key){
        int flag = 0;
        MJA_LinkedList<HashTableEntry>* index = &(table[hashFunc(key)]);
        MJA_NodeLL<HashTableEntry>* nodePtr = getTableNode(index, key);
        if (nodePtr != nullptr){ //item is already bound to that key, replace it destructively -- can do so non-destructively by manually poping key first
            index->remNode(nodePtr);
            flag = 1;
        }
        index->addEnd(new HashTableEntry(obj, key));
        return flag; //0 - item added, 1 - item replaced destructively
    };

    //access a stored object given the key
    Tobj* get(Tkey key){
        MJA_NodeLL<HashTableEntry>* nodePtr = getTableNode(&(table[hashFunc(key)]), key);
        if (nodePtr != nullptr){
            return nodePtr->obj->obj; //NodeLL -> HashTableEntry -> Tobj
        } else {
            return nullptr; //not found return nullptr
        }
    };

    //remove and deallocate an object if stored inside of the table
    int rem(Tkey key){
        MJA_LinkedList<HashTableEntry>* index = &(table[hashFunc(key)]);
        MJA_NodeLL<HashTableEntry>* nodePtr = getTableNode(index, key);
        if (nodePtr != nullptr){
            index->remNode(nodePtr);
            return 0; //0 - item removed successfully
        }
        return 1; //1 - item wasn't found
    };

    //remove an object from the table and return a pointer to it
    Tobj* pop(Tkey key){
        MJA_LinkedList<HashTableEntry>* index = &(table[hashFunc(key)]);
        MJA_NodeLL<HashTableEntry>* nodePtr = getTableNode(index, key);
        if (nodePtr != nullptr){
            HashTableEntry* entry = index->popNode(nodePtr); //pop entry off the linked list, deallocating the linked list node
            Tobj* output = entry->obj; //get object to return
            entry->obj = nullptr; //clear entry's ptr so object isn't destroyed
            delete entry; //deallocate memory for entry
            return output; //return object pointer
        } else {
            return nullptr; //not found return nullptr
        }
    };

};

#endif
