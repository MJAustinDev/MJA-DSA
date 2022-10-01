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

#ifndef MJA_QUICKSORT_H
#define MJA_QUICKSORT_H


//configuration codes for specific quick sorts, keeps all quick sort codes bundled together
class mja_ConfigCode_QuickSort{

public:

    //partition style
    static const int PARTITION_LEFT = 0;
    static const int PARTITION_RIGHT = 1;
    static const int PARTITION_MIDDLE = 2;
    //alternative names for above partition styles
    static const int PARTITION_LOMUTO_LEFT = PARTITION_LEFT;
    static const int PARTITION_LOMUTO_RIGHT = PARTITION_RIGHT;
    static const int PARTITION_LOMUTO = PARTITION_LOMUTO_LEFT;
    static const int PARTITION_HOARE = PARTITION_MIDDLE; //technically not a full Hoare partition, but very similar
    //pivot positions
    static const int PIVOT_LEFT = 0;
    static const int PIVOT_RIGHT = 1;
    static const int PIVOT_MIDDLE = 2;
    static const int PIVOT_RANDOM = 3;
};

//quick sort, left is inclusive, and right is exclusive to match with the 'length based parameters of the basic searches'

//quick sort partition that uses the left most index as a pivot (Lomuto partition scheme)
template <typename T>
int mja_quickSortLeftPivPart(T* data, int left, int right, bool(*sortOp)(T, T)){

    //want to work out the correct pivot position, while shoving other items to the other end
    int pivot = left;
    right--; //decrement to keep in index range (better than having right-1 through out the code)
    for (int i=left+1; i<=right;i++){
        if (data[left] == data[i] || sortOp(data[left], data[i])){
            pivot++; //shift pivot along as data[i] is either equal to the pivot data or has triggered the sorting operation
        } else {
            //shove data[i] to the far right as we know that it hasn't triggered the sorting operation
            swap(data[i], data[right]);
            right--;
            i--; //derecrement i as a new unchecked value exists at data[i] now
        }

    }
    swap(data[pivot], data[left]); //place pivot data at the correct pivot point
    return pivot;
}

//quick sort partition that uses the right most index as a pivot (Lomuto partition scheme)
template <typename T>
int mja_quickSortRightPivPart(T* data, int left, int right, bool(*sortOp)(T, T)){

    //want to work out the correct pivot position, while shoving other items to the other end
    int pivot = right-1;
    for (int i=right-2; i>=left; i--){
        if ((data[right-1] == data[i]) || (sortOp(data[i], data[right-1]))){
            pivot--; //shift pivot along as data[i] is either equal to the pivot data or has triggered the sorting operation
        } else {
            //shove data[i] to the far left as we know that it hasn't triggered the sorting operation
            swap(data[i], data[left]);
            left++;
            i++; //increment i as a new unchecked value exists at data[i] now
        }
    }
    swap(data[pivot], data[right-1]);
    return pivot;
}

//quick sort partition that uses a given pivot index (Hoare partition scheme mixed with Lomuto partition scheme, allows for indexed pivots rather than pure value)
template <typename T>
int mja_quickSortSetPivPart(T* data, int left, int right, bool(*sortOp)(T, T), int pivot){

    //set scanners to both sides of the array
    int iLeft = left;
    int iRight = right-1;

    while(true){ //loop will end when one scanner index is equal to the pivot
        //scan from left until either at the pivot or a swap is triggered
        while ((iLeft != pivot) && (sortOp(data[pivot], data[iLeft]))){
            iLeft++;
        }
        //scan from right until either at the pivot or a swap is triggered
        while ((iRight != pivot) && (!sortOp(data[pivot], data[iRight]))){
            iRight--;
        }
        //if an index is at the pivot point then we know that all values behind iLeft and after iRight are sorted
        if (iLeft == pivot){
            return mja_quickSortLeftPivPart(data, pivot, iRight+1, sortOp); //sort remaining using iLeft as the pivot
        }
        if (iRight == pivot){
            return mja_quickSortRightPivPart(data, iLeft, pivot+1, sortOp); //sort remaining using iRight as the pivot
        }
        swap(data[iLeft], data[iRight]); //neither are at the pivot so swap
    }
}

#include <iostream> //needed for rand()

//quick sort itself, handles configuration codes for specific quick sorts
template <typename T>
void mja_quickSort(T* data, int left, int right, bool(*sortOp)(T, T), int partType, int pivotType){

    int pivotPoint; //starting position of the chosen element
    int pivot; //final pivot position of the sorted element
    switch (pivotType){
        case (mja_ConfigCode_QuickSort::PIVOT_LEFT) : {pivot = mja_quickSortLeftPivPart(data, left, right, sortOp); break;}
        case (mja_ConfigCode_QuickSort::PIVOT_RIGHT) : {pivot = mja_quickSortRightPivPart(data, left, right, sortOp); break;}
        case (mja_ConfigCode_QuickSort::PIVOT_MIDDLE) : {pivotPoint = (left+right)/2; break;} //set pivot point

        case (mja_ConfigCode_QuickSort::PIVOT_RANDOM) : //have random be the default
        default : {pivotPoint = (rand()%(right-left))+left; break;} //set pivot point randomly within left to right range
    }

    if (pivotType != (mja_ConfigCode_QuickSort::PIVOT_LEFT) && pivotType != (mja_ConfigCode_QuickSort::PIVOT_RIGHT)){
        switch (partType){
            //swap the wanted item to the correct pivot position, then partition to pivot element
            case (mja_ConfigCode_QuickSort::PARTITION_LEFT) : {swap(data[left], data[pivotPoint]); pivot = mja_quickSortLeftPivPart(data, left, right, sortOp); break;}
            case (mja_ConfigCode_QuickSort::PARTITION_RIGHT) : {swap(data[right-1], data[pivotPoint]); pivot = mja_quickSortRightPivPart(data, left, right, sortOp); break;}

            case (mja_ConfigCode_QuickSort::PARTITION_MIDDLE) : //have middle partition as the default
            default : {pivot = mja_quickSortSetPivPart(data, left, right, sortOp, pivotPoint); break;} //use middle partition to pivot element
        }
    }
    //sort left side if 'un-pivoted' elements remain
    if (left!=pivot){
        mja_quickSort(data, left, pivot, sortOp, partType, pivotType);
    }
    //sort right side if 'un-pivoted' elements remain
    if (right!=pivot+1){
        mja_quickSort(data, pivot+1, right, sortOp, partType, pivotType);
    }
}

//default quick sort settings
template <typename T>
void mja_quickSort(T* data, int left, int right, bool(*sortOp)(T, T)){
    mja_quickSort(data, left, right, sortOp, (mja_ConfigCode_QuickSort::PARTITION_MIDDLE), (mja_ConfigCode_QuickSort::PIVOT_RANDOM));
}


#endif
