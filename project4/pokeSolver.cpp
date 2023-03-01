#include "pokeSolver.h"


int pokeSolver::getOptions(int argc, char** argv) {
	int optionIndex = 0, option = 0;

	struct option longOpts[] = {
		{ "mode", required_argument, nullptr, 'm' },
		{ "help", no_argument, nullptr, 'h' },
		{ nullptr, 0, nullptr, '\0' }
	};

	while ((option = getopt_long(argc, argv,
		"m:h", longOpts, &optionIndex)) != -1) {

		switch (option) {
			case 'm': {
				if (optarg)
					mode = optarg[0];
				else
					mode = 'n'; 
				break;
			}
			case 'h': {
				help = true; 
				break;
			}
			default: {
				break;
			}
		}
	}

	return validateOptionInput();
}

int pokeSolver::validateOptionInput() {
	if (help) {
		cout << "Welcome to pokeSolver!! \n"
			"Correct usage is including a \n"
			"--mode or -m flag, followed by \n"
			"one of MST, FASTTSP, or OPTTSP.\n"
			"The -m/--mode flag is required!!\n";
		return 0; 
	}
	if (!mode) {
		cout << "Please include --mode/-m flag for proper program running!!\n"; 
		return 1; 
	}
	if (!(mode == 'M' ||
		mode == 'F' ||
		mode == 'O')) {
		cout << "Please select one of:\n"
			"MST, FASTTSP, OPTTSP for proper execution.\n"; 
		return 1;
	}
	return -1;
}

void pokeSolver::readVerticies() {
	int x, y;
	verticies.reserve(verts);
	for (int i = 0; i < verts; ++i) {
		cin >> x;
		cin >> y;
		cout.flush(); 
		if (x < 0 && y < 0)
			waterVerts++;
		else if (x > 0 || y > 0)
			landVerts++;
		else
			edgeVerts++; 
		verticies.push_back({ x, y });
	}
}

void pokeSolver::readVerticiesB(vector<vb> & bvec) {
	int x, y;
	bvec.reserve(verts);
	for (int i = 0; i < verts; ++i) {
		cin >> x;
		cin >> y;
		bvec.push_back({ x, y });
	}
}

int pokeSolver::runMode() {
	cin >> verts; 
	switch (Mode(mode)) {
		case Mode::MST: {
			readVerticies();
			return runMST();
		}
		case Mode::FTSP: {
			return runFTSP();
		}
		case Mode::OTSP: {
			return runOTSP();
		}
	}
	return 0; 
}		

int pokeSolver::runMST() {
	if (!checkValidMSTSet()) {
		std::cerr << "Cannot construct MST\n";
		return 1;
	}
	findMST();
	outputMST();
	verticies.clear();
	return 0;
}

double pokeSolver::distanceB(int v1, int v2) {
	return std::sqrt(double(bvec[v1].x - bvec[v2].x)*double(bvec[v1].x - bvec[v2].x) + 
					 double(bvec[v1].y - bvec[v2].y)*double(bvec[v1].y - bvec[v2].y));
}

int pokeSolver::findNext(int cur) {
	double min = std::numeric_limits<double>::infinity(), temp;
	int index = -1; 
	for (int i = cur + 1; (i % verts) != cur; ++i) {
		temp = distanceB(cur, i); 
		if (temp < min) {
			min = temp; 
			index = i; 
		}
	}
	return index; 
}

double pokeSolver::findNextValue(int cur) {
	double min = std::numeric_limits<double>::infinity(), temp;
	for (int i = cur + 1; (i % verts) != cur; ++i) {
		temp = distanceB(cur, i);
		if (temp < min) {
			min = temp;
		}
	}
	return min;
}

void pokeSolver::arbitraryInsert(vector<int> & tour, vector<bool> & used) {

	tour.reserve(verts);
	tour.push_back(0);
	int n = findNext(0);

	used[0] = true;
	used[n] = true;

	tour.push_back(n);
	sum = 0;
	sum += 2 * distanceB(tour[0], tour[1]);
	int inserted = 2, minIndex = -1, toInsert = 1;
	double minCost = std::numeric_limits<double>::infinity();
	double tempCost;
	while (inserted < verts) {
		while (used[toInsert])
			++toInsert;
		minCost = std::numeric_limits<double>::infinity();
		for (int i = 0; i < int(tour.size()) - 1; ++i) {
			tempCost = sum - distanceB(tour[i], tour[i + 1]);
			tempCost += distanceB(tour[i], toInsert) + distanceB(tour[i + 1], toInsert);
			if (tempCost < minCost) {
				minCost = tempCost;
				minIndex = i;
			}
		}
		sum = minCost;
		tour.insert(tour.begin() + minIndex + 1, toInsert);
		used[toInsert] = true;
		inserted++;
	}
}

int pokeSolver::runFTSP() {
	readVerticiesB(bvec);

	vector<int> tour; 
	vector<bool> used(verts, false); 

	arbitraryInsert(tour, used); 

	outputTSP(tour); 
	return 0; 
}

void pokeSolver::outputTSP(vector<int> & t) {
	int i = 0;
	while (t[i % verts])
		++i; 
	cout << sum << "\n"; 
	cout << t[i] << " "; 
	for (int j = i + 1; (j % verts) != i; ++j) {
		cout << t[j % verts] << " "; 
	}
	cout << "\n"; 
}

void pokeSolver::setZero() {
	int i = 0; 
	while (path[i] != 0) {
		std::rotate(path.begin(), path.begin() + i, path.end()); 
	}
}

int pokeSolver::runOTSP() {

	readVerticiesB(bvec);

	vector<bool> used(verts, false);

	arbitraryInsert(path, used);
	fillDMAT(); 
	setZero(); 
	lenBest = tourLength(verts); 
	pathBest = path; 
	setZero();

	len = 0; 
	estimate = 0; 
	contained.resize(verts); 
	distances.resize(verts); 
	genPerms(1);
	
	sum = lenBest;
	outputTSP(pathBest);
	//cout << permcnt; 
	return 0; 
}

void pokeSolver::genPerms(int permLength) {
	len += dmat[path[(permLength - 1)%permLength]][path[(permLength - 2)%permLength]];
	if (verts == permLength) {
		len += dmat[path[permLength - 1]][path[0]];
		if (len < lenBest) {
			pathBest = path;
			lenBest = len; 
		}
		len -= dmat[path[permLength - 1]][path[0]];
		len -= dmat[path[(permLength - 1) % permLength]][path[(permLength - 2) % permLength]];
		return;
	}
	if (!promising(permLength)) {
		len -= dmat[path[(permLength - 1) % permLength]][path[(permLength - 2) % permLength]];
		return;
	}
	permcnt++;
	for (int i = permLength; i < verts; ++i) {
		std::swap(path[permLength], path[i]);
		genPerms(permLength + 1);
		std::swap(path[permLength], path[i]);
	}
	len -= dmat[path[(permLength - 1) % permLength]][path[(permLength - 2) % permLength]];
}

void pokeSolver::fillDMAT() {
	dmat.resize(verts, vector<double>(verts)); 
	for (int i = 0; i < verts; ++i) {
		for (int j = 0; j < verts; ++j) {
			dmat[i][j] = distanceB(i, j); 
		}
	}
}

bool pokeSolver::promising(int permLength) {
	if (verts - permLength < 6)
		return true;
	estimate = len;//+ dmat[path[0]][path[permLength - 1]];
	if (estimate > lenBest) {
		//cout << "F: " << estimate << " " << lenBest << " " << permLength << "\n";
		return false;
	}
	estimate += mstSize(permLength);
	if (estimate > lenBest) {
		//cout << "F: " << estimate << " " << lenBest << " " << permLength << "\n";
		return false;
	}
	estimate += groupSeparation(permLength);
	if (estimate > lenBest) {
		//cout << "F: " << estimate << " " << lenBest << " " << permLength << "\n";
		return false;
	}
	//cout << "P: " << estimate << " " << lenBest << " " << permLength << "\n";
	return true;
}

double pokeSolver::mstSize(int permLength) {
	tempdbl = 0; 
	std::fill(contained.begin() + permLength, contained.end(), false); 
	std::fill(distances.begin() + permLength, distances.end(), std::numeric_limits<double>::infinity()); 
	int spanSize = permLength; 
	int cur; 
	cur = permLength; 
	double min; 
	distances[cur] = 0; 
	while (spanSize < verts) {
		min = std::numeric_limits<double>::infinity(); 
		for (int i = permLength; i < verts; ++i) {
			if(!contained[i]) {
				if (distances[i] < min) {
					cur = i;
					min = distances[cur];
				}
			}
		}
		tempdbl += min; 
		spanSize++; 
		contained[cur] = true; 
		for (int i = permLength; i < verts; ++i) {
			if (!contained[i]) {
				if (dmat[path[cur]][path[i]] < distances[i]) {
					distances[i] = dmat[path[cur]][path[i]];
				}
			}
		}
	}
	return tempdbl;
}

double pokeSolver::groupSeparation(int permLength) {
	double min1 = std::numeric_limits<double>::infinity(); 
	for (int i = permLength; i < verts; ++i) {
		if (dmat[path[0]][path[i]] < min1)
			min1 = dmat[path[0]][path[i]]; 
	}
	double min2 = std::numeric_limits<double>::infinity(); 
	for (int i = permLength; i < verts; ++i) {
		if (dmat[path[permLength - 1]][path[i]] < min2)
			min2 = dmat[path[permLength - 1]][path[i]]; 
	}
	return min1 + min2; 
}

double pokeSolver::tourLength(int permLength) {
	if (permLength < 2)
		return 0;
	double s = 0;
	for (int i = 1; i < permLength; ++i) {
		s += distanceB(path[i], path[i - 1]);
	}
	s += distanceB(path[permLength - 1], path[0]);
	return s;
}

void pokeSolver::printAdjList() {
	for (int i = 0; i < verts; ++i) {
		cout << i << ": ";
		for (auto elt : adjList[i]) {
			cout << elt << " ";
		}
		cout << "\n";
	}
}


//void pokeSolver::findMSTUtil() {
//	double td;
//	int temp = 0;
//	verticies[0].distance = 0;
//	spanSize = 0;
//	while (spanSize < verts) {
//		temp = getNextVert();
//		spanSize++;
//		verticies[temp].used = true;
//
//		for (int i = 0; i < verts; ++i) {
//			if (!verticies[i].used) {
//				td = distanceCalc(verticies[i], verticies[temp]);
//				if (verticies[i].distance > td) {
//					if (verticies[i].distance != std::numeric_limits<double>::infinity())
//						distanceSum -= verticies[i].distance;
//					distanceSum += td;
//					verticies[i].distance = td;
//					verticies[i].closest = temp;
//				}
//			}
//		}
//	}
//}


void pokeSolver::findMST() {
	double td;
	int temp = 0;
	verticies[0].distance = 0;
	spanSize = 0;
	distanceSum = 0; 
	while (spanSize < verts) {
		temp = getNextVert();
		spanSize++;
		verticies[temp].used = true;
		for (int i = 0; i < verts; ++i) {
			if (!verticies[i].used && MSTcanReach(i, temp)) {
				td = distanceCalc(verticies[i], verticies[temp]);
				if (verticies[i].distance > td) {
					if (verticies[i].distance != std::numeric_limits<double>::infinity())
						distanceSum -= verticies[i].distance;
					distanceSum += td;
					verticies[i].distance = td;
					verticies[i].closest = temp;
				}
			}
		}
	}
}

double pokeSolver::distanceCalc(const v & a, const v & b) {
	return std::sqrt(double(a.x - b.x)*double(a.x - b.x) + double(a.y - b.y)*double(a.y - b.y));
}

int pokeSolver::getNextVert() {
	int minIndex = -1; 
	double min = std::numeric_limits<double>::infinity(); 
	for (int i = 0; i < verts; ++i) {
		if (!verticies[i].used) {
			if (verticies[i].distance < min) {
				min = verticies[i].distance;
				minIndex = i;
			}
		}
	}
	return minIndex; 
}

bool pokeSolver::checkValidMSTSet() {
	if (!verts)
		return false; 
	if ((landVerts && waterVerts) && !edgeVerts)
		return false; 
	return true; 
}

void pokeSolver::outputMST() {
	cout << distanceSum << std::endl;
	for (int i = 1; i < verts; ++i) {
		i < verticies[i].closest ? cout << i << " " << verticies[i].closest : cout << verticies[i].closest << " " << i;
		cout << "\n"; 
	}
}

bool pokeSolver::MSTcanReach(int i, int index) {
	if ((onLand(i) && onWater(index)) || (onWater(i) && onLand(index)))
		return false; 
	return true; 
}

bool pokeSolver::onWater(int i) {
	return verticies[i].x < 0 && verticies[i].y < 0; 
}

bool pokeSolver::onLand(int i) {
	return verticies[i].x > 0 || verticies[i].y > 0; 
}


//void pokeSolver::findOdds() {
//	for (int i = 0; i < verts; ++i) {
//		if (adjList[i].size() % 2) {
//			odds.push_back(i); 
//		}
//	}
//}
//
//void pokeSolver::findPerfectMatching() {
//	findOdds(); 
//	auto cur = odds.begin();
//	auto minit = odds.end(); 
//	double td = 0, min = std::numeric_limits<double>::infinity();
//	while (odds.size()) {
//		cur = odds.begin(); 
//		auto it = cur + 1; 
//		minit = odds.end(); 
//		for (; it != odds.end(); ++it) {
//			td = distanceCalc(verticies[*it], verticies[*cur]); 
//			if (td < min) {
//				if (std::find(adjList[*cur].begin(), adjList[*cur].end(), *it) == adjList[*cur].end()) {
//					min = td;
//					minit = it;
//				}
//			}	 
//		}
//		if (minit == odds.end()) {
//			leftoverOdds = { *cur, *cur + 1 };
//			odds.erase(cur, cur + 1); 
//
//		}
//		else {
//			adjList[*minit].push_back(*cur);
//			adjList[*cur].push_back(*minit);
//			odds.erase(minit);
//			odds.erase(cur);
//		}
//
//		min = std::numeric_limits<double>::infinity();
//	}
//}
//
//void pokeSolver::findEuler() {
//	 
//	if (!(leftoverOdds.first < 0))
//		TSPSTART = leftoverOdds.first; 
//	eulerStack.push_front(TSPSTART); 
//	int cur = eulerStack.front();
//	int temp = 0; 
//	while (!eulerStack.empty() || adjList[cur].size() > 0) {
//		// if cur has no neighbors
//		if (adjList[cur].empty()) {
//			eulerPath.push_back(cur);
//			cur = eulerStack.front();
//			eulerStack.pop_front();
//		}
//		// else if it has neighbors
//		else {
//			// add "the vertex to the stack" (?? hm okey) 
//			eulerStack.push_front(cur); 
//			temp = cur; 
//			cur = adjList[cur].back(); 
//			adjList[temp].pop_back(); 
//			adjList[cur].erase(std::find(adjList[cur].begin(), adjList[cur].end(), temp)); 
//		}
//	}
//}
//
//
//void pokeSolver::outputFTSP() {
//	cout << sum << "\n";
//	for (auto elt : hamilton)
//		cout << elt << " ";
//	cout << "\n";
//}
//
//
//void pokeSolver::getHam() {
//	hamilton.resize(verts, 0); 
//	vector<int>::iterator it = eulerPath.begin(); 
//	while (*it != 0)
//		it++; 
//	int cnt = 0;
//	sum = 0;
//
//	for (vector<int>::iterator elt = it; elt != eulerPath.end(); elt++) {
//		if (!verticies[*elt].used) {
//			hamilton[cnt++] = *elt; 
//			verticies[*elt].used = true;
//		}
//	}
//		 
//	for (vector<int>::iterator elt = eulerPath.begin(); elt != it; elt++) {
//		if (!verticies[*elt].used) {
//			hamilton[cnt++] = *elt;
//			verticies[*elt].used = true;
//		}
//	}
//	for (int i = 1; i < verts; ++i) {
//		sum += distanceCalc(verticies[hamilton[i - 1]], verticies[hamilton[i]]);
//	}
//	sum += distanceCalc(verticies[hamilton[0]], verticies[hamilton.back()]);
//}

//void pokeSolver::mstMethod() {
//	int temp = 0; int cnt = 0;
//	deque<int> toVisit;
//	vector<int> visited;
//	toVisit.push_front(0);
//	double sum = 0;
//	while (!toVisit.empty()) {
//		temp = toVisit.front();
//		toVisit.pop_front();
//		cnt = 0;
//		visited.push_back(temp);
//		if (!verticies[temp].used) {
//			for (auto i : adjList[temp])
//				if (!verticies[i].used) {
//					toVisit.push_front(i);
//					cnt++;
//				}
//			if (!cnt) {
//				toVisit.push_front(visited.back());
//			}
//		}
//	}
//	cout << temp << "\n";
//	verticies[temp].used = true;
//	for (int i = 1; i < verts; i++) {
//		sum += distanceCalc(verticies[visited[i - 1]], verticies[visited[i]]);
//	}
//	sum += distanceCalc(verticies[visited[0]], verticies[visited.back()]); 
//	cout << sum << "\n"; 
//	for (auto elt : visited)
//		cout << elt << " "; 
//	cout << "\n";
//}
//
//void pokeSolver::findEulerianCycle(vector<int> & v) {
//	//	2. Choose a leaf node i_0 (i.e.node of degree 1). Set i : = i_0.
//	TSPSTART = 0; 
//	while (!(adjList[TSPSTART].size() % 2))
//		TSPSTART++; 
//	int i = TSPSTART, temp; 
//	deque<int> eulerStack; 
//	eulerStack.push_front(i); 
//	//	3. If an untraversed edge[i, j] incident to i exists, go from i to j(edge[i, j] is added to the path).
//	//		Set i : = j and repeat step 3.
//	//		If all edges from i are traversed, return to the first predecessor of i, be it node k.If k = i(the o
//	//		starting node), an Eulerian cycle is formed : go to 4. Else, set i : = k and repeat step 3..
//	while (!eulerStack.empty() || adjList[i].size() > 0) {
//		if (adjList[i].empty()) {
//			v.push_back(i); 
//			i = eulerStack.front(); 
//			eulerStack.pop_front(); 
//		}
//		else {
//			eulerStack.push_front(i); 
//			temp = i; 
//			v.push_back(i);
//			i = adjList[i].back(); 
//			adjList[temp].pop_back(); 
//			adjList[i].erase(std::find(adjList[i].begin(), adjList[i].end(), temp)); 
//		}
//	}
//}
//
//void pokeSolver::createAdjList() {
//	//distFunctor d{ 0, verticies }; 
//	adjList.resize(verts); 
//	verticies[0].used = false; 
//	for (int i = 1; i < verts; i++) {
//		verticies[i].used = false; 
//		adjList[verticies[i].closest].push_back(i);
//		adjList[i].push_back(verticies[i].closest);
//	}
//}
//
//void pokeSolver::findHamiltonianCycle(vector<int> & v) {
//	int last = 0;
//	verticies[v[last]].used = true;
//	sum = 0;
//	for (int i = 1; i < int(v.size()); ++i) {
//		if (!verticies[v[i]].used) {
//			verticies[v[i]].used = true;
//			verticies[v[i]].closest = v[last];
//			last = i;
//		}
//	}
//	verticies[v[0]].closest = v[last];
//	v.resize(verts);
//	int cnt = 0, cur = 0;
//	while (cnt < verts) {
//		v[cnt] = cur;
//		cur = verticies[cur].closest;
//		cnt++;
//	}
//	sum = tourLength(v);
//}
//
//void pokeSolver::outputTSP(vector<int> & v) {
//	cout << tourLength(v) << "\n"; 
//	for (auto elt : v)
//		cout << elt << " "; 
//	//int cnt = 0, cur = 0;
//	//cout << sum << "\n";
//	//while (cnt < verts) {
//	//	cout << cur << " ";
//	//	cur = verticies[cur].closest;
//	//	cnt++;
//	//}
//	//cout << "\n" << cnt;
//}
//
//double pokeSolver::tourLength(vector<int> & v) {
//	double sum = 0;
//	for (int i = 1; i < int(v.size()); ++i) {
//		sum += distanceCalc(verticies[v[i]], verticies[v[i - 1]]);
//	}
//	sum += distanceCalc(verticies[v[0]], verticies[v.back()]);
//	return sum;
//}
//
//void pokeSolver::twoOptSmall(vector<int> & v) {
//	double change = 0, minChange = 0; 
//	double last = sum + 1; 
//	int mini = 0, minj = 0;
//	int i1, i2, j1, j2; 
//	bool changed = true; 
//	int count = 0; 
//	while(changed && sum < last && count < 100) {
//		changed = false; 
//		minChange = 0; 
//		for (int i = 0; i < verts - 2; ++i) {
//			i1 = i; 
//			i2 = (i + 1) % verts; 
//			for (int j = i + 2; (j + 1) % verts != i; j++) {
//				j1 = j % verts; 
//				j2 = (j + 1) % verts; 
//				change = distanceCalc(verticies[v[i1]], verticies[v[j1]]) + distanceCalc(verticies[v[i2]], verticies[v[j2]]) -
//					distanceCalc(verticies[v[i1]], verticies[v[i2]]) - distanceCalc(verticies[v[j1]], verticies[v[j2]]);
//				if (change < minChange) {
//					mini = i; minj = j;
//					minChange = change;
//					//cout << change << "\n"; 
//					changed = true; 
//				}
//			}
//		}
//		if (changed) {
//			last = sum; 
//			std::reverse(v.begin() + mini + 1, v.begin() + minj + 1);
//			sum = tourLength(v);
//			count++; 
//			//cout << "iteration " << count << ": " << sum << "\n"; 
//		}
//	}
//
//}
//
//void pokeSolver::twoOptLarge(vector<int> & v) {
//	double change = 0, minChange = 0;
//	int mini = 0, minj = 0;
//	int i1, i2, j1, j2;
//	bool changed = true;
//	bool everChanged = true; 
//	int count = 0;
//	int countN = 2100; 
//	while (everChanged && count < countN) {
//		everChanged = false; 
//		for (int i = 0; i < verts - 2 && count < countN; ++i) {
//			i1 = i;
//			i2 = (i + 1) % verts;
//			minChange = 0;
//			changed = false; 
//			for (int j = i + 2; (j + 1) % verts != i && count < countN; j++) {
//				j1 = j % verts;
//				j2 = (j + 1) % verts;
//				change = distanceCalc(verticies[v[i1]], verticies[v[j1]]) + distanceCalc(verticies[v[i2]], verticies[v[j2]]) -
//					distanceCalc(verticies[v[i1]], verticies[v[i2]]) - distanceCalc(verticies[v[j1]], verticies[v[j2]]);
//				if (change < minChange) {
//					mini = i % verts; minj = j % verts;
//					minChange = change;
//					changed = true;
//				}
//			}
//			if (changed && mini != minj) {
//				std::reverse(v.begin() + mini + 1, v.begin() + minj + 1); 
//				change = tourLength(v);
//				if (change < sum) {
//					sum = change;
//					everChanged = true; 
//					count++; 
//					//cout << count++ << " " << mini << " " << minj << " " << sum << "\n";
//				}
//				else
//					std::reverse(v.begin() + mini + 1, v.begin() + minj + 1); 
//			}
//		}
//		//cout << "iteration " << count << ": " << sum << "\n"; 
//	}
//}
