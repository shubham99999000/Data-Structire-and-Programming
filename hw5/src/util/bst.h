/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
	friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):				
      _data(d), _left(l), _right(r) {}																		// == 0
   ~BSTreeNode(){}																								// not sure

   T              _data;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
};


template <class T>
class BSTree
{
public:
// TODO: decide the initial value for _isSorted
   BSTree(): _root(0), _size(0){     
   }
   ~BSTree() {clear();}																		
   // TODO: design your own class!!
   class iterator {
   	friend class BSTree<T>;
   public:
   	iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {
     		clear();
     		for(int j = 0; j < i.size(); j++){
     			_trace.push_back(i._trace[j]);
     			_dir.push_back(i._dir[j]);
     		}
      }
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
      	//cout << "++" << endl;
      	if (_node == 0) return *(this); 
      	else{
      		BSTreeNode<T>* n = succ(_node);
      		//cout << "n is : " << n << endl;
      		if(n != 0){
      			addTrace(n,_node);
      			_node = n;
      		}
      		// no succ
      		else{
      			//cout << "no succ" << endl;
      			if(size() > 0){
      				int i = size() - 1;
      				while(i >= 0){
      					if(_dir[i] == false) break;
      					i--;
      				}
      				if(i < 0){
                     _dir.push_back(true);
      					_trace.push_back(_node);   						
	      				_node = 0;
      				}
      				else{ 
	   					_node = _trace[i];
	      				int j = size();
	      				while(i < j){
                        _dir.pop_back();
	      					_trace.pop_back();	      					
	      					j--;
	      				} 
      				}				
      			}
      			// root
      			else{
      				_trace.push_back(_node);
     					_dir.push_back(true);
      				_node = 0;
      			}
      		}
      		return *(this);
      	}
      }
      iterator operator ++ (int) {
         ++(*this);
      	iterator ret(_node);      
         return ret; 
      }
      iterator& operator -- () { 
      	if (_node) {
      		BSTreeNode<T>* n = pred(_node);
            if(n != 0){
               addTrace(n,_node);
               _node = n;
            }
            else{
               if(size() > 0){
                  int i = size() - 1;
                  while(i >= 0){
                     if(_dir[i] == true) break;
                     i--;
                  }
                  if(i >=0){
                     _node = _trace[i];
                     int j = size();
                     while(i < j){
                        _dir.pop_back();
                        _trace.pop_back();                     
                        j--;
                     }
                  }
               }
            }
            //cout << _node->_data << endl;
            return *(this);
      	}
      	else{
            if(size() > 0){
               _node = _trace[size() - 1];
               _dir.pop_back();
               _trace.pop_back();         
            }
            //cout << _node << endl;
            return *(this);  
      	}
      	
      }
      iterator operator -- (int) {    	
         --(*this);
         iterator ret(_node);
         return ret; 
      }

      iterator& operator = (const iterator& i) {
      	clear();
     		for(int j = 0; j < i.size(); j++){
     			_trace.push_back(i._trace[j]);
     			_dir.push_back(i._dir[j]);
     		}
         _node = i._node; 
      	return *(this); }

      bool operator != (const iterator& i) const { return (i._node != _node); }
      bool operator == (const iterator& i) const { return (i._node == _node); }


		size_t size() const { return _trace.size(); } 

		void addTrace(BSTreeNode<T>* n, BSTreeNode<T>* _start){
			BSTreeNode<T>* _temp = _start;
			while(n != _temp){
				if(n->_data >= _temp->_data){
					//cout << "1";
               _dir.push_back(true);
					_trace.push_back(_temp);								
					_temp = _temp->_right;
				}
				else{
					//cout << "2";
               _dir.push_back(false);
					_trace.push_back(_temp);			          // left as false
					_temp = _temp->_left;
				}
			}	
			//cout << size() << endl;	
		}

		void clear(){
         vector<BSTreeNode<T>*> _tmpTrace;
         _trace.swap(_tmpTrace);
			vector<bool> _tmpDir;
			_dir.swap(_tmpDir);
		}      

		BSTreeNode<T>* succ(BSTreeNode<T>* n) const{
			if(n->_right == 0) return 0;
			else return min(n->_right);
		}

		BSTreeNode<T>* pred(BSTreeNode<T>* n) const{
			if(n->_left == 0) return 0;
			else return max(n->_left);
		}

		BSTreeNode<T>* min(BSTreeNode<T>* n) const{
			if(n->_left == 0) return n;
			else return min(n->_left);
		}

		BSTreeNode<T>* max(BSTreeNode<T>* n) const{
			if(n->_right == 0) return n;
			else return max(n->_right);
		} 

   private:
   	BSTreeNode<T>* _node;
   	vector<BSTreeNode<T>*> _trace;
   	vector<bool> _dir;

   };

   iterator begin() const {
   	if(empty()) return end();
   	BSTreeNode<T>* _tmpmin = min(_root);
   	iterator i(_tmpmin);
   	i.clear();
   	i.addTrace(_tmpmin, _root);
   	return i;
   }
   iterator end() const {
   	BSTreeNode<T>* _tmpmax = max(_root);
   	iterator i(_tmpmax);
   	i.clear();
   	i.addTrace(_tmpmax, _root);
   	
   	if (_tmpmax) {
         i._dir.push_back(true);
   		i._trace.push_back(_tmpmax);		
   	}
      i._node = 0;
   	return i;
   }
   bool empty() const {
   	if(!size()) return true;
      else return false;
   }
   size_t size() const {return _size;}

   void insert(const T& x) {
   	nodeInsert(x, _root);
   	//cout << "insert" << endl;
   }
   void print() {
   	//cout << "print" << endl;
   	if (empty()) return;
   	BSTreeNode<T>* _n = _root;
   	size_t depth = 0;
   	printNode(_n, depth);
   }

   void pop_front() {
   	if(empty()) return;
   	BSTreeNode<T>* _tmpmin = min(_root);
   	if(_tmpmin != _root) { 	
         getParents(_tmpmin)->_left = _tmpmin->_right;                                       
         delete _tmpmin;		
   	}
   	else{
   		_root = _tmpmin->_right;
         delete _tmpmin;
   	}
   	_size --;
   }
   void pop_back() {
   	if(empty()) return;
   	BSTreeNode<T>* _tmpmax = max(_root);
   	if(_tmpmax != _root){
   		getParents(_tmpmax)->_right = _tmpmax->_left;                                  
         delete _tmpmax;
   	}
   	else{   		
         _root = _tmpmax->_left;
         delete _tmpmax;
   	}
   	_size -- ;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(pos._node == 0) return false;
   	if(empty()) return false;
   	BSTreeNode<T>* _tmp = pos._node;
   	//cout << _tmp->_data << endl;
   	BSTreeNode<T>* _tmpPa = getParents(pos._node);
   	// 2 Childen
   	if((_tmp->_right != 0) && (_tmp->_left != 0)){
         BSTreeNode<T>* _tmpSucc = succ(_tmp);                       // =0 is impossible
         T& erasData = _tmpSucc->_data;
         iterator i(_tmpSucc);
         erase(i);
         _tmp->_data = erasData;
         return true;
   	}
   	// 0 Childen
   	else if((_tmp->_right == 0) && (_tmp->_left == 0)){
         _size --;
         if(_tmp != _root){
            if(_tmpPa->_data <= _tmp->_data)
               _tmpPa->_right = 0;
            else 
               _tmpPa->_left = 0;
         } 
         else
            _root = 0;        
         delete _tmp;
         return true;
   	}
   	// One Child
   	else{
   		//cout << "erase3" << endl;
   		BSTreeNode<T>* _tmpSucc;
         _size --;
   		if(_tmp->_right != 0)  
   			_tmpSucc = _tmp->_right;			
   		else                  
   			_tmpSucc = _tmp->_left;	

   		if(_tmp == _root)     _root = _tmpSucc;
   		else{
	   		if(_tmpPa->_data <= _tmp->_data)
	   			_tmpPa->_right = _tmpSucc;
	   		else 
	   			_tmpPa->_left = _tmpSucc;
   		}   		
   		delete _tmp;
   		return true;
   	}
   }

   bool erase(const T& x) {
   	if(empty()) return false;
   	BSTreeNode<T>* _tmp;
      _tmp = findNode(x);
   	if(_tmp == 0) return false;
   	iterator i(_tmp);
   	return erase(i); 	
   }

   void clear() {
   	if(empty()) return;
   	clearNode(_root);
   	_root = 0;
   }  // delete all nodes except for the dummy node

   void sort() const {}




private:
	BSTreeNode<T>*  _root;
	size_t  _size;

	BSTreeNode<T>* nodeInsert(const T& x, BSTreeNode<T>* n = 0){												// not sure if I should initiate     
		if(n == 0){
			n = new BSTreeNode<T>(x, 0, 0);
			if(!_size) _root = n;
			_size++;
			return n;
		}
		if(x == n->_data) {		
			BSTreeNode<T>*  _temp = new BSTreeNode<T>(x, 0, 0);
			if(n->_right == 0)
				n->_right = _temp;
			else{
				_temp->_right = n->_right;
				n->_right = _temp;			
			}
         _size++;
			return n;
		}																		
		if (x > n->_data)
			n->_right = nodeInsert(x, n->_right);
		else 		
         n->_left = nodeInsert(x, n->_left);
		return n;
	}

	BSTreeNode<T>* getParents(BSTreeNode<T>* n){
		if(n == _root) return 0;
		if(empty()) return 0;																							
		//cout << "getParents" << endl;
		BSTreeNode<T>* _temp = _root;
		while((_temp->_left != n) && (_temp->_right != n)){
			if (_temp == 0) return 0;
			if(_temp->_data >= n->_data)
				_temp = _temp->_left;
			else
				_temp = _temp->_right;
		}
		return _temp;
	}

	BSTreeNode<T>* findNode(const T& x){
		if(empty()) return 0;
		BSTreeNode<T>* _temp = _root;
		while(!(x == _temp->_data)){
			if(x >= _temp->_data)
				_temp = _temp->_right;
			else
				_temp = _temp->_left;
			if(_temp == 0) return 0;
		}
		return _temp;
	}

	void clearNode(BSTreeNode<T>* n){
		if(n->_right != 0){
			clearNode(n->_right);
			n->_right = 0;
		}
		if(n->_left != 0){
			clearNode(n->_left);
			n->_left = 0;
		}
		delete n;
		_size --;
	}

	void printNode(BSTreeNode<T>* _n, size_t depth){
   	for(int i = 0; i < depth; i++) cout << "  ";
   	cout << _n->_data << endl;
   	if(_n->_left != 0)
   		printNode(_n->_left, depth + 1);
   	// dummy child
   	else{
   		for(int i = 0; i < (depth + 1); i++) cout << "  ";
   		cout << "[0]" << endl;
   	}
   	if(_n->_right != 0){
   		printNode(_n->_right, depth+1);
   	}
   	// dummy child
   	else{
   		for(int i = 0; i < (depth + 1); i++) cout << "  ";
   		cout << "[0]" << endl;
   	}
	}


	BSTreeNode<T>* min(BSTreeNode<T>* n) const{
		if (empty()) return 0;
		if(n->_left == 0) return n;
		else return min(n->_left);
	}

	BSTreeNode<T>* max(BSTreeNode<T>* n) const{
		if (empty()) return 0;
		if(n->_right == 0) return n;
		else return max(n->_right);
	} 

	BSTreeNode<T>* succ(BSTreeNode<T>* n) const{
		if (empty()) return 0;
		if(n->_right == 0) return 0;
		else return min(n->_right);
	}

	BSTreeNode<T>* pred(BSTreeNode<T>* n) const{
		if (empty()) return 0;
		if(n->_left == 0) return 0;
		else return max(n->_left);
	}

};

#endif // BST_H
