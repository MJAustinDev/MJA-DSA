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

#ifndef MJA_GRAPHADJLIST_H
#define MJA_GRAPHADJLIST_H

#include "MJA_LinkedList.h"
#include "MJA_HashTable.h"


//graph data structure represented via an adjacency list
template <typename T>
class MJA_GraphAdjList {

private:

    class Vertex;

    struct Edge{
        Edge(Vertex* v, double w) {this->v = v; this->w = w;};
        Edge(Vertex* v) : Edge(v, 1.0){}; //1.0 as default weight if 'unweighted' graph
        ~Edge(){v = nullptr;};
        Vertex* v;
        double w;
    }; //END -- STRUCT EDGE

    class Vertex {

    friend class MJA_GraphAdjList;
    friend class MJA_HashTable<Vertex, unsigned int>;

    public:

        //~Vertex(){if (obj != nullptr){delete obj;}};
        unsigned int getUniqueID(){return uniqueID;};

    private:

        Vertex(T* obj, unsigned int uniqueID){this->obj = obj; this->uniqueID = uniqueID;};
        ~Vertex(){if (obj != nullptr){delete obj;}};

        unsigned int uniqueID;
        T* obj;

        MJA_LinkedList<Edge> connections; //edges from this node to other nodes

        //returns the linked list node where the edge between this node (u) and the other node (v) exists
        MJA_NodeLL<Edge>* getConnectionNode(Vertex* v){
            if (connections.resetCycleUp()){
                do {
                    if (connections.cycle->obj->v == v){
                        return connections.cycle; //edge found return cycle node's position
                    }
                } while (connections.cycleUp());
            }
            return nullptr; //edge not found return nullptr
        }

        //add new edge
        void addEdge(Vertex* v, double w){connections.addEnd(new Edge(v, w));};
        //rem edge if exists
        int remEdge(Vertex* v){
            MJA_NodeLL<Edge>* ptr = getConnectionNode(v);
            if (ptr != nullptr) {
                connections.remNode(ptr);
                return 0; //0 indicates successful removal of edge
            }
            return 4; //4 indicates that the edge doesn't exist
        }
        //returns edge structure from memory heap
        Edge* getConnection(Vertex* v){
            MJA_NodeLL<Edge>* ptr = getConnectionNode(v);
            if (ptr != nullptr){
                return ptr->obj;
            }
            return nullptr; //edge doesn't exist return a nullptr
        }
    }; //END -- CLASS VERTEX

    unsigned int uniqueID = 0;
    MJA_HashTable<Vertex, unsigned int> vertices;
    MJA_LinkedList<unsigned int> keys; //allows for iteration over whole graph

    int srcDestCheck(Vertex* srcPtr, Vertex* destPtr){
        if (srcPtr == nullptr){
            return 1; //1 indicates that the source node doesn't exist
        }
        if (destPtr == nullptr){
            return 2; // 2 indicates that the destination node doesn't exist
        }
        if (srcPtr == destPtr){
            return 3; //3 indicates that source and destination are the same
        }
        return 0; //passes the basic check
    }

public:

    MJA_GraphAdjList(){};
    ~MJA_GraphAdjList(){};

    int getNodeCount(){return keys.getNodeCount();};
    unsigned int* getKeys(){
        if(keys.getNodeCount() == 0){
            return nullptr;
        }
        unsigned int* output = new unsigned int[keys.getNodeCount()];
        unsigned int* ptr = output;
        if (keys.resetCycleUp()){
            do {
                *(ptr++) = *(keys.cycle->obj); //use post increment via ptr to set all values of output to the values of the stored keys
            } while (keys.cycleUp());
        }
        return output;
    }

    //add a new vertex to the graph
    void addVertex(T* obj){ vertices.add(new Vertex(obj, uniqueID), uniqueID); keys.addEnd(new unsigned int (uniqueID)); uniqueID++;};

    //remove a vertex from the graph (also remove it's key store and all edges where the vertex is a destination
    void remVertex(unsigned int key){
        //pop vertex from hash table
        Vertex* v = vertices.pop(key);
        if (v != nullptr){ //if v doesn't exist inside of the graph then don't waste time processing the bellow but attempting to match with a nullptr and a key that wont exist
            MJA_NodeLL<unsigned int>* keyPtr = nullptr;
            //check for connections
            if (keys.resetCycleUp()){
                do {
                    if (*(keys.cycle->obj) == key){
                        keyPtr = keys.cycle; //remember the keys node position
                    } else {
                        vertices.get(*(keys.cycle->obj))->remEdge(v); //remove edge between u and v if it exists
                    }
                } while (keys.cycleUp());
            }
            keys.remNode(keyPtr); //safe to remove key now cycling has ended
            delete v; //deallocate heap memory storing the vertex
        }
    };

    //remove an vertex from the graph while preserving the stored object
    T* popVertex(unsigned int key){
        Vertex* v = vertices.get(key);
        if (v != nullptr){
            T* output = v->obj;
            v->obj = nullptr;
            v = nullptr;
            remVertex(key);
            return output;
        }
        return nullptr; //vertex doesn't exist so return nullptr
    }

    //add a new edge to the graph (or update weight of pre-existing edge)
    int addEdge(unsigned int src, unsigned int dest, double weight){
        Vertex* srcPtr = vertices.get(src);
        Vertex* destPtr = vertices.get(dest);
        int check = srcDestCheck(srcPtr, destPtr);
        if (check != 0){ //0 passes check
            return check; //1 source doesn't exist, 2 destination doesn't exist, 3 source equals desination
        }
        Edge* edgePtr = srcPtr->getConnection(destPtr);
        if (edgePtr != nullptr){ //edge exists already
            edgePtr->w = weight; //update weight
            return 4; //4 indicates that edge already exists and that the weight has been reset
        }
        srcPtr->addEdge(destPtr, weight); //edge doesn't exist add new edge
        return 0; //0 indicates successful addition of new edge
    };
    int addEdge(unsigned int src, unsigned int dest) {return addEdge(src, dest, 1.0);};

    //removes an edge from the graph
    int remEdge(unsigned int src, unsigned int dest){
        Vertex* srcPtr = vertices.get(src);
        Vertex* destPtr = vertices.get(dest);
        int check = srcDestCheck(srcPtr, destPtr);
        if (check != 0){ //0 passes check
            return check; //1 source doesn't exist, 2 destination doesn't exist, 3 source equals desination
        }
        return srcPtr->remEdge(destPtr); //0 for success, 4 for none existent edge
    };


};


#endif
