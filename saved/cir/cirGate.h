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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.
class CirGateV;
class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGateV
{
public:
  #define NEG 0x1
  CirGateV(CirGate* g, size_t phase) : _gateV(size_t(g) + phase){}
  CirGate* gate()const{return (CirGate*)(_gateV & ~size_t(NEG)); }
  bool isInv() const{return (NEG & _gateV); } 
  void setGate(CirGate* g, size_t phase){ _gateV = size_t(g) + phase; }
  size_t getOrgGate() const {return _gateV;}

private:
  size_t _gateV;
};

class CirGate
{
public:
   CirGate() {}
   CirGate(unsigned g, GateType gt, string l, unsigned ln, bool s, bool strHh = false, bool value = false, bool preValue = false, unsigned valSet = 0, bool fec = false): _Ref(0), _gateId(g), _gateType(gt), _lineNum(l), _lineNo(ln), _optDel(s), _strHhDel(strHh), _value(value), _preValue(preValue), _valueSet(valSet) , _fec(fec){}
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
   size_t getFanInOrgGate(int i) const {return _fanInList[i].getOrgGate();}
   CirGate* getFanOut(int i) const {return _fanOutList[i];}
   bool faninIsInv(int i) const {return _fanInList[i].isInv();}
   void eraseFanOut(int&);
   void eraseFanIn(int&);
   void setFanIn(int i, CirGate* g, size_t& phase) {_fanInList[i].setGate(g, phase);}
   void setFanOut(CirGate* go){_fanOutList.push_back(go);}

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
   // new in Final
   virtual bool isAig() const { return false; }
   //SW, OPT, Strash, SIM
   void setSw(bool s) {_sw = s;}
   void setOptDel(bool s) {_optDel = s;}
   void setStHDel(bool sth) {_strHhDel = sth;}
   void setValue(bool v) { _value = v;}
   void setValToPre() { _preValue = _value;}
   void setBukSl(unsigned b, unsigned s) {_bukSl.first = b; _bukSl.second = s;}
   void setFec(bool f) {_fec = f;} 
   void setVar(const Var& v) {_var = v;}
   void setValSet(bool b);

   bool isSwList() const{ return _sw;} 
   bool isOptDel() const { return _optDel;}
   bool isStHDel() const { return _strHhDel;}
   bool getValue() const { return _value;}
   bool getPreVal() const { return _preValue;}
   bool getFec() const { return _fec;}
   unsigned getBuk() const {return _bukSl.first;}
   unsigned getSl() const {return _bukSl.second;}
   Var getVar() const { return _var; }
   

   
private:
  static unsigned  _globalRef;
  vector<CirGateV> _fanInList;
  string           _symbol;
  GateList         _fanOutList;
  unsigned         _Ref;
  unsigned         _gateId;                      // what it writes in file (positive int)
  GateType         _gateType;
  string           _lineNum; 
  unsigned         _lineNo;  
  bool             _sw;
  bool             _optDel;
  bool             _strHhDel;
  bool             _value;
  bool             _preValue;
  unsigned         _valueSet;
  bool             _fec;
  Var              _var;

  pair<unsigned, unsigned>   _bukSl;


protected:
};

#endif // CIR_GATE_H
