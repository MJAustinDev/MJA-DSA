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

#ifndef MJA_BUBBLESORT_H
#define MJA_BUBBLESORT_H

//bubble sort
template <typename T>
void MJA_BubbleSort(T* data, int length, bool(*sortOp)(T, T)){ //sorting operation takes form (left most item, right most item)

    //set j as the stopping point where sorted items are placed
    for (int j=length; j>0; j--){
        bool flag = false; //flag to indicate that no swaps occured, i.e, list in order and can stop
        //scan upwards from 1 to the stopping point (comparing i to the previous element)
        for (int i=1;i<j;i++){
            if (sortOp(data[i-1], data[i])){ //data[i-1] and data[i] have triggered the sorting operation condition
                swap(data[i-1], data[i]);
                flag = true;
            }
        }
        if (!flag){
            break;
        }
    }
}


#endif
