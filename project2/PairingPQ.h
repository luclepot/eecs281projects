#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>
#include <vector>
#include <cassert>

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
		// Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child = nullptr;
            Node *sibling = nullptr;
			Node *parent = nullptr;
            // TODO: Add one extra pointer (parent or previous) as desired.
    }; // Node


    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
		heap = nullptr;
    } // PairingPQ()


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
		while (start != end) {
			auto t = *(start);
			push(t);
			start++;
		}
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare } {

  		std::deque<Node *> deq;
  		Node * current = nullptr;

  		if(other.heap)
  			deq.push_back(other.heap);

  		while (!deq.empty()) {
  			current = deq.back();
  			deq.pop_back();
  			if (current->child)
  				deq.push_back(current->child);
  			if (current->sibling)
  				deq.push_back(current->sibling);
  			push(current->getElt());
  		}
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) {
    		PairingPQ<TYPE, COMP_FUNCTOR> temp(rhs);

    		Node * nTemp = nullptr;
    		size_t sTemp = -1;

    		nTemp = heap;
    		sTemp = SIZE;

    		heap = temp.heap;
    		SIZE = temp.SIZE;

    		temp.heap = nTemp;
    		temp.SIZE = sTemp;

        return *this;
    } // operator=()

	//void printHeap() {
	//	std::deque<Node *> deq;
	//	Node * current = nullptr;
	//	deq.push_back(heap);
	//	while (!deq.empty()) {
	//		current = deq.back();
	//		deq.pop_back();
	//		if (current->child)
	//			deq.push_back(current->child);
	//		if (current->sibling)
	//			deq.push_back(current->sibling);
	//		std::cout << current << " ";
	//	}
	//}

    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
		std::deque<Node *> deq;
		Node * current = nullptr;
		if (heap)
			deq.push_back(heap);
		heap = nullptr;
		while (!deq.empty()) {
			current = deq.back();
			deq.pop_back();
			if (current->child)
				deq.push_back(current->child);
			if (current->sibling)
				deq.push_back(current->sibling);
			delete current;
			current = nullptr;
		}
    } // ~PairingPQ()


	void switchNodes(Node * n1, Node * n2) {
		TYPE temp = n1->getElt();
		n1->elt = n2->elt;
		n2->elt = temp;
	}

    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
		if (heap == nullptr)
			return;
		std::deque<Node *> deq;
		Node * temp = heap;
		if (heap->child)
			deq.push_back(heap->child);
		if (heap->sibling)
			deq.push_back(heap->sibling);
		heap->child = nullptr;
		heap->sibling = nullptr;
		while (!deq.empty()) {
			temp = deq.back();
			deq.pop_back();
			if (temp->child)
				deq.push_back(temp->child);
			if (temp->sibling)
				deq.push_back(temp->sibling);
			temp->child = nullptr;
			temp->sibling = nullptr;
			heap = meld(temp, heap);
		}
	} // updatePriorities()

    // Description: Add a new element to the priority_queue. This has been provided for you,
    //              in that you should implement push functionality in the
    //              addNode function.
    // Runtime: Amortized O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
		addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
		std::vector<Node*> siblings;
		Node * next = heap->child, *temp = nullptr;
		while(next) {
			if (next->sibling) {
				temp = next->sibling->sibling;
				siblings.push_back(meld(next, next->sibling));
				next = temp;
				temp = nullptr;
			}
			else {
				siblings.push_back(next);
				break;
			}
		}
		for (int i = int(siblings.size()) - 2; i >= 0; --i) {
			siblings[siblings.size() - 1] = meld(siblings[siblings.size() - 1], siblings[i]);
		}
		temp = heap;
		if (siblings.empty()) {
			heap = nullptr;
		}
		else {
			heap = siblings.back();
			heap->parent = nullptr;
			heap->sibling = nullptr;
		}
		delete temp;
		next = nullptr;
		temp = nullptr;
		--SIZE;
	} // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
		return heap->getElt();
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const {
		return SIZE;
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
		return SIZE == 0;
    } // empty()


    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
		if (node == nullptr)
			return;
		if(this->compare(new_value, node->elt))
			return;
		node->elt = new_value;
		if (node == heap)
			return;
		if (this->compare(node->getElt(), (node->parent)->getElt()))
			return;
		Node * prev = node->parent->child;
		if (prev == node) {
			node->parent->child = node->sibling;
		}
		else {
			while (prev->sibling != node) {
				prev->parent = node->parent;
				prev = prev->sibling;
			}
			prev->sibling = node->sibling;
			prev->parent = node->parent;
		}
		heap->sibling = nullptr;
		heap->parent = nullptr;
		node->sibling = nullptr;
		node->parent = nullptr;
		heap = meld(heap, node);
    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: Amortized O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    Node* addNode(const TYPE & val) {
		Node * n = new Node(val);
		n->parent = nullptr;
		n->sibling = nullptr;
		n->child = nullptr;
		if (SIZE == 0)
			heap = n;
		else
			heap = meld(heap, n);
		++SIZE;
        return n;
    } // addNode()


private:
    // TODO: Add any additional member functions or data you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).

	Node* meld(Node * n1, Node * n2) {
		n1->sibling = nullptr;
		n2->sibling = nullptr;
		n1->parent = nullptr;
		n2->parent = nullptr;
		if (this->compare(n1->elt, n2->elt)) {
			n1->parent = n2;
			n1->sibling = n2->child;
			n2->child = n1;
			if(n2->sibling == n1)
				n2->sibling = nullptr;
			return n2;
		}
		else {
			n2->parent = n1;
			n2->sibling = n1->child;
			n1->child = n2;
			if(n1->sibling == n2)
				n1->sibling = nullptr;
			return n1;
		}
	}

	Node * heap = nullptr;

	size_t SIZE = 0;

};


#endif // PAIRINGPQ_H
