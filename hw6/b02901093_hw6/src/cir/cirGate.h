/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGateV;
class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGateV
{
public:
  #define NEG 0x1
  CirGateV(CirGate* g, size_t phase) : _gateV(size_t(g) + phase){}
  CirGate* gate()const{return (CirGate*)(_gateV & ~size_t(NEG)); }
  bool isInv() const{return (NEG & _gateV); } 

private:
  size_t _gateV;
};

class CirGate
{
public:
   CirGate() {}
   CirGate(unsigned g, GateType gt, string l, unsigned ln): _Ref(0), _gateId(g), _gateType(gt), _lineNum(l), _lineNo(ln) {}
   virtual ~CirGate() {}

   // Basic access methods  
   unsigned getGateId()const{return _gateId;}
   unsigned getLineNo() const { return _lineNo; }
   string getLineNum()const {return _lineNum;}
   string getTypeStr() const;
   GateType getGatetype()const{return _gateType;}   
   string getSymbol()const{return _symbol;}
   unsigned getFanInSize() const {return _fanInList.size();}
   unsigned getFanOutSize() const {return _fanOutList.size();}
   CirGate* getFanIn(int i) const {return _fanInList[i].gate();}
   CirGate* getFanOut(int i) const {return _fanOutList[i];}
   
   // Basic Setting method
   void setSymbol(string& sym){ _symbol = sym;}
   void addFanIn(CirGate* g, size_t phase){  CirGateV a(g, phase); _fanInList.push_back(a);}
   void addFanOut(CirGate* g){_fanOutList.push_back(g);}

   // Printing functions
   void printGate() const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   void faninPrint(vector<const CirGate*>& _track, const CirGate* g, int level, bool inV, int breath)const;
   void fanoutPrint(vector<const CirGate*>& _track, const CirGate* g, int level, const CirGate* fanIn, int depth) const;

   // ref & globalRef
   bool eqaulGloRef() {return (_Ref == _globalRef);}
   void setToGlo()      {_Ref = _globalRef;}
   static void setGlobalRef(){_globalRef = 1;}

   bool hasSymbol()const {return (_symbol.size() != 0);}
   void dfs(GateList& d);

private:
  static unsigned  _globalRef;
  vector<CirGateV> _fanInList;
  string           _symbol;
  GateList         _fanOutList;
  unsigned         _Ref;
  GateType         _gateType;
  unsigned         _lineNo;
  string           _lineNum; 
  unsigned         _gateId;                      // what it writes in file (positive int)
  
  
  
  // no lineString and #line


protected:

};

#endif // CIR_GATE_H
