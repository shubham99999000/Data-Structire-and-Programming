/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() : _isSorted(false) {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator cpyNode(_node);
                                   _node = _node->_next; return cpyNode; }                          
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) {  iterator cpyNode(_node);
                                   _node = _node->_prev; return cpyNode;}

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (i._node != _node); }              
      bool operator == (const iterator& i) const { return (i._node == _node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {
      if(empty()) return end();
      else {
         iterator i(_head);
         return i;
      }
   }
   iterator end() const { 
      iterator i (_head->_prev);
      return i;
   }
   bool empty() const { 
      if (size() == 0)
         return true;
      else return false; }
   size_t size() const {  
      size_t count = 0;
      DListNode<T>* _temp = _head;
      while(_temp->_next != _head){
         _temp = _temp->_next;
         count ++;
      }
      return count; 
   }
   void push_back(const T& x) { 
      DListNode<T>* _dummy = _head->_prev;
      DListNode<T>* _temp = new DListNode<T>(x, _dummy->_prev, _dummy);
      (_dummy->_prev)->_next = _temp;
      _dummy->_prev = _temp;
      if(_dummy == _head) _head = _temp; 
   }
   void pop_front() { 
      if(empty()) return;
      DListNode<T>* _dummy = _head->_prev;
      DListNode<T>* _temp = _head->_next;
      _dummy->_next = _temp;
      _temp->_prev = _dummy;
      delete _head;
      _head = _temp;                                  // take care 'head' anytime
   }
   void pop_back() { 
      if(empty()) return;
      DListNode<T>* _dummy = _head->_prev;
      DListNode<T>* _last = _dummy->_prev;
      (_last->_prev)->_next = _dummy;
      _dummy->_prev = _last->_prev;
      delete _last;
      if(size() == 0) _head = _dummy;                 
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(empty()) return false;
      DListNode<T>* _temp = pos._node;
      if(_temp == _head) _head = _head->_next;
      deleteNode(_temp);
      return true;
   }
   bool erase(const T& x) { 
      if(empty()) return false;
      DListNode<T>* _temp = _head;
      while(_temp->_next != _head){
         if(_temp->_data == x){
            if(_temp == _head) _head = _head->_next;
            deleteNode(_temp);
            return true;
         }
         _temp = _temp->_next;
      }
      return false; 
   }

   void clear() {
      if(empty()) return;
      DListNode<T>* _dummy = _head->_prev;
      DListNode<T>* _temp = _head;
      while(_temp != _dummy){
         DListNode<T>* _t = _temp;
         deleteNode(_temp);
         _temp = _t->_next;
      }
      _head = _dummy;
      _head->_prev = _head->_next = _head;
   }  // delete all nodes except for the dummy node

   void sort() const {
      DListNode<T>* _dummy = _head->_prev;
      for(DListNode<T>* _i = _head; _i != _dummy; _i = _i->_next){
         for(DListNode<T>* _j = _i->_next; _j != _dummy; _j = _j->_next){
            T di = _i->_data;
            T dj = _j->_data;
            if(di > dj){
               _j->_data = di;
               _i->_data = dj;
            }
         }
      }
      _isSorted = true;
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void deleteNode(DListNode<T>*& _del){
      assert( _del != 0 );
      DListNode<T>* _n =  _del->_next;
      DListNode<T>* _p =  _del->_prev;
      _n->_prev = _p;
      _p->_next = _n;     
      delete _del;
   }
};

#endif // DLIST_H
