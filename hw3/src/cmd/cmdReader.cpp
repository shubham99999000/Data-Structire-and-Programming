/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Read the command from the standard input or dofile ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cassert>
#include <cstring>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class CmdParser
//----------------------------------------------------------------------
bool
CmdParser::readCmd(istream& istr)
{
   resetBufAndPrintPrompt();

   // THIS IS EQUIVALENT TO "readCmdInt()" in HW#2
   bool newCmd = false;
   while (!newCmd) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) {
         if (_dofile != 0)
            closeDofile();
         break;
      }
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : {/* TODO */ 
            if(_readBufPtr == _readBuf)
                mybeep();
            else{
                moveBufPtr(_readBufPtr - 1);
                deleteChar();
            }
            break;
         }                             
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : newCmd = addHistory();
                               cout << char(NEWLINE_KEY);
                               if (!newCmd) resetBufAndPrintPrompt();
                               break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1); break;                         
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : // THIS IS DIFFERENT FROM HW#2
                               { char tmp = *_readBufPtr; *_readBufPtr = 0;
                               string str = _readBuf; *_readBufPtr = tmp;
                               listCmd(str);
                               break; }
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
   return newCmd;
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // THIS IS YOUR HW#2 TODO
   if(ptr < _readBuf || ptr > _readBufEnd){ 
        mybeep();
        return false;
    }
    else if(_readBufPtr > ptr){  // arrow left || home
        for(int i = 0; i < (_readBufPtr - ptr); i++)
            cout << '\b';
    }
        
    else { // arrow right || end
        for(int i = 0; i < (ptr - _readBufPtr); i++)
            cout << *(_readBufPtr +i);
    }
        
    _readBufPtr = ptr;
    //cout << "(current point to : "<< _readBufPtr << ")";
    // why "ptr" show series of string?
    return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // THIS IS YOUR HW#2 TODO
   if(_readBufEnd == _readBufPtr){
        mybeep();
        return false; 
    }
    else{
        for(int i = 0; i < (_readBufEnd - _readBufPtr); i++){
            *(_readBufPtr + i) = *(_readBufPtr + 1 + i);
        }
        *(_readBufEnd - 1) = ' ';

        for(int i = 0; i < (_readBufEnd - _readBufPtr - 1); i++)
            cout << *(_readBufPtr + i);
        cout << ' ';
            
        for(int i = 0; i < (_readBufEnd - _readBufPtr); i++)
            cout << '\b';

        _readBufEnd --;
        *_readBufEnd = 0;
        return true;
    }
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // THIS IS YOUR HW#2 TODO
   assert(repeat >= 1);

    for(int i = 1; i <= (_readBufEnd - _readBufPtr); i++)                        // assign original value to new pointing memory 
        *(_readBufEnd - i + repeat) = *(_readBufEnd - i);

    for(int i = 0; i < repeat; i++)                                              // fill the value in blank memory
        *(_readBufPtr + i) = ch;

    
    for(int i = 0; i < (_readBufEnd - _readBufPtr + repeat); i++)                        // cout
        cout << *(_readBufPtr + i);

    
    if(_readBufPtr != _readBufEnd){                                              // cursor back
        for(int i = 0; i < (_readBufEnd - _readBufPtr); i++)
            cout << '\b';
    }
    _readBufEnd += repeat;
    _readBufPtr += repeat;
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // THIS IS YOUR HW#2 
  char*  tempEnd = _readBufEnd;
    moveBufPtr(_readBufEnd);
    for(int i = 0; i < (tempEnd - _readBuf); i++){
        moveBufPtr(_readBufEnd - 1);
        deleteChar();       
    }
    _readBufPtr = _readBufEnd = _readBuf;
    *_readBufPtr = 0;
    *_readBufEnd = 0;
}


// Reprint the current command to a newline
// cursor should be restored to the original location
void
CmdParser::reprintCmd()
{
   // THIS IS NEW IN HW#3
   cout << endl;
   char *tmp = _readBufPtr;
   _readBufPtr = _readBufEnd;
   printPrompt(); cout << _readBuf;
   moveBufPtr(tmp);
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // THIS IS YOUR HW#2 TODO
  // moving up
    if( index < _historyIdx){
        if(_historyIdx == 0){
            mybeep();
            index = 0;
        }
        else if(_historyIdx == _history.size()){
            _tempCmdStored = true;
            addHistory();           // not sure about parameter
            _historyIdx = index;
            retrieveHistory();
            
        }
        else{
            _historyIdx = index;
            retrieveHistory();
        }
    }
    // moving down
    else {
        if(_historyIdx == _history.size()){
            mybeep();
            index = _history.size() - 1;
        }
        else if(_historyIdx == _history.size() - 2){
            _historyIdx = index;
            retrieveHistory();
            _history.pop_back();
        }
        else{
            _historyIdx = index;
            retrieveHistory();
        }   
    }
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
// 6. Reset _readBufPtr and _readBufEnd to _readBuf
// 7. Make sure *_readBufEnd = 0 ==> _readBuf becomes null string
//
bool
CmdParser::addHistory()
{
   // THIS IS SIMILAR TO addHistory in YOUR HW#2 TODO
   // HOWEVER, return true if a new command is entered
   // return false otherwise
    bool newCmd = false;

    string tempHist = "";
    // parse all the string on cursor
    for(int i = 0; i < (_readBufEnd - _readBuf); i++)
        tempHist.push_back(*(_readBuf+i));  

    // without any char or null
    if(tempHist == ""){                                               
        if(_tempCmdStored == true){                                    //  arrow up
            _history.push_back("");
            return newCmd;
        }                                     
        else{return true;}                                                         //  NewLine key
    }
    // with char
    else{
        // remove space at head & tail
        if(_tempCmdStored == true){
            _history.push_back(tempHist);
            return newCmd;
        }    
        else{
            while(tempHist.at(0) == ' ' && tempHist.size() != 1)
                tempHist.erase(0,1);
            while(tempHist.at(tempHist.size()-1) == ' ' && tempHist.size() != 1)
                tempHist.pop_back(); 
            if(_history.size() > 0 && (_history[_history.size() - 1] == ""))
                _history.pop_back();                                                       
            _history.push_back(tempHist); 
            return true; 
        }
    }
                   
    _tempCmdStored = false;
    if(_history.size() >= 1)
        _historyIdx = _history.size();
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
