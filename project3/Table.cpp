#include "Table.h"

Table::Table() {
	cols = 0;
}

Table::Table(int N,
	colIt cbegin, colIt cend,
	typeIt tbegin, typeIt tend,
	string _name, bool _quiet) {
	quiet = _quiet;
	name = _name;
	cols = N;
	columnNames.reserve(N);
	for (int i = 0; cbegin != cend; cbegin++, i++) {
		columnNames[*cbegin] = i;
	}
	columnTypes = vector<EntryType>(tbegin, tend);
}


void Table::insertRows(int num) {
	string trash; 
	auto rows0 = rows;
	data.resize(rows + num);
	for (int j = 0; j < num; ++j) {
		data[rows0 + j].reserve(cols);
		for (int i = 0; i < int(columnNames.size()); ++i) {
			switch (columnTypes[i]) {
			case EntryType::Bool: {
				bool tempb;
				cin >> boolalpha >> tempb;
				data[rows0 + j].emplace_back(TableEntry(tempb));
				break;
			}
			case EntryType::String: {
				string temps;
				cin >> temps;
				data[rows0 + j].emplace_back(TableEntry(temps));
				break;
			}
			case EntryType::Int: {
				int tempi;
				cin >> tempi;
				data[rows0 + j].emplace_back(TableEntry(tempi));
				break;
			}
			case EntryType::Double: {
				double tempd;
				cin >> tempd;
				data[rows0 + j].emplace_back(TableEntry(tempd));
				break;
			}
			}
		}
		++rows;
	}
	updateIndicies();
}

void Table::updateIndicies() {
	if (indexType == 'h') {
		hash.clear();
		bst.clear();
		makeHash();
	}
	else if (indexType == 'b') {
		bst.clear();
		hash.clear();
		makeBST();
	}
}

void Table::generate() {
	string temp;
	cin >> indexType;
	cin >> temp;
	cin >> temp;
	cin >> temp;
	cin >> temp; // should be at colName
	bst.clear();
	hash.clear();
	if (columnNames.find(temp) == columnNames.end()) {
		printError(name, temp);
		return;
	}

	indexCol = columnNames[temp];
	if (indexType == 'h')
		makeHash();
	else
		makeBST();

	cout << "Created ";
	indexType == 'h' ? cout << "hash" : cout << "bst";
	cout << " index for table " << name << " on column " << temp << "\n";
}

void Table::makeHash() {
	for (int i = 0; i < rows; ++i)
		hash[data[i][indexCol]].push_back(i);
}

void Table::makeBST() {
	for (int i = 0; i < rows; ++i) {
		bst[data[i][indexCol]].push_back(i);
	}
}

void Table::printAll(colIt cbegin, colIt cend) {
	string trash;
	for (auto it = cbegin; it != cend; it++) {
		if (columnNames.find(*it) == columnNames.end()) {
			printError(name, *it);
			std::getline(cin, trash); 
			return;
		}
	}
	if (!quiet) {
		for (auto it = cbegin; it != cend; it++) {
			cout << *it << " ";
		}
		cout << "\n";
		for (int i = 0; i < rows; ++i) {
			for (auto it = cbegin; it != cend; it++) {
				cout << data[i][columnNames[*it]] << " ";
			}
			cout << "\n";
		}
	}
	cout << "Printed " << rows << " matching rows from " << name << "\n";
}

void Table::del() {
	string trash; 
	string colname;
	char op;
	cin >> colname;
	if (columnNames.find(colname) == columnNames.end()) {
		printError(name, colname);
		return;
	}
	cin >> op;
	switch (columnTypes[columnNames[colname]]) {
		case EntryType::Bool: {
			bool temp;
			cin >> boolalpha >> temp;
			removeComp<bool> rc(columnNames[colname], op, temp);
			delHelper(rc);
			break;
		}
		case EntryType::String: {
			string temp;
			cin >> temp;
			removeComp<string> rc(columnNames[colname], op, temp);
			delHelper(rc);
			break;
		}
		case EntryType::Int: {
			int temp;
			cin >> temp;
			removeComp<int> rc(columnNames[colname], op, temp);
			delHelper(rc);
			break;
		}
		case EntryType::Double: {
			double temp;
			cin >> temp;
			removeComp<double> rc(columnNames[colname], op, temp);
			delHelper(rc);
			break;
		}
	}
	updateIndicies();
}

template<typename T>
void Table::delHelper(removeComp<T> & comp) {
	vector<vector<TableEntry>>::iterator it = std::remove_if(data.begin(), data.end(), comp);
	auto deletions = data.size() - std::distance(data.begin(), it);
	for (int i = 0; i < int(deletions); ++i) {
		data.pop_back();
		rows--;
	}
	cout << "Deleted " << deletions << " rows from " << name << "\n";
}

void Table::printWhere(colIt cbegin, colIt cend) {
	string trash; 
	string colname;
	char op;
	cin >> colname;
	if (columnNames.find(colname) == columnNames.end()) {
		printError(name, colname);
		std::getline(cin, colname); 
		return;
	}
	for (auto it = cbegin; it != cend; ++it) {
		if (columnNames.find(*it) == columnNames.end()) {
			printError(name, *it);
			std::getline(cin, colname); 
			return;
		}
	}
	cin >> op;
	switch (columnTypes[columnNames[colname]]) {
		case EntryType::Bool: {
			bool b;
			cin >> boolalpha >> b;
			printWhereHelper(cbegin, cend, op, TableEntry(b), columnNames[colname]);
			break;
		}
		case EntryType::String: {
			string s;
			cin >> s;
			printWhereHelper(cbegin, cend, op, TableEntry(s), columnNames[colname]);
			break;
		}
		case EntryType::Int: {
			int i;
			cin >> i;
			printWhereHelper(cbegin, cend, op, TableEntry(i), columnNames[colname]);
			break;
		}
		case EntryType::Double: {
			double d;
			cin >> d;
			printWhereHelper(cbegin, cend, op, TableEntry(d), columnNames[colname]);
			break;
		}
	}
}

void Table::printError(int n, string& name) {
	cout << "Error: ";
	switch (n) {
	case 1:
		cout << "Cannot create already existing table " << name;
		break;
	case 2:
		cout << name << " does not name a table in the database";
		break;
	}
	cout << "\n";
}

void Table::join(Table& t1, Table& t2) {
	string col1, col2, trash;
	cin >> trash;
	cin >> col1;
	cin >> trash;
	cin >> col2;

	if (t1.columnNames.find(col1) == t1.columnNames.end()) {
		printError(t1.name, col1);
		std::getline(cin, trash);
		return;
	}
	if (t2.columnNames.find(col2) == t2.columnNames.end()) {
		printError(t2.name, col2);
		std::getline(cin, trash);
		return;
	}
	cin >> trash;
	cin >> trash;

	int n;
	cin >> n;
	short int tn;
	string s;
	vector<short int> firstTable(n);
	vector<int> colsToPrint(n);
	vector<string> printed(n);
	for (int i = 0; i < n; ++i) {
		cin >> s;
		cin >> tn;
		if (tn == 1) {
			if (t1.columnNames.find(s) == t1.columnNames.end()) {
				printError(t1.name, s);
				std::getline(cin, trash);
				return;
			}
			colsToPrint[i] = t1.columnNames[s];
		}
		else {
			if (t2.columnNames.find(s) == t2.columnNames.end()) {
				printError(t2.name, s);
				std::getline(cin, trash);
				return;
			}
			colsToPrint[i] = t2.columnNames[s];
		}
		firstTable[i] = tn;
		printed[i] = s;
	}

	int c1 = t1.columnNames[col1],
		c2 = t2.columnNames[col2];

	unordered_map<TableEntry, vector<int>> temphash;
	if (!(t2.indexType == 'h' && t2.indexCol == c2))
		for (int i = 0; i < t2.rows; i++)
			temphash[t2.data[i][c2]].push_back(i);
	else
		temphash = t2.hash;

	if (!t1.quiet) {
		for (int i = 0; i < int(colsToPrint.size()); ++i)
			cout << printed[i] << " ";
		cout << "\n";
	}
	int printCount = 0;
	for (int row = 0; row < t1.rows; ++row) {
		if (temphash.find(t1.data[row][c1]) != temphash.end()) {
			for (auto row2 : temphash[t1.data[row][c1]]) {
				if (!t1.quiet) {
					for (int col = 0; col < int(colsToPrint.size()); ++col) {
						firstTable[col] == 1 ? cout << t1.data[row][colsToPrint[col]] :
							cout << t2.data[row2][colsToPrint[col]];
						cout << " ";
					}
					cout << "\n";
				}
				printCount++;
			}
		}
	}
	cout << "Printed " << printCount << " rows from joining " << t1.name << " to " << t2.name << "\n";
	temphash.clear();
}

void Table::printError(string& name, string& s) {
	cout << "Error: " << s <<
		" does not name a column in " <<
		name << "\n";
}

void Table::printWhereHelper(colIt cbegin, colIt cend, char op, TableEntry value, int colVal) {
	if (!quiet) {
		for (auto it = cbegin; it != cend; it++) {
			cout << *it << " ";
		}
		cout << "\n";
	}
	int rowCount = 0;

	if (colVal != indexCol || indexType != 'b') {
		if (op == '=') {
			if (indexType == 'h' && indexCol == colVal) {
				for (int rowval : hash[value]) {
					if (!quiet) {
						for (auto it = cbegin; it != cend; it++) {
							cout << data[rowval][columnNames[*it]] << " ";
						}
						cout << "\n";
					}
					++rowCount;
				}
			}
			else {
				for (int rowval = 0; rowval < rows; ++rowval) {
					if (data[rowval][colVal] == value) {
						if (!quiet) {
							for (auto it = cbegin; it != cend; it++) {
								cout << data[rowval][columnNames[*it]] << " ";
							}
							cout << "\n";
						}
						++rowCount;
					}
				}
			}
		}
		else if (op == '<') {
			for (int rowval = 0; rowval < rows; ++rowval) {
				if (data[rowval][colVal] < value) {
					if (!quiet) {
						for (auto it = cbegin; it != cend; it++) {
							cout << data[rowval][columnNames[*it]] << " ";
						}
						cout << "\n";
					}
					++rowCount;
				}
			}
		}
		else {
			for (int rowval = 0; rowval < rows; ++rowval) {
				if (data[rowval][colVal] > value) {
					if (!quiet) {
						for (auto it = cbegin; it != cend; it++) {
							cout << data[rowval][columnNames[*it]] << " ";
						}
						cout << "\n";
					}
					++rowCount;
				}
			}
		}
	}
	// else using binary tree on correct col
	else {
		if (op == '=') {
			vector<int> goodRows = bst[value];
			for (int rowval : goodRows) {
				if (!quiet) {
					for (auto it = cbegin; it != cend; it++) {
						cout << data[rowval][columnNames[*it]] << " ";
					}
					cout << "\n";
				}
				++rowCount;
			}
		}
		else if (op == '<') {
			auto bstend = bst.lower_bound(value);
			for (auto bstit = bst.begin(); bstit != bstend; bstit++) {
				for (auto rowval : (*bstit).second) {
					if (!quiet) {
						for (auto it = cbegin; it != cend; it++) {
							cout << data[rowval][columnNames[*it]] << " ";
						}
						cout << "\n";
					}
					++rowCount;
				}
			}
		}
		else {
			auto bstit = bst.upper_bound(value);
			for (auto bstend = bst.end(); bstit != bstend; bstit++) {
				for (int rowval : (*bstit).second) {
					if (!quiet) {
						for (auto it = cbegin; it != cend; it++) {
							cout << data[rowval][columnNames[*it]] << " ";
						}
						cout << "\n";
					}
					++rowCount;
				}
			}
		}
	}
	cout << "Printed " << rowCount << " matching rows from " << name << "\n";
}
