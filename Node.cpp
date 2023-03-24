#include "Node.h"
#include <string>
using namespace std;
Node::Node(string label) {
	this->label = label;
	leftC = nullptr;
	rightC = nullptr;
	parent = nullptr;
	prefixCode.reserve(32);
	freqs = 0;
}

Node::Node(string label,int freqs) {
	this->label = label;
	leftC = nullptr;
	rightC = nullptr;
	parent = nullptr;
	prefixCode.reserve(32);
	this->freqs = freqs;

}

bool Node::isLeftChild() {
	return (parent->leftC == this);
}
bool Node::isRightChild() {
	return (parent->rightC == this);
}
string Node::toString() {
	return prefixCode + " " + label;
}

int Node::getTotalBits() {
	return freqs * prefixCode.length();
}