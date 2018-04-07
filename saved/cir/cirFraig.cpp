/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions


/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash(){
	//printDfs();
	GateList opt;
	unsigned l;
	size_t phs = 0;

	#define NEG 0x1
	HashMap<HashKey, HashData>   _strHash;

	_strHash.init(_dfsList.size());
	for(int i = 0; i < _dfsList.size(); i++){
		CirGate* g = _dfsList[i];
		if(g->getFanInSize() == 2){
			HashKey  k(g->getFanInOrgGate(0), g->getFanInOrgGate(1));
			size_t gWra = size_t(g);
			HashData gI(gWra);

			size_t _chk = _strHash.check(k);
			if(_chk != 0){
				CirGate* _simulG = (CirGate*)(_chk & ~size_t(NEG)); 
				l = g->getGateId();
				opt.push_back(g);
				g->setStHDel(true);
				disConnect(g, l);
				setNewConnect(g, _simulG, phs);

				//cout << g->getGateId() << ' ' << _simulG->getGateId() << endl;
				cout << "Strashing: " << _simulG->getGateId() << " merging " << g->getGateId() << "..." << endl;
			}
			else
				_strHash.insert(k,gWra);
		}	
	}
	_strHash.reset();
	listUpdate(opt, 1);
}

void
CirMgr::fraig()
{
	initCircuit();
	SatSolver solver;
   solver.initialize();
   genProofModel(solver);

   bool result;
   unsigned l;
   Var newV;
   GateList fecList;

   for (size_t i = 0, n = _dfsList.size(); i < n; ++i) {
   	CirGate* g = _dfsList[i];
   	size_t origBuk = g->getBuk();
   	size_t blankSlot = g->getSl();

   	if(g->getSl() > 0){ // with two or more elements in Buk
   		newV = solver.newVar();
   		CirGate* gPa = _simHach[origBuk]->getGate(0);// find parent gate in buk
   		bool iFEC = ((gPa->getValue() == g->getValue())? false : true);
			size_t phs = (iFEC? 1 : 0);

   		solver.addXorCNF(newV, gPa->getVar(), false, g->getVar(), iFEC);
   		solver.assumeRelease();  // Clear assumptions
		   solver.assumeProperty(newV, true);  // k = 1
		   result = solver.assumpSolve();
		   // merge
		   if(!result){
		   	l = g->getGateId();
				fecList.push_back(g);
				g->setFec(true);
				disConnect(g, l);
				setNewConnect(g, gPa, phs);
		   }
		   // new FEC group
		   else{
		   	size_t _hashSize = _simHach.size();
				FECGroup* newFEC = new FECGroup(_hashSize, 0);
				_simHach.push_back(newFEC);
				_simHach[_hashSize]->insert(g);
				// change gate's bucket and slot and original FECGroup's seperative bucket
				_simHach[origBuk]->setSepBuk(_hashSize);	
				g->setBukSl(_hashSize, 0);
				// move last to full the blank
				CirGate* _lastGate = _simHach[origBuk]->getLastPop();
				if(g != _lastGate){
					_simHach[origBuk]->setGate(_lastGate, blankSlot);
					_lastGate->setBukSl(origBuk, blankSlot);
				}
		   }
		   cout << (result? "SAT" : "UNSAT") << ' ' << gPa->getGateId() << ' ' << g->getGateId() << endl;
		   //reportResult(solver, result);
   	}
   }
   listUpdate(fecList, 2);

}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::initCircuit(){
	// purpose?
}

void
CirMgr::genProofModel(SatSolver& s){
	for (size_t i = 0, n = _dfsList.size(); i < n; ++i) {
      Var v = s.newVar();
      _dfsList[i]->setVar(v);
   }

   for (size_t i = 0, n = _dfsList.size(); i < n; ++i) {
   	CirGate* g = _dfsList[i];
   	if(g->getGatetype() == AIG_GATE)
   		s.addAigCNF(g->getVar(), g->getFanIn(0)->getVar(), g->faninIsInv(0), g->getFanIn(1)->getVar(), g->faninIsInv(1));
   }
}

void
CirMgr::reportResult(const SatSolver& solver, bool result)
{
   //solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
   /*
   if (result) {
      for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
         cout << solver.getValue(_dfsList[i]->getVar()) << endl;
   }
   */
}
