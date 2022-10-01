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

#ifndef MJA_MERGESORT_H
#define MJA_MERGESORT_H

//merge sort, left is inclusive, and right is exclusive to match with the 'length based parameters of the basic searches'
template <typename T>
void mja_mergeSort(T* data, int left, int right, bool(*sortOp)(T, T)){ //sorting operation takes form (right index, left index to keep the same sort order as the basic searches

    int range = right - left; //work out range of indices (save multiple iRight- iLeft calculations)

    //handle the base case (non-'split-able' sub-lists)
    if (range < 2){
        return;
    }

    int split = (right + left)/2; //work out the split point

    //recursively sort
    mja_mergeSort(data, left, split, sortOp);
    mja_mergeSort(data, split, right, sortOp);

    //copy the current semi-sorted data into a new working array
    T* copyData = new T[range];
    T* shift = &(data[left]); //shifts along the main data array to copy over values
    for (int i=0; i<range; i++){
        copyData[i] = *(shift++); //post increment access
    }

    //perform sort with two index heads, left and right
    int iLeft = 0; //set to start of the copy array
    int iRight = range/2; //set to half way though the copy array
    for (int i=left; i<right; i++){
        //if right index has exceeded array length then only left hand side copy data remains to be added,
        //if the left index is less than its exceeded limit, then perform a sort operation
        if ((iRight >= range) || (iLeft <(range/2) && sortOp(copyData[iRight], copyData[iLeft]))) {
            //set to front array as either right has exceeded or the sort operation has triggered this response
            data[i] = copyData[iLeft];
            iLeft++;
        } else {
            //set to back array as either left has exceeded or the sort operation has triggered this response
            data[i] = copyData[iRight];
            iRight++;
        }
    }

    delete[] copyData; //memory clean up
}



#endif
