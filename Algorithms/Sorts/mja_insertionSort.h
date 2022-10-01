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

#ifndef MJA_INSERTIONSORT_H
#define MJA_INSERTIONSORT_H

//insertion sort, faster as less elements get swapped
template <typename T>
void mja_insertionSort(T* data, int length, bool(*sortOp)(T, T)){ //sorting operation takes form (left most item, right most item)

    //scan up from index 1 to end of array
    for (int j=1; j<length; j++){
        T temp = data[j];
        int i;
        //scan down from below j until position is found
        for (i=j-1; ((i>=0) && (sortOp(data[i], temp))); i--){
            data[i+1] = data[i]; //push data[i] forward in the array as it's larger than the original data[j]
        }
        data[i+1] = temp; //insert original data[j] value into it's correct place
    }

}

//slower as each triggered comparison sets 2 locations in memory rather than just 1
template <typename T>
void mja_insertionSortSlower(T* data, int length, bool(*sortOp)(T, T)){ //sorting operation takes form (left most item, right most item)

    //scan up from index 1 to end of array
    for (int j=1; j<length; j++){
        //scan down from below j until position is found
        for (int i=j; (i>0) && (sortOp(data[i-1], data[i])); i--){
            swap(data[i-1], data[i]); //swap as data[i-1] and data[i] are in the wrong place
        }
    }
}



#endif
