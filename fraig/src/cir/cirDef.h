/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;

typedef vector<CirGate*>           GateList;
typedef vector<unsigned>           IdList;

enum GateType
{
   UNDEF_GATE = 0,
   PI_GATE    = 1,
   PO_GATE    = 2,
   AIG_GATE   = 3,
   CONST_GATE = 4,

   TOT_GATE
};

class FECGroup
{
public:
   FECGroup() {}
   FECGroup(size_t thBuk, size_t sepBuk, bool sep = false): _thisBuk(thBuk), _sepBuk(sepBuk), _sep(sep){}
   ~FECGroup() {}

   size_t size() const { return _v.size();}
   CirGate* getGate(size_t i) {return _v[i];}
   const CirGate* getGate(size_t i) const { return _v[i]; }
   void setGate(CirGate* g, size_t i) {_v[i] = g;}

   void insert(CirGate* g){ _v.push_back(g);}
   CirGate* getLastPop(){
   	CirGate* la = _v[_v.size() - 1];
   	_v.pop_back();
   	return la;
   }

   size_t getThisBuk()const { return _thisBuk;}
   size_t getSepBuk()const { return _sepBuk;}
   bool   isSep()  const { return _sep;}

   void   setSep(bool s){ _sep = s;}
   void   setThisBuk(size_t b){_thisBuk = b;}
   void   setSepBuk(size_t b){_sepBuk = b;}
   

   
private:
	
   GateList       _v;
   size_t			_thisBuk;
   size_t 			_sepBuk;
   bool 				_sep;
   

};

#endif // CIR_DEF_H
