/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "sat.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() {} 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;
   CirGate* getUndef(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();
   void disConnect(CirGate*, unsigned&);
   void setNewConnect(CirGate*, CirGate*, size_t&);
   void optLog(CirGate*, CirGate*, size_t&);
   void faninJudge(CirGate*, GateList&);

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   void piValSet(CirGate*, char&);
   void defValue(CirGate*, int&);
   void setHash(CirGate*, int&, int&);
   void resetPi();
   void resetSep();
   bool addNonDfsConst();
   void setRepSim(bool b){_repSim = b;}
   bool getRepSim(){return _repSim;}

   // Member functions about fraig
   void strash();
   void listUpdate(GateList&, const int&);
   unsigned printFEC(const unsigned&, const CirGate* g) const;
   void fraig();
   void fraigLoop(CirGate*, GateList&, SatSolver&);
   void SATSetHash(CirGate*, bool);
   CirGate* genProofModel(SatSolver&);
   void reportResult(const SatSolver&, bool);
   
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void printDfs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;


private:
   
   GateList  _piList;
   GateList  _poList;
   GateList  _undefList;
   GateList  _dfsList;
   GateList  _totalList;
   int       synt[5]; // first line
   bool      _repSim;

   // new in Final 
   ofstream             *_simLog;
   vector<FECGroup *>     _simHach;

};

#endif // CIR_MGR_H
