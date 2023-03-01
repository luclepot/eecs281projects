#include <iostream> 
#include <vector>
#include <unordered_map>
#include <map>
#include "TableEntry.h"
#include <algorithm>

using std::vector;

using std::string; 
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::unordered_map;

using colIt = vector<string>::iterator;
using typeIt = vector<EntryType>::iterator;

using std::boolalpha; 

class Table {
	public: 

		template<typename T> 
		class removeComp {
		public: 
			removeComp(int _col, char _op, T _value) : col(_col), op(_op), value(_value) {}
			int col; 
			char op;
			T value; 
			bool operator()(const vector<TableEntry> &a) {
				if (op == '<')
					return a[col] < TableEntry(value);
				else if (op == '>')
					return a[col] > TableEntry(value);
				else
					return a[col] == TableEntry(value);
			}
		};

		int cols, rows = 0;

		Table(); 

		Table(int N, 
			colIt cbegin, colIt cend, 
			typeIt tbegin, typeIt tend, 
			string _name, bool quiet);

		void insertRows(int num); 

		void printAll(colIt cbegin, colIt cend); 

		void printWhere(colIt cbegin, colIt cend);

		void generate(); 

		void updateIndicies(); 

		void del(); 

		static void printError(int n, string& name);
		static void printError(string& name, string& s);

		static void join(Table& t1, Table& t2); 

	private:
		string name; 
		vector<vector<TableEntry>> data;
		unordered_map<string, int> columnNames;
		vector<EntryType> columnTypes;

		unordered_map<TableEntry, vector<int>> hash; 
		map<TableEntry, vector<int>> bst; 
		int indexCol = -1; 
		char indexType = 'n'; 

		bool quiet = false; 

		unordered_map<string, int> placement;

		void printWhereHelper(colIt cbegin, colIt cend, char op, TableEntry value, int colVal); 

		template<typename T>
		void delHelper(removeComp<T> & comp);

		void makeHash(); 

		void makeBST(); 

};

