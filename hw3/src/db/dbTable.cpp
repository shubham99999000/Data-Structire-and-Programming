/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(int i = 0; i < r.size(); i++){
      if(r[i] == INT_MAX)
         os << '.';
      else
         os << r[i];
      os << ' ';
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for(int i = 0; i < t.nRows(); i++){
      for(int j = 0; j < t.nCols(); j++){
         if(t._table[i][j] == INT_MAX)
            os << setw(6) << right << '.';
         else
            os << setw(6) << right << t._table[i][j];           // not sure t_table
      }
      os << endl;
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.   ifs >> dbtbl
   string str = "";
   int num;
   bool endRowFlag = false;
   DBRow r;

   char ch;
   while(true){
      ch = myGetChar(ifs);
      //cout << "read test" << ch;
      if(ch >= 48 && ch <= 57){
         endRowFlag = false;
         str.push_back(ch);
      }
      else if(ch == ','){
         endRowFlag = false;
         if(str.empty())
            num = INT_MAX;
         else{
            if(myStr2Int(str,num) == false)
               break;
         }
         //cout << num << endl;
         r.addData(num);
         str.clear();
      }
      else if(ch == '\r' && endRowFlag == false){
         if(str.empty())
            num = INT_MAX;
         else{
            if(myStr2Int(str,num) == false)
               break;
         }
         //cout << num << endl;
         r.addData(num);
         t.addRow(r);
         str.clear();
         r.reset();
         endRowFlag = true;
      }
      else if(ch == '\n'){
         break;
      }
      else break;

   }
   //cout << t._table[3][1] << " " << t._table[0][24];
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   _data.erase(_data.begin()+c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for (int i=0;i<_sortOrder.size();i++) {
      if ( r1[_sortOrder[i]] != r2[_sortOrder[i]] ) 
         return (r1[_sortOrder[i]] < r2[_sortOrder[i]]);
      else 
         break;
   }
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   vector<DBRow> tmp; 
   tmp.swap(_table);
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   for(int i = 0; i < _table.size(); i++)
      _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin()+c);  
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   float max = INT_MAX;
   for(int i = 0; i < nRows(); i++){
      if(_table[i][c] != max){
         if((_table[i][c] > max && _table[i][c] != INT_MAX)|| max == INT_MAX)
            max = _table[i][c];
      }
   }
   if(max == INT_MAX) return NAN;
   else return max;  
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   float min = INT_MAX;
   for(int i = 0; i < nRows(); i++){
      if(_table[i][c] < min)
         min = _table[i][c];
   }
   if(min == INT_MAX) return NAN;
   else return min;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   float sum = 0;
   bool nanFlag = true;

   for(int i = 0; i < nRows(); i++){
      if(_table[i][c] != INT_MAX){
         sum += _table[i][c];
         nanFlag = false;
      }
   }
   if (nanFlag) return NAN;
   else return sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   int distCount = 1;
   bool sameNum = false;
   
   if(_table[0][c] == INT_MAX)
      distCount = 0;          // don't count first one

   for(int i = 0; i < nRows(); i++){
      for(int j = 0; j < i; j++){
         if((_table[j][c] == _table[i][c]) && (_table[i][c] != INT_MAX) && (_table[j][c] != INT_MAX))
            sameNum = true;
         if(j == (i-1) && sameNum == false && (_table[i][c] != INT_MAX)) 
            distCount++;
      }
      sameNum = false;
   }
         
   return distCount;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   float sum = 0;
   int count = 0;
   bool nanFlag = true;

   for(int i = 0; i < nRows(); i++){
      if(_table[i][c] != INT_MAX){
         sum += _table[i][c];
         nanFlag = false;
         count ++;
      }
   }

   if (nanFlag) return NAN;
   else return sum/count;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   std::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'                                                  not sure
   size_t row = nRows();
   for(size_t i = 0; i < row; i++){
      if(_table[i][c] == INT_MAX)
         cout << ". ";
      else   
         cout << _table[i][c] << ' ';
   }

}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

