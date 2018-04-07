/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted(false) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { 
         ++_node;
         return (*this); }
      iterator operator ++ (int) { 
         iterator i(_node);
         ++(*this); 
         return i; }
      iterator& operator -- () {
         --_node;
         return (*this); }
      iterator operator -- (int) {
         iterator i(_node);
         --(*this); 
         return i; }

      iterator operator + (int i) const { 
         iterator it(_node+i);
         return it; }
      iterator& operator += (int i) { 
         _node += i;
         return (*this); }

      iterator& operator = (const iterator& i) { 
         _node = i._node;
         return (*this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
      if(empty()) return end();
      else{
         iterator i(_data);
         return i;
      }
   }
   iterator end() const { 
      if(_capacity == 0) return 0;
      else{
         iterator i(_data+ _size);
         return i;
      }
   }
   bool empty() const {  
      if(!size()) return true;
      else return false;
   }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      //cout << "haha" << endl;
      if(_data == 0){
         _size = 1;
         _capacity = 1;
         _data = new T [1];
         _data[0] = x;
      }
      else if(_size == _capacity){
         //cout << "fdjif" << endl;
         _capacity = _capacity*2;
         T* _tempCapa = new T [_capacity];
         for(size_t i = 0; i < _size; i++)                        // not sure
            _tempCapa[i] = _data[i];
         delete [] _data;
         _data = _tempCapa;
         _data[_size] = x;
         _size++;
      }
      else {
         _data[_size] = x;
         _size++;
      }
   }
   void pop_front() {
      if (empty()) return;
      if(size() >= 2)
         _data[0] = _data[_size-1];
      _size--;
   }
   void pop_back() {
      if (empty()) return;
      _size --;
   }

   bool erase(iterator pos) { 
      if (empty()) return false; 
      size_t posSize = size_t(pos._node - _data);
      _data[posSize] = _data[_size-1];
      _size --;
      return true;
   }
   bool erase(const T& x) {
      if (empty()) return false;
      for(size_t i = 0; i < _size; i++){
         if(x == _data[i]){
            _data[i] = _data[_size-1];
            _size --;
            return true;
         }
      }  
      return false;
   }

   void clear() {_size = 0;}

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
