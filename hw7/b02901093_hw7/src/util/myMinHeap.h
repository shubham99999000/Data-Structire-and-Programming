/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   
   void insert(const Data& d) {
      int s = size();
      _data.push_back(d);           //s is last index  
      while(s != 0){
         int p = (s-1)/2;           // parent
         if(_data[s] < _data[p]){
            Data tmp = _data[s];
            _data[s] = _data[p];
            _data[p] = tmp;
            s = p;           
         } 
         else break;
      }
   }
   void delMin() {delData(0);}
   
   void delData(size_t i) {
      int p = i, lch = 2*i+1;
      int s = size();
      while(lch <= s-1){
         if(lch <= s-2){
            if( _data[lch+1] < _data[lch] ) lch +=1;
         }
         if(_data[s-1] < _data[lch]) break;
         _data[p] = _data[lch];
         p = lch;
         lch = 2*p+1;
      }
      _data[p] = _data[s-1];
      _data.pop_back();
   }


private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
