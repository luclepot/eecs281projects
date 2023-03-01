#include "Letterman.h"

// public functions, to be used in main

void Letterman::readData(std::string * strp, bool length) {
	std::string s = "";

	// we want to use getline for all the file readings, 
	// to ensure consistency. 

	std::getline(std::cin, s);
	dictType = s[0];

	std::getline(std::cin, s);
	N = std::stoi(s);

	if(dictType == 'S') d.reserve(N);
	else d.reserve(int(1.25*N));

	while (std::getline(std::cin, s)) {
		// break on empty line
		if (s == "") break;

		// ignore comments
		if (s.substr(0, 2) != "//") {
			if (s.find("\r") != std::string::npos)
				s.erase(s.find("\r"), 1);
			if (dictType == 'S') simpleRead(s, strp, length);
			if (dictType == 'C') complexRead(s, strp, length);
		}
	}
}

bool Letterman::contStartEnd(const std::string &s, const std::string &e) {
	int sc = -1, ec = -1;
	for (int i = 0; i < int(d.size()); ++i) {
		if (d[i] == s) sc = i;
		if (d[i] == e) ec = i;
		if (sc >= 0 && ec >= 0) return true;
	}
	return false;
}

int Letterman::getOptions(int argc, char** argv) {
	int optionIndex = 0, option = 0;

	struct option longOpts[] = { 
		{ "stack", no_argument, nullptr, 's' },
		{ "queue", no_argument, nullptr, 'q' },
		{ "change", no_argument, nullptr, 'c'},
		{ "swap", no_argument, nullptr, 'p'},
		{ "length", no_argument, nullptr, 'l' },
		{ "output", required_argument, nullptr, 'o' },
		{ "begin", required_argument, nullptr, 'b' },
		{ "end", required_argument, nullptr, 'e' },
		{ "help", no_argument, nullptr, 'h' },
		{ nullptr, 0, nullptr, '\0' }
	};

	while ((option = getopt_long(argc, argv,
		"sqcplo:b:e:h", longOpts, &optionIndex)) != -1) {
		
		switch (option) {
			case 's': {
				useStack = true;
				break; 
			}
			case 'q': {
				useQueue = true;
				break; 
			}
			case 'c': {
				change = true;
				break; 
			}
			case 'p': {
				swap = true;
				break; 
			}
			case 'l': {
				length = true;
				break; 
			}
			case 'o': {
				if(optarg)
					outputFormat = *optarg;
				break; 
			}
			case 'b': {
				if(optarg) 
					wBegin = (" " + std::string(optarg));
				break; 
			}
			case 'e': {
				if(optarg)
					wEnd = (" " + std::string(optarg));
				break; 
			}
			case 'h': {
				printHelp = true;
				break; 
			}
			default: {
				break; 
			}
		}
	}
	
	return validateOptionInput();
}

int Letterman::getData() {
	readData(&wBegin, length);
	dSize = int(d.size()); 
	return validateDataInput(); 
}

void Letterman::setUnsetUsed(std::string & str, bool used) {
	if (used) str[0] = '/'; 
	if (!used) str[0] = ' ';
}

bool Letterman::isUsed(std::string & str) {
	if (str.length() < 2) return false; 
	if (str[0] == '/') return true; 
	if (str[0] == ' ') return false;
	return false;
}

void Letterman::pathFind() { 
	start = findWord(wBegin); 
	current = start; 
	end = findWord(wEnd);
	deq.push_back(current);
	setUnsetUsed(d[current], true);

	while (!isUsed(d[end]) && !deq.empty()) {
		if (useQueue) {
			//addToOutput(false);
			current = deq.front();
			deq.pop_front();

		}
		if (useStack) {
			//addToOutput(false);
			current = deq.back();
			deq.pop_back();
		}
		findSimilar();
	}
	if (!deq.empty()) {
		printOutput(); 
	}
	if (deq.empty()) {
		
		checked = 0;
		for (std::string & str : d) {
			if (isUsed(str)) ++checked; 
		}
		std::cout << "No solution, "<< checked << " words checked.\n";
	}
}

// private functions; class helpers

int Letterman::findWord(const std::string & word) {
	for (int i = 0; i < dSize; ++i) {
		if (d[i] == word) 
			return i; 
	}
	return -1;
}

Letterman::changeEntry* Letterman::findInChangeVec(int indexToFind) {
	for (int i = 0; i < int(changesVec.size()); ++i) {
		if (changesVec[i].wordIndex == indexToFind) 
			return &changesVec[i]; 
	}
	return nullptr;
}

void Letterman::printOutput() {
	std::string tempString; 
	std::vector<std::string> printVec; 
	Letterman::changeEntry* ce = findInChangeVec(end); 
	while (ce != nullptr) {
		if (outputFormat == 'W') {
			d[ce->wordIndex].erase(0, 1);
			tempString = d[ce->wordIndex] + "\n";
			printVec.push_back(tempString);
		}
		else if (outputFormat == 'M') {
			char changeType = ce->changeType; 
			tempString = changeType; 
			tempString += ",";
			tempString += std::to_string(ce->changeIndex - 1);
			if (changeType == 'c' || changeType == 'i') {
				tempString += ","; 
				tempString += (d[ce->wordIndex][ce->changeIndex]);
			}
			tempString += "\n"; 
			printVec.push_back(tempString);
		}
		ce = findInChangeVec(ce->parentIndex); 
	}
	int wordsInMorph = int(printVec.size()) + 1;
	std::cout << "Words in morph: " << wordsInMorph << "\n"; 
	std::cout << d[start].erase(0,1) << "\n"; 
	for (int i = int(printVec.size()) - 1; i >= 0; --i) {
		std::cout << printVec[i]; 
	}
}

void Letterman::findSimilar() {

	// set current word string to current word
	wCurrent = &d[current];

	// loop through entire dictionary
	for (int i = 0; i < dSize; ++i) {
		index = i;
		// if not already used.. 
		if (!(isUsed(d[i]) || &d[i] == wCurrent)) {
			
			// update current index word string, current length, and index length
			wIndex = &d[i];
			lCurrent = int(wCurrent->length());
			lIndex = int(wIndex->length());

			// for each word, if either condition matches
			if (lengthMatch()) {
				// mark word as used
				setUnsetUsed(d[i], true);

				// add word to stack/queue
				deq.push_back(i);
				addToChangeVec();
				if (i == end) return;
			}
			if (changeMatch()) {
				// mark word as used
				setUnsetUsed(d[i], true);

				// add word to stack/queue
				deq.push_back(i);
				addToChangeVec();
				if (i == end) return;
			}
			if (swapMatch()) {
				// mark word as used
				setUnsetUsed(d[i], true);

				// add word to stack/queue
				deq.push_back(i);
				addToChangeVec();
				if (i == end) return;
			}
		}
	}
}

void Letterman::addToChangeVec() {
	changesVec.push_back(changeEntry{
		changeParent,
		wordIndex,
		changeIndex,
		changeType
		}); 
}

void Letterman::resetChangeVariables() {
	changeParent = -1;
	wordIndex = -1; 
	foundChange = false;
	changeType = '\0';
	changeIndex = -1;
	return; 
}

void Letterman::updateChangeVariables(char _changeType, int _changeIndex, bool _foundChange ) {
	changeParent = current;
	wordIndex = index;
	foundChange = _foundChange;
	changeType = _changeType;
	changeIndex = _changeIndex;
	return;
}

bool Letterman::lengthMatch() {
	if (!length) return false;
	if (std::abs(lCurrent - lIndex) != 1 ) return false;

	resetChangeVariables();

	for (int iC = 1, iI = 1; (iC < lCurrent) && (iI < lIndex); ++iC, ++iI) {
		if ((*wCurrent)[iC] != (*wIndex)[iI]) {
			if (foundChange) return false; 
			if (lCurrent > lIndex) {
				if ((*wCurrent)[iC + 1] != (*wIndex)[iI]) return false;
				else {
					updateChangeVariables('d', iI, true); 
					++iC;
				}
			}
			else if (lIndex > lCurrent) {
				if ((*wCurrent)[iC] != (*wIndex)[iI + 1]) return false;
				else {
					updateChangeVariables('i', iC, true);
					++iI;
				}
			}
		}
	}
	if (!foundChange) {
		if (lCurrent > lIndex) {
			updateChangeVariables('d', lCurrent - 1, true);
		}
		else if (lCurrent < lIndex) {
			updateChangeVariables('i', lIndex - 1, true); 
		}
	}
	return true; 
}

bool Letterman::changeMatch() {
	if (!change) return false;
	if (lCurrent != lIndex) return false;

	resetChangeVariables();
	
	for (int i = 1; i < lCurrent; ++i) {
		if ((*wCurrent)[i] != (*wIndex)[i]) {
			if (foundChange) {
				return false;
			}
			updateChangeVariables('c', i, true);
		}
	}
	return true;
}

bool Letterman::swapMatch() {
	if (!swap) return false; 
	if (lCurrent != lIndex) return false;

	resetChangeVariables(); 

	for (int i = 1; i < lCurrent; ++i) {
		if ((*wCurrent)[i] != (*wIndex)[i]) {
			if (foundChange) {
				return false;
			}
			if (i + 1 < lCurrent) {
				if ((*wCurrent)[i + 1] == (*wIndex)[i] &&
					(*wCurrent)[i] == (*wIndex)[i + 1]) {
					updateChangeVariables('s', i, true);
					i += 1;
				}
				else
					return false; 
			}
			else {
				return false;
			}
		}
	}
	return true;
}

int Letterman::validateOptionInput() {
	if (printHelp) {
		std::cout << "Letterman Pathfinding Program Help\n\n";
		std::cout << "-----------------------------------------------------------------------------------\n\n" << std::flush;
		std::cout << "                                   GENERAL:\n\n" << std::flush;
		std::cout << "Given a dictionary inputted through the standard console input, std::cin, and both \n" << std::flush;
		std::cout << "a starting and ending word, this program will find a path between the two words, \n" << std::flush;
		std::cout << "using words from the dictionary and making changes allowed as specified by the \n" << std::flush;
		std::cout << "command line arguments.\n\n" << std::flush;
		std::cout << "-----------------------------------------------------------------------------------\n\n" << std::flush;
		std::cout << "                             COMMAND LINE OPTIONS:\n\n" << std::flush;
		std::cout << "              --stack,  -s:  if this switch is set, use the stack \n" << std::flush;
		std::cout << "                             based routing routine. \n\n" << std::flush;

		std::cout << "              --queue,  -q:  if this switch is set, use the queue \n" << std::flush;
		std::cout << "                             based routing routine. \n\n" << std::flush;

		std::cout << "              --change, -c:  if this switch is set, allow the \n" << std::flush;
		std::cout << "                             program to change one letter to another\n\n" << std::flush;

		std::cout << "              --swap,   -p:  if this switch is set, allow the \n" << std::flush;
		std::cout << "                             program to swap any two characters. \n\n" << std::flush;

		std::cout << "              --length, -l:  if this switch is set, allow the \n" << std::flush;
		std::cout << "                             program to modify the length of a word.\n\n" << std::flush;

		std::cout << "  --output (W|M), -o (W|M):  indicates the output file format, with\n" << std::flush;
		std::cout << "                             either a W (word format) or an M \n" << std::flush;
		std::cout << "                             (modification format). Defaults to W; \n" << std::flush;
		std::cout << "                             however if --output is specified without\n" << std::flush;
		std::cout << "                             an argument after an error will result. \n\n" << std::flush;

		std::cout << "--begin <word>, -b <word> :  this switch MUST be set, and MUST be \n" << std::flush;
		std::cout << "                             followed with a word argument for the \n" << std::flush;
		std::cout << "                             word for letterman to start with. \n\n" << std::flush;

		std::cout << "  --end <word>, -b <word> :  this switch MUST be set, and MUST be \n" << std::flush;
		std::cout << "                             followed with a word argument for the \n" << std::flush;
		std::cout << "                             word for letterman to end with. \n\n" << std::flush;

		std::cout << "               --help, -h :  bring up this helpful help message!! \n\n" << std::flush;
		return 0; 
	}
	if (wBegin == "") {
		exitMessage("--begin flag MUST be included to run program.", 1);
		return 1;
	}
	if (wEnd == "") {
		exitMessage("--end flag MUST be included to run program.", 1);
		return 1;
	}
	if (outputFormat == '\0') {
		outputFormat = 'W'; 
	}
	if (!(outputFormat == 'W' || outputFormat == 'M')) {
		exitMessage("--output flag must be followed by exactly one of (W|M); another character was given.", 1);
		return 1;
	}
	if (useQueue && useStack) {
		exitMessage("Legal command line must contain either --stack or --queue, not both.", 1);
		return 1;
	}
	if (!(useQueue || useStack)) {
		exitMessage("Please include either the --stack, -s flag or the --queue -q flag\n to indicate a routing scheme.", 1);
		return 1;
	}
	if (!(change || swap || length)) {
		exitMessage("Please include AT LEAST one of: \n--change (-c), --swap (-p), or --length (-l) options", 1);
		return 1;
	}
	if (wBegin == "" || wEnd == "") {
		exitMessage("Please include both a begins (--begin, -b) and ends (--end, -e) flag.", 1);
		return 1;
	}
	if (!length && (wBegin.length() != wEnd.length())) {
		exitMessage("The start and end words to this solution are different lengths, and the\n"
			"--length -l flag is not checked. Please rerun with a possible situation.", 1); 
		return 1;
	}

	return -1; 
}

int Letterman::validateDataInput() {
	if (!contStartEnd(wBegin, wEnd)) {
		exitMessage("Either start or end word is not contained in dictionary; please \n"
			"either choose another dictionary or different start/end words.", 1);
		return 1;
	}
	return -1; 
}

void Letterman::exitMessage(std::string msg, int exitStatus) {
	if (exitStatus != 0)
		std::cerr << msg.c_str() << std::endl << std::flush;
	else
		std::cout << msg.c_str() << std::endl;
	msg = ""; 
}

void Letterman::simpleRead(std::string &s, std::string * strp, bool length) {
	if (length || (int(strp->length()) == int(s.length()) + 1))
		d.push_back({ " " + s, false });
}

void Letterman::complexRead(std::string &s, std::string * strp, bool length) {

	// index of potential special character(s)
	size_t special = 0;

	// index within specialCharacters vector at which the char to compare lies
	int specialCharacterIndex = 0;

	// loop through all possible special characters, checking each iteration that
	// one is not present. If one is present, break the loop. 
	// Note that loop will always break as the last character in the specialCharacters
	// vector is an empty string, and this is always contained within any string.
	while (specialCharCheck(s, specialCharacters[specialCharacterIndex], special)) ++specialCharacterIndex;

	// grab the char representation of whichever special character was in the word s
	char c = specialCharacters[specialCharacterIndex][0];

	// switch based on this char
	switch (c) {

		// ampersand case
	case '&': {
		s.erase(int(special), 1);
		if (length || (int(strp->length()) == int(s.length()) + 1))
			d.push_back((" " + s));
		std::reverse(s.begin(), s.end());
		if (length || (int(strp->length()) == int(s.length()) + 1))
			d.push_back((" " + s));
		break;
	}

			  // indexing notation case
	case '[': {
		std::string front = s.substr(0, special),
			back = s.substr(s.find("]") + 1, s.length());
		for (int i = int(special) + 1; s[i] != ']'; ++i) {
			if (length || (int(strp->length()) == int((front + back).length() + 2)))
				d.push_back((" " + front + s[i] + back));
		}
		break;
	}
			  // exclamation point case
	case '!': {
		std::string front = s.substr(0, special - 2),
			back = s.substr(special + 1, s.length());
		if (length || (int((front + back).length()) + 2 == int(strp->length()) - 1)) {
			d.push_back((" " + front + s[special - 2] + s[special - 1] + back));
			d.push_back((" " + front + s[special - 1] + s[special - 2] + back));
		}
		break;
	}
			  // question mark case
	case '?': {
		std::string front = s.substr(0, special),
			back = s.substr(special + 1, s.length());
		if (length || (int((front + back).length() + 1 == strp->length())))	d.push_back(( " " + front + back ));
		if (length || (int((front + back).length() + 2 == strp->length()))) d.push_back(( " " + front + s[special - 1] + back ));
		break;
	}
			  // default (no special char) case
	default: {
		if (length || (int(s.length()) == int(strp->length()) - 1) )
			d.push_back((" " + s));
		break;
	}
	}
}

bool Letterman::specialCharCheck(std::string &s, std::string &c, size_t &special) {
	// assign value to special char index, special
	special = s.find(c);

	// return whether or not a special char was in the string at all
	// (string::npos is a huge unsigned number; no english words are that long, luckily)
	return (special == std::string::npos);
}