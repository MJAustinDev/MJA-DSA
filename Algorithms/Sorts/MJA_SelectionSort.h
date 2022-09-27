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

#ifndef MJA_SELECTIONSORT_H
#define MJA_SELECTIONSORT_H

//selection sort
template <typename T>
void MJA_SelectionSort(T* data, int length, bool(*sortOp)(T, T)){ //sorting operation takes form (left most item, right most item)

    //scan up from index 0 to 2nd to last element of array
    for (int j=0; j<length-1; j++){
        int index = j; //assume data[j] is the value for the index position
        //scan up from j+1 to the last element of the array
        for (int i=j+1; i<length; i++){
            if (sortOp(data[index], data[i])){ //data[i] has triggered the sorting operation condition
                index = i; //set i as the new swapping index
            }
        }
        swap(data[j], data[index]); //swap original with the element that should be at j
    }
}

#endif
