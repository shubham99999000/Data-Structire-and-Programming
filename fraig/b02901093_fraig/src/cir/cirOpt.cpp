/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
	IdList swL;
	vector < pair <unsigned, unsigned> > pairs;
	//printDfs();
	// fine unused PI
	for(int i = 0; i < _totalList.size(); i++){
		if(_totalList[i]->isSwList() == true && _totalList[i]->getGatetype() == PI_GATE){
			_totalList[i]->setSw(false);
		}
		else if(_totalList[i]->isSwList() == true && _totalList[i]->getGatetype() != PI_GATE && _totalList[i]->getGatetype() != CONST_GATE){
			unsigned n = _totalList[i]->getGateId();
			swL.push_back(n);
		}
	}

	for(int i = 0; i < _undefList.size(); i ++){
		if(_undefList[i]->isSwList()){
			unsigned n =  _undefList[i]->getGateId();
			swL.push_back(n);
		}
	}

	sort(swL.begin(), swL.end());

	
	for(int i = 0; i < swL.size(); i ++){
		CirGate* g = getGate(swL[i]);
		if(g == 0){
			g = getUndef(swL[i]);
		}
		cout << "Sweeping: " << g->getTypeStr() << '(' << swL[i] << ')' << " removed..." << endl;
		disConnect(g, swL[i]);
		
	}

	GateList _tempTtList, _tempuDList;
	for(int i = 0; i < _totalList.size(); i++){
		if(_totalList[i]->isSwList() == false)
			_tempTtList.push_back(_totalList[i]);
	}

	for(int i = 0; i < _undefList.size(); i++){
		if(_undefList[i]->isSwList() == false)
			_tempuDList.push_back(_undefList[i]);
	}
	_totalList = _tempTtList;
	_undefList = _tempuDList;

	// delete
	for(int i = 0; i < swL.size(); i ++){
		CirGate* g = getGate(swL[i]);
		if(g == 0){
			g = getUndef(swL[i]);
		}
		delete g;
	}
	
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	GateList opt;
	for(int i = 0; i < _dfsList.size(); i++){
		faninJudge(_dfsList[i], opt);
	}
	// update gateList & delete unused
	listUpdate(opt, 0);
}

void CirMgr::faninJudge(CirGate* g, GateList& opt){
	unsigned l;
	size_t phs = 0;
	if(g->getGatetype() == PO_GATE) return;

	for(int i = 0; i < g->getFanInSize(); i++){
		if(g->getFanIn(i)->getGatetype() == CONST_GATE){
			l = g->getGateId();
			opt.push_back(g);
			g->setOptDel(true);
			disConnect(g, l);
			if(g->faninIsInv(i) == false){	// CONST 0
				setNewConnect(g, g->getFanIn(i), phs);
				optLog(g, g->getFanIn(i), phs);
				return;
			}		
			else{							// CONST 1
				size_t idx;
				if(i == 0) idx = 1;
				else 	   idx = 0;
				if(g->faninIsInv(idx)) phs = 1;
				setNewConnect(g, g->getFanIn(idx), phs);
				optLog(g, g->getFanIn(idx), phs);
				return;
			}
		}
	}
	if(g->getFanInSize() == 2){
		if(g->getFanIn(0) == g->getFanIn(1)){
			l = g->getGateId();
			opt.push_back(g);
			g->setOptDel(true);
			disConnect(g, l);

			if(g->faninIsInv(0) == g->faninIsInv(1)){
				if(g->faninIsInv(0)) phs = 1;
				setNewConnect(g, g->getFanIn(0), phs);
				optLog(g, g->getFanIn(0), phs);
				return;
			}
			else{
				CirGate* gTemp = getGate(0);
				if(!gTemp) gTemp = new CirGate(0, CONST_GATE, "CONST", 0, true);	
				g->setFanIn(0 ,gTemp, phs);
				setNewConnect(g, g->getFanIn(0), phs);	
				optLog(g, g->getFanIn(0), phs);	
				return;
			}
		}
	}	
}

void CirMgr::disConnect(CirGate* g, unsigned& i){
	for(int j = 0; j < g->getFanInSize(); j++){
		CirGate* gi = g->getFanIn(j);
		for(int k = 0; k < gi->getFanOutSize(); k++){
			if(gi->getFanOut(k)->getGateId() == i){
				gi->eraseFanOut(k);
				//g->eraseFanIn(j);
			}					
		}
	}
}

void CirMgr::setNewConnect(CirGate* g, CirGate* gi, size_t& phase){
	for(int i = 0; i < g->getFanOutSize(); i++){
		CirGate* go = g->getFanOut(i);
		for(int k = 0; k < go->getFanInSize(); k++){
			if(go->getFanIn(k) == g){
				// set go fanin to gi (replace)
				size_t finPhase = phase;
				if(go->faninIsInv(k)){
					if(finPhase) finPhase = 0;
					else finPhase = 1;
				}
				go->setFanIn(k ,gi, finPhase);
				// set gi fanout to go (add)
				gi->setFanOut(go);
				break;
			}
		}
	}
}

void CirMgr::optLog(CirGate* g, CirGate* gi, size_t& phase){
	cout << "Simplifying: " << gi->getGateId() << " merging ";
	if(phase) cout << '!'; 
	cout << g->getGateId() << "..." << endl;
}

void CirMgr::listUpdate(GateList& opt, const int& flag){
	GateList _tempTtList, _tempuDList, _tempDFSList;
	if(flag == 0){
		for(int i = 0; i < _totalList.size(); i++){
		if(!(_totalList[i]->isOptDel()))
			_tempTtList.push_back(_totalList[i]);
		}
		for(int i = 0; i < _undefList.size(); i++){
			if(!(_undefList[i]->isOptDel()))
				_tempuDList.push_back(_undefList[i]);
		}
		for(int i = 0; i < _dfsList.size(); i++){
			if(!(_dfsList[i]->isOptDel()))
				_tempDFSList.push_back(_dfsList[i]);
		}
	}
	else if (flag == 1){
		for(int i = 0; i < _totalList.size(); i++){
		if(!(_totalList[i]->isStHDel()))
			_tempTtList.push_back(_totalList[i]);
		}
		for(int i = 0; i < _undefList.size(); i++){
			if(!(_undefList[i]->isStHDel()))
				_tempuDList.push_back(_undefList[i]);
		}
		for(int i = 0; i < _dfsList.size(); i++){
			if(!(_dfsList[i]->isStHDel()))
				_tempDFSList.push_back(_dfsList[i]);
		}
	}
	else{
		for(int i = 0; i < _totalList.size(); i++){
		if(!(_totalList[i]->getFec()))
			_tempTtList.push_back(_totalList[i]);
		}
		for(int i = 0; i < _undefList.size(); i++){
			if(!(_undefList[i]->getFec()))
				_tempuDList.push_back(_undefList[i]);
		}
		for(int i = 0; i < _dfsList.size(); i++){
			if(!(_dfsList[i]->getFec()))
				_tempDFSList.push_back(_dfsList[i]);
		}

	}
	
	_totalList = _tempTtList;
	_undefList = _tempuDList;
	_dfsList = _tempDFSList;

	for(int i = 0; i < opt.size(); i++)
		delete opt[i];

}






/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
