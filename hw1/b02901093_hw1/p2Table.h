#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

using namespace std;

class Row
{
public:
   const int operator[] (size_t i) const { return _data[i];} // TODO
   int& operator[] (size_t i) { return _data[i]; } // TODO
   void init(int);
   void delete_memory();

private:
   int  *_data;
};

class Table
{
public:
   const Row& operator[] (size_t i) const;
   Row& operator[] (size_t i);

   bool read(const string&);
   int saved_col, saved_row;
   void init(int, int);
   void delete_memory();
   bool valueAssign(const string&);
   void addRow(string, string);
   void print();
   void sum(int);
   void ave(int);
   void max(int);
   void min(int);
   void count(int);


private:
   vector<Row>  _rows;
};

#endif // P2_TABLE_H
