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


inline int defaultHash(unsigned int key){
    return (key % (unsigned int)1033);
}

//keeps all error codes for the adjacency list graph together, allows for both Vertex and Graph classes to use the same codes
class MJA_ErrorCode_GraphAdjList {

protected:

    //operation success indicator constants
    const int SUCCESS = 0; //operation was successful
    const int NOT_EXIST_SCR = 1; //source key doesn't exist in the graph
    const int NOT_EXIST_DEST = 2; //destination key doesn't exist in the graph
    const int SAME_SRC_DEST = 3; //source and destination are the same node
    const int NOT_EXIST_EDGE = 4; //edge doesn't exist (failed attempt to remove non-existent edge)
    const int RESET_EDGE = 5; //edge already existed (attempt to add an existing edge, just updates the edges weight)

};


//graph data structure represented via an adjacency list
template <typename T>
class MJA_GraphAdjList : MJA_ErrorCode_GraphAdjList {

private:

    class Vertex;

    struct Edge{
        Edge(Vertex* v, double w) {this->v = v; this->w = w;};
        Edge(Vertex* v) : Edge(v, 1.0){}; //1.0 as default weight if 'unweighted' graph
        ~Edge(){v = nullptr;};
        Vertex* v;
        double w;
    }; //END -- STRUCT EDGE

    class Vertex : MJA_ErrorCode_GraphAdjList {

    friend class MJA_GraphAdjList;
    friend class MJA_HashTable<Vertex, unsigned int>;

    private:

        Vertex(T* obj, unsigned int uniqueID){this->obj = obj; this->uniqueID = uniqueID;};
        ~Vertex(){if (obj != nullptr){delete obj;}};

        unsigned int uniqueID;
        T* obj;
        int edgeCount = 0;
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
                edgeCount--;
                return SUCCESS; //indicate successful removal of edge
            }
            return NOT_EXIST_EDGE; //indicate that the edge doesn't exist
        }
        //returns edge structure from memory heap
        Edge* getConnection(Vertex* v){
            MJA_NodeLL<Edge>* ptr = getConnectionNode(v);
            if (ptr != nullptr){
                return ptr->obj;
            }
            return nullptr; //edge doesn't exist return a nullptr
        }

    public:
        unsigned int getUniqueID(){return uniqueID;};
        int getEdgeCount(){return edgeCount;};
        unsigned int* getEdges(){
            if (connections.resetCycleUp()){
                unsigned int* output = new unsigned int[edgeCount];
                unsigned int* ptr = output;
                //set all values of output to the keys that this node is connected to
                do {
                    *(ptr++) = connections.cycle->obj->v->getUniqueID(); //set values of output via post incrementing ptr
                } while (connections.cycleUp());
                return output;
            }
            return nullptr; //no edges stored return a nullptr
        };

    }; //END -- CLASS VERTEX

    unsigned int uniqueID = 0;
    MJA_HashTable<Vertex, unsigned int> vertices;
    int tableLength;
    int (*tableFunc)(unsigned int);

    int srcDestCheck(Vertex* srcPtr, Vertex* destPtr){
        if (srcPtr == nullptr){
            return NOT_EXIST_SCR; //indicate that the source node doesn't exist
        }
        if (destPtr == nullptr){
            return NOT_EXIST_DEST; //indicate that the destination node doesn't exist
        }
        if (srcPtr == destPtr){
            return SAME_SRC_DEST; //indicate that source and destination are the same
        }
        return SUCCESS; //indicate that the basic check has passed
    }

public:

    MJA_GraphAdjList(int tableLength, int(*tableFunc)(unsigned int)) : vertices(tableLength, tableFunc, false){this->tableLength = tableLength; this->tableFunc = tableFunc;};
    MJA_GraphAdjList() : MJA_GraphAdjList(1033, &defaultHash){};
    MJA_GraphAdjList(MJA_GraphAdjList<T> &oldGraph); //copy constructor

    ~MJA_GraphAdjList(){};

    int getNodeCount(){return vertices.getKeyCount();};
    unsigned int* getKeys(){return vertices.getKeys();};

    //add a new vertex to the graph
    int addVertex(T* obj){vertices.add(new Vertex(obj, uniqueID), uniqueID); uniqueID++; return SUCCESS;}; //returns success as uniqueID will always be unique (except for unlikely overflow which will take over 4294967295 nodes to be added)
    int remVertex(unsigned int key); //remove a vertex from the graph (also remove all edges where the vertex is a destination)
    T* popVertex(unsigned int key); //remove a vertex from the graph while preserving the stored object

    //add a new edge to the graph (or update weight of pre-existing edge)
    int addEdge(unsigned int src, unsigned int dest, double weight);
    int addEdge(unsigned int src, unsigned int dest) {return addEdge(src, dest, 1.0);}; //use default weight of 1.0 (i.e., unweighted graph)

    int remEdge(unsigned int src, unsigned int dest); //removes an edge from the graph

};

/*
more lengthy/looping functions are defined below to prevent above main header getting spammed
*/

//copy constructor
template <typename T>
MJA_GraphAdjList<T> :: MJA_GraphAdjList(MJA_GraphAdjList<T> &oldGraph) : MJA_GraphAdjList(oldGraph.tableLength, &defaultHash) {

    //copy over each vertex
    unsigned int* oldKeys = oldGraph.getKeys();
    for (int i=0; i<oldGraph.getNodeCount();i++){
        Vertex* v = oldGraph.vertices.get(oldKeys[i]);
        this->vertices.add(new Vertex(new T(*(v->obj)), oldKeys[i]), oldKeys[i]); //add new node to graph
    }
    //copy over all edges
    for (int i=0;i<oldGraph.getNodeCount();i++){
        Vertex* u = oldGraph.vertices.get(oldKeys[i]);
        if (u->getEdgeCount() != 0){
            unsigned int* edges = u->getEdges(); //should never be a nullptr as getEdgeCount would return 0 if no edges existed
            for (int j=0;j<u->getEdgeCount();j++){
                Vertex* v = oldGraph.vertices.get(edges[j]);
                this->vertices.get(oldKeys[i])->addEdge(this->vertices.get(edges[j]), u->getConnection(v)->w);
            }
            delete[] edges;
        }
    }
    delete[] oldKeys;
    this->uniqueID = oldGraph.uniqueID;
};

//remove a vertex from the graph (also remove all edges where the vertex is a destination)
template <typename T>
int MJA_GraphAdjList<T> :: remVertex(unsigned int key){
    //pop vertex from hash table
    Vertex* v = vertices.pop(key);
    if (v != nullptr){ //if v doesn't exist then cant deallocate it
        //remove all incoming edges to this node
        unsigned int* nodes = getKeys();
        for (int i=0; i<getNodeCount();i++){
            remEdge(nodes[i], key);
        }
        delete[] nodes; //deallocate heap memory storing current node keys
        delete v; //deallocate heap memory storing the vertex
        return SUCCESS;
    }
    return NOT_EXIST_SCR; //vertex did not exist anyhow
};

//remove a vertex from the graph while preserving the stored object
template <typename T>
T* MJA_GraphAdjList<T> :: popVertex(unsigned int key){
    Vertex* v = vertices.get(key);
    if (v != nullptr){
        T* output = v->obj;
        v->obj = nullptr; //set object pointer to null so it wont be destroyed
        v = nullptr;
        remVertex(key); //use remVertex function as it handles incoming edge removal
        return output;
    }
    return nullptr; //vertex doesn't exist so return nullptr
}

//add a new edge to the graph (or update weight of pre-existing edge)
template <typename T>
int MJA_GraphAdjList<T> :: addEdge(unsigned int src, unsigned int dest, double weight){
    Vertex* srcPtr = vertices.get(src);
    Vertex* destPtr = vertices.get(dest);
    int check = srcDestCheck(srcPtr, destPtr);
    if (check != SUCCESS){ //if failed the src/dest key check
        return check;
    }
    //both source and destination exist, see if an edge between them exists
    Edge* edgePtr = srcPtr->getConnection(destPtr);
    if (edgePtr != nullptr){ //edge exists already
        edgePtr->w = weight; //update weight
        return RESET_EDGE; //indicate that edge already exists and that the weight has been reset
    }
    srcPtr->addEdge(destPtr, weight); //edge doesn't exist add a new edge
    srcPtr->edgeCount++;
    return SUCCESS; //indicates successful addition of new edge
};

//removes an edge from the graph
template <typename T>
int MJA_GraphAdjList<T> :: remEdge(unsigned int src, unsigned int dest){
    Vertex* srcPtr = vertices.get(src);
    Vertex* destPtr = vertices.get(dest);
    int check = srcDestCheck(srcPtr, destPtr);
    if (check != SUCCESS){ //if failed the src/dest key check
        return check;
    }
    return srcPtr->remEdge(destPtr); //function returns success/fail depending on if the edge exists or not
};







#endif
