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
	SatSolver solver;
   solver.initialize();
   CirGate* g0 = genProofModel(solver);  
   GateList fecList;
   fraigLoop(g0, fecList, solver);
   listUpdate(fecList, 2);

}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::fraigLoop(CirGate* g0, GateList& fecList, SatSolver& solver){
	GateList _FECgate;
	bool result;
   unsigned l;
   Var newV;
   int it = 0;
   while(true){
   	if(it==0) _FECgate = _dfsList;
   	if(_FECgate.size() == 0){
   		resetSep();
   		return;
   	}
	   for (size_t i = 0, n = _FECgate.size(); i < n; ++i) {
	   	CirGate* g = _FECgate[i];
	   	size_t origBuk = g->getBuk();
	   	size_t blankSlot = g->getSl();

	   	if(blankSlot > 0){ // with two or more elements in Buk
	   		newV = solver.newVar();
	   		CirGate* gPa = _simHach[origBuk]->getGate(0);// find parent gate in buk
	   		bool iFEC = ((gPa->getValue() == g->getValue())? false : true);
				size_t phs = (iFEC? 1 : 0);

	   		solver.addXorCNF(newV, gPa->getVar(), false, g->getVar(), iFEC);
	   		solver.assumeRelease();  // Clear assumptions
			   solver.assumeProperty(newV, true);  // k = 1
			   solver.assumeProperty(g0->getVar(),false);
			   result = solver.assumpSolve();
			   // merge
			   if(!result){
			   	l = g->getGateId();
					fecList.push_back(g);
					g->setFec(true);
					disConnect(g, l);
					setNewConnect(g, gPa, phs);
			   }
			   // new FEC group (g, true)
				SATSetHash(g,result);
			   //cout << (result? "SAT" : "UNSAT") << ' ' << gPa->getGateId() << ' ' << g->getGateId() << endl;
				if(!result){
					cout << "Fraig: " << gPa->getGateId() << " merging ";
					if(iFEC) cout << '!';
					cout << g->getGateId() << "..." << endl;

				} 
	   	}
	   }
		resetSep();
		_FECgate.clear();
		if(_simHach.size() > 0){
	      for(int i = 0, n = _simHach.size(); i < n; i++){
	         int m = _simHach[i]->size();
	         if(m >= 2){
	            for(int j = 0; j < m; j++){
	               CirGate* g = _simHach[i]->getGate(j);
	               _FECgate.push_back(g);
	            }
	         }
	      }
	   }
		++it;
	}
}

void
CirMgr::SATSetHash(CirGate* g, bool result){
	size_t origBuk = g->getBuk();
	size_t blankSlot = g->getSl();
	if(result){
		if(!_simHach[origBuk]->isSep()){		
			// set Seperate
			_simHach[origBuk]->setSep(true);
			// new FECGroup and insert first gate 	
			size_t _hashSize = _simHach.size();
			FECGroup* newFEC = new FECGroup(_hashSize, 0);
			_simHach.push_back(newFEC);
			_simHach[_hashSize]->insert(g);

			// change gate's bucket and slot and original FECGroup's seperative bucket
			_simHach[origBuk]->setSepBuk(_hashSize);	
			g->setBukSl(_hashSize, 0);						
		}
		// isSep & move gate to new buk;
		else{
			size_t sepBuk = _simHach[origBuk]->getSepBuk();
			size_t sl = _simHach[sepBuk]->size();
			_simHach[sepBuk]->insert(g);
			g->setBukSl(sepBuk, sl);
		}
	}
	// move last to full the blank
	CirGate* _lastGate = _simHach[origBuk]->getLastPop();
	if(g != _lastGate){
		_simHach[origBuk]->setGate(_lastGate, blankSlot);
		_lastGate->setBukSl(origBuk, blankSlot);
	}

}


CirGate*
CirMgr::genProofModel(SatSolver& s){
	for (size_t i = 0, n = _dfsList.size(); i < n; ++i) {
      Var v = s.newVar();
      _dfsList[i]->setVar(v);
   }
   // deal with const0
   CirGate* g0 = getGate(0);
   bool constAdd = true;
   for(int i = 0, n = _dfsList.size(); i < n; i++){
		if(_dfsList[i]->getGatetype() == CONST_GATE) constAdd = false;
	}
	if(constAdd){
		Var v = s.newVar();
		g0->setVar(v);
	}
	// deal with const0 

   for (size_t i = 0, n = _dfsList.size(); i < n; ++i) {
   	CirGate* g = _dfsList[i];
   	if(g->getGatetype() == AIG_GATE)
   		s.addAigCNF(g->getVar(), g->getFanIn(0)->getVar(), g->faninIsInv(0), g->getFanIn(1)->getVar(), g->faninIsInv(1));
   }
   return g0;
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
