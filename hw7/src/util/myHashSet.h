/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(size_t s,vector<Data>* b) : _size(s), _b(b) {}
      iterator(const iterator& i) : _size(i._size), _b(i._b), _buk(i._buk), _slot(i._slot) {}
      ~iterator() {}

      void setBegin(){
         _buk = 0;
         while(_b[_buk].empty()) _buk ++;  
         _slot = 0;              
      }
      void setEnd(){
         _slot = 0;
         _buk = _size;     
      }
      bool allEmpty() const {
      for(int i = 1; i <= _size; i++){
         if(_b[i-1].empty() == false) return false;
      }
      return true;
      }

      iterator operator ++ (int){
         iterator i = (*this);
         ++(*this);
         return i;
      }

      iterator& operator ++ (){
         if(allEmpty()) return (*this);
         if(_buk < _size){
            _slot ++;
            if(_slot >= _b[_buk].size()){
               _buk ++;
               while(_buk < _size){
                  if(_b[_buk].empty() == false) break;
                  _buk ++;
               }
               _slot = 0;
            }
         }
         return (*this);
      }

      iterator operator -- (int){
         iterator i = (*this);
         --(*this);
         return i;
      }    

      iterator& operator -- (){           
         if(allEmpty()) return (*this);
         if(_buk != _size){                     // not sure
            if(_slot - 1 < 0){
               _buk --;
               while(_buk >= 0){
                  if(_b[_buk].empty() == false) break;
                  _buk --;
               }
               if(_buk < 0) _slot = _buk = 0;
               else         _slot = _b[_buk].size() - 1;
            }
            else            _slot --;
         }

         else{                // start from end
            _buk --;
            while(_buk >= 0){
               if(_b[_buk].empty() == false) break;
               _buk --;
            }
            _slot = _b[_buk].size() - 1;

         }
         return (*this);
      }

      iterator& operator = (const iterator& i){
         _size = i._size;
         _b = i._b;
         _slot = i._slot;
         _buk = i._buk;
      } 

      bool operator == (const iterator& i) const{
         if(_b == i._b && _buk == i._buk && _slot == i._slot) return true;
         else return false;
      }

      bool operator != (const iterator& i) const{
         if(_b != i._b || _buk != i._buk || _slot != i._slot) return true;
         else return false;
      }

      const Data& operator * () const { return _b[_buk][_slot];}
      Data& operator * () {return _b[_buk][_slot];}

   private:
      size_t _size;     // size of _b, =_numBuckets
      vector<Data>* _b; // buckets
      int _buk, _slot;  // #bucket and #slot    
   };


   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      if(empty() == true) return end();
      iterator i(_numBuckets, _buckets);
      i.setBegin();
      return i;
   }
   iterator end() const {
      iterator i(_numBuckets, _buckets);
      i.setEnd();
      return i;
   }
   // return true if no valid data
   bool empty() const {
      for(int i = 1; i <= _numBuckets; i++){
         if(_buckets[i-1].empty() == false) return false;
      }
      return true;
   }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for(int i = 1; i <= _numBuckets; i++){
         if(_buckets[i-1].empty() == false)   s = s + _buckets[i-1].size();
      }
      return s; 
    }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t b = bucketNum(d);
      for(int i = 1; i <= _buckets[b].size(); i++)
         if(d == _buckets[b][i-1]) return true;
      return false;
   }
   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t b = bucketNum(d);
      cout << _numBuckets;
      for(int i = 1; i <= _buckets[b].size(); i++)
         if(d == _buckets[b][i-1]){
            d = _buckets[b][i-1];
            return true;
         }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t b = bucketNum(d);
      for(int i = 1; i <= _buckets[b].size(); i++)
         if(d == _buckets[b][i-1]){
            _buckets[b][i-1] = d;
            return true;
         }
      insert(d);   
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      if(!check(d)){
         size_t b = bucketNum(d);
         _buckets[b].push_back(d);
         return true;     
      } 
      else return false;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      if(check(d)){
         size_t b = bucketNum(d);
         for(int i = 1; i < _buckets[b].size(); i++){
            if(d == _buckets[b][i-1]){
               for(int j = i-1; j < (_buckets[b].size() - 1); j++)
                  _buckets[b][j] = _buckets[b][j+1];
               _buckets[b].pop_back();
            }                 
         }
         return true;                                                         // not sure
      }
      else return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
