/*
 * Compile this test against your .h files to make sure they compile. Note how
 * the eecs281 priority queues can be constructed with the different types. We
 * suggest adding to this file or creating your own test cases to test your
 * priority queue implementations more thoroughly.  If you do not call a
 * function from here, it is NOT compiled due to templates!  So for instance,
 * if you don't add code to test updatePriorities(), you could have compiler
 * errors that you don't even know about.
 *
 * Notice that testPairing() tests the range-based constructor but main() and
 * testPriorityQueue() do not.  Make sure to test the range-based constructor
 * for other PQ types, and also test the PairingPQ-specific member functions.
 *
 * These tests are NOT a complete test of your priority queues!
 */

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "BinaryPQ.h"
#include "Eecs281PQ.h"
#include "UnorderedPQ.h"
#include "PairingPQ.h"
#include "SortedPQ.h"



// Used to test a priority queue containing pointers to integers.
struct IntPtrComp {
    bool operator() (const int *a, const int  *b) const {
        return *a < *b;
    }
};

struct StrPtrComp {
	bool operator() (const std::string *a, const std::string *b) {
		return *a > *b; 
	}
};

Eecs281PQ<int>* PQFactory(const std::string &pqType) {
	Eecs281PQ<int> * pq = nullptr;
	if (pqType == "Sorted")
		pq = new SortedPQ<int>;
	else if (pqType == "Binary")
		pq = new BinaryPQ<int>;
	else if (pqType == "Pairing")
		pq = new PairingPQ<int>;
	else {
		pq = new UnorderedPQ<int>;
	}
	return pq;
}

void callOutTest(const std::string &name, const std::string &pqType, bool first) {
	if (first) std::cout << "Testing " << pqType << " with " << name << " function.\n";
	else std::cout << "   " << name << "() succeeded!\n";
}

void testRange(const std::string &pqType) {
	callOutTest("testRange", pqType, true);
	//class testClass {
	//public:
	//	int id;
	//	std::string name;
	//};

	//class testClassComp {
	//	bool operator()(testClass c1, testClass c2) {
	//		return c1.id < c2.id;
	//	}
	//};
	std::vector<int> v = { 1, 5, 6, 3, 2, - 12, 34, 2, 4 };

	SortedPQ<int> pq(v.begin(), v.end());
	assert(pq.top() == 34); 
	pq.pop();
	assert(pq.top() == 6); 
	assert(pq.size() == 8); 
	callOutTest("testRange", pqType, false);
}

void testIsEmpty(const std::string &pqType) {
	callOutTest("testIsEmpty", pqType, true);
	Eecs281PQ<int> * pq = PQFactory(pqType);
	assert(pq->empty()); 
	pq->push(34);
	assert(!pq->empty()); 
	delete pq;
	callOutTest("testIsEmpty", pqType, false);
}



// TODO: Make sure that you're using this-compare() properly, and everywhere
// that you should.  Complete this function by adding a functor that compares
// two HiddenData structures, create a PQ of the specified type, and call
// this function from main().
void testHiddenData(const std::string &pqType) {
    struct HiddenData {
        int data;
    };
    struct HiddenDataComp {
        bool operator()(const HiddenData &a, const HiddenData &b) {
            return a.data > b.data;
        }
    };
	Eecs281PQ< HiddenData, HiddenDataComp > * pq = nullptr; 
	std::cout << "Testing " << pqType << " with hidden data\n";
	if (pqType == "Sorted")
		pq = new SortedPQ<HiddenData, HiddenDataComp>;
	else if (pqType == "Binary")
		pq = new BinaryPQ<HiddenData, HiddenDataComp>;
	else if (pqType == "Pairing")
		pq = new PairingPQ<HiddenData, HiddenDataComp>;

	pq->push(HiddenData{ 3 });
	pq->push(HiddenData{ 4 });
	pq->push(HiddenData{ 5 });
	pq->push(HiddenData{ 17 });
	pq->push(HiddenData{ 199 }); 
	pq->push(HiddenData{ -100 }); 
	pq->updatePriorities();
	assert(pq->top().data == -100); 
	pq->pop(); 
	assert(pq->top().data == 3);
	pq->pop();
	pq->push(HiddenData{ 8 }); 
	pq->pop(); 
	pq->pop();
	assert(pq->top().data == 8);
	pq->pop();
	assert(pq->size() == 2);
	pq->pop();
	assert(pq->top().data == 199); 
	pq->pop();
	assert(pq->empty());

	std::cout << "   testHiddenData() succeeded!" << std::endl; 
	delete pq; 
} // testHiddenData()



// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePrioritiesHelper(Eecs281PQ<int *, IntPtrComp> *pq) {
	std::vector<int> data{ 1, 5 };

	pq->push(&data[0]);
    pq->push(&data[1]);
    data[0] = 10;
    pq->updatePriorities();
    assert(*pq->top() == 10);

	std::vector<std::string> data2{ "hey", "delialah", "there", "aaa", "bbb" };
	Eecs281PQ<std::string *, StrPtrComp> *pq2 = new SortedPQ<std::string *, StrPtrComp>;
	for (int i = 0; i < 3; ++i) pq2->push(&data2[i]);
	Eecs281PQ<std::string *, StrPtrComp> *pq3(pq2);
	assert(*(pq3->top()) == "delialah");
	pq3->push(&data2[3]); 
	pq3->push(&data2[4]); 
	assert(*(pq3->top()) == "aaa"); 
	data2[3] = "zzz"; 
	assert(*(pq3->top()) == "zzz"); 
	pq3->updatePriorities(); 
	assert(*(pq3->top()) == "bbb");
	for (int i = 0; i < 4; ++i) pq3->pop(); 
	assert(*(pq3->top()) == "zzz");
	pq3->pop();
	assert(pq3->empty()); 
	delete pq2; 
	std::cout << "   testUpdatePriorities() succeeded!" << std::endl;
} // testUpdatePrioritiesHelper()


// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePriorities(const std::string &pqType) {
    Eecs281PQ<int *, IntPtrComp> *pq = nullptr;
	callOutTest("testUpdatePriorities", pqType, true); 
    if (pqType == "Unordered") {
        pq = new UnorderedPQ<int *, IntPtrComp>;
		testUpdatePrioritiesHelper(pq);
    }
	if (pqType == "Sorted") {
		pq = new SortedPQ<int *, IntPtrComp>;
		testUpdatePrioritiesHelper(pq);
	}
	if (pqType == "Binary") {
		pq = new BinaryPQ<int *, IntPtrComp>;
		testUpdatePrioritiesHelper(pq); 
	}
	callOutTest("testUpdatePriorities", pqType, false);
    delete pq;
} // testUpdatePriorities()


// Very basic testing.
void testPriorityQueue(const std::string &pqType) {

	Eecs281PQ<int> *pq = PQFactory(pqType);
	std::cout << "Testing priority queue: " << pqType << std::endl;

    pq->push(3);
    pq->push(4);
    assert(pq->size() == 2);
    assert(pq->top() == 4);

    pq->pop();
    assert(pq->size() == 1);
    assert(pq->top() == 3);
    assert(!pq->empty());

    pq->pop();
    assert(pq->size() == 0);
    assert(pq->empty());

    // TODO: Add more testing here!

	std::cout << "   testPriorityQueue() succeeded!" << std::endl;
	delete pq; 
} // testPriorityQueue()


void testSize(const std::string & pqType) {
	callOutTest("testSize", pqType, true);
	Eecs281PQ<int> * pq = PQFactory(pqType);
	assert(0 == pq->size());
	pq->push(312);
	assert(1 == pq->size());
	pq->push(-34); 
	assert(2 == pq->size());
	pq->top();
	assert(2 == pq->size());
	pq->pop(); pq->pop();
	for (int i = 0; i < 1000; ++i) pq->push(i % 3);
	assert(1000 == pq->size()); 
	callOutTest("testSize", pqType, false);
	delete pq; 
}


// Test the pairing heap's range-based constructor, copy constructor,
// and operator=().  Still not complete, should have more code, test
// addNode(), updateElt(), etc.
void testPairing(std::vector<int> & vec) {
	callOutTest("testPairing", "Pairing", true);
    Eecs281PQ<int> * pq1 = new PairingPQ<int>(vec.begin(), vec.end());
    Eecs281PQ<int> * pq2 = new PairingPQ<int>(*((PairingPQ<int> *)pq1));
    // This line is different just to show two different ways to declare a
    // pairing heap: as an Eecs281PQ and as a PairingPQ. Yay for inheritance!
    PairingPQ<int> * pq3 = new PairingPQ<int>();
    *pq3 = *((PairingPQ<int> *)pq2);

    pq1->push(3);
    pq2->pop();
    assert(pq1->size() == 3);
    assert(!pq1->empty());
    assert(pq1->top() == 3);
    pq2->push(pq3->top());
    assert(pq2->top() == pq3->top());


    delete pq1;
    delete pq2;
    delete pq3;

	callOutTest("testPairing", "Pairing", false);
} // testPairing()

void testPairingBegin(const std::string & pqType) {
	callOutTest("testPairingBegin", pqType, true);
	Eecs281PQ<int> * pq = PQFactory(pqType);
	pq->push(19);
	pq->push(9);
	pq->push(2);
	pq->push(11);
	pq->push(324); 
	
	assert(pq->top() == 324); 
	pq->pop(); 
	assert(pq->top() == 19);
	pq->pop(); 
	pq->pop();
	pq->push(4);
	assert(pq->top() == 9);
	
	PairingPQ<int> * pq2 = new PairingPQ<int>; 
	pq2->push(10);
	pq2->push(3);
	pq2->push(-34);

	PairingPQ<int>::Node * n = pq2->addNode(33);

	pq2->updateElt(n, 34); 
	assert(pq2->top() == 34); 
	pq2->pop();
	pq2->push(3); 
	pq2->pop();
	pq2->pop();
	assert(pq2->top() == 3);

	pq2->pop();
	delete pq; 
	delete pq2;
	callOutTest("testPairingBegin", pqType, false);
}

void testUpdateElt(const std::string &pqType) {
	callOutTest("testUpdateElt", pqType, true);
	std::vector<int> vec = { 7, 2, 3, 6, 5 };

	PairingPQ<int> pq(vec.begin(), vec.end());
	PairingPQ<int>::Node * n = pq.addNode(4);
	assert(pq.top() == 7);
	pq.updateElt(n, 9);
	assert(pq.top() == 9); pq.pop();
	assert(pq.top() == 7); pq.pop();
	assert(pq.top() == 6); pq.pop();
	assert(pq.top() == 5); pq.pop();
	assert(pq.top() == 3); pq.pop();
	assert(pq.top() == 2); pq.pop();
	assert(pq.empty());

	std::vector<PairingPQ<int>::Node *> v;

	v.push_back(pq.addNode(4)); 
	v.push_back(pq.addNode(3));
	pq.updateElt(v[1], 5); 
	assert(pq.top() == 5); 
	pq.pop();
	assert(pq.size() == 1); 
	pq.updateElt(v[0], 10);
	assert(pq.top() == 10);
	pq.pop();
	pq.updatePriorities();
	pq = PairingPQ<int>(vec.begin(), vec.end());
	n = pq.addNode(15);
	pq.updateElt(n, 1);
	assert(pq.top() == 15); 
	pq = PairingPQ<int>(vec.begin(), vec.end());

	callOutTest("testUpdateElt", pqType, false);
}

void testUpdatePrioritiesNew(const std::string &pqType) {
	struct strGreater {
		bool operator()(const std::string & s1, const std::string & s2) {
			return s1 > s2;
		}
	};
	callOutTest("testUpdatePrioritiesNew", pqType, true);
	std::vector<std::string> vec = { "a", "b", "dogs", "i'm so dumb", "yeet", "aaa" };
	PairingPQ<std::string, strGreater> pq(vec.begin(), vec.end(), strGreater()); 
	pq.updatePriorities();
	assert(pq.top() == "a");

	PairingPQ<std::string, strGreater> pq2; 
	pq = pq2; 
	pq.updatePriorities();
	assert(pq.empty()); 

	pq = PairingPQ<std::string, strGreater>(vec.begin(), vec.end());
	pq.pop();
	PairingPQ<std::string, strGreater>::Node * n1 = pq.addNode("a"),
		*n2 = pq.addNode("z");
	pq.switchNodes(n1, n2);
	assert(pq.top() == "z"); 
	pq.updatePriorities(); 
	assert(pq.top() == "a"); 


	callOutTest("testUpdatePrioritiesNew", pqType, false);
}

int main() {
    // Basic pointer, allocate a new PQ later based on user choice.

	std::vector<std::string> types{ "Unordered", "Sorted", "Binary", "Pairing" };
    int choice;

	std::cout << "PQ tester" << std::endl << std::endl;
	for (size_t i = 0; i < types.size(); ++i)
		std::cout << "  " << i << ") " << types[i] << std::endl;
	//std::cout << std::endl;
	//std::cout << "Select one: ";
	//std::cin >> choice;
	//std::cout << std::endl;
	choice = 3; 
	testUpdatePrioritiesNew(types[choice]);	std::cout << std::endl; 
	testUpdatePriorities(types[choice]);	std::cout << std::endl;
	testRange(types[choice]);				std::cout << std::endl;
	testPairingBegin(types[choice]);		std::cout << std::endl;
    testPriorityQueue(types[choice]);		std::cout << std::endl;
	testHiddenData(types[choice]);			std::cout << std::endl;
	testIsEmpty(types[choice]);				std::cout << std::endl;
	testSize(types[choice]);				std::cout << std::endl;
	testUpdateElt(types[choice]);			std::cout << std::endl; 

    if (choice == 3) {
		std::vector<int> vec;
        vec.push_back(0);
        vec.push_back(1);
        testPairing(vec);
    } // if
	std::cout << "PASSSED EM BITCH" << std::endl;
    return 0;
} // main()
