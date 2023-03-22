#pragma once
#include <string>
using namespace std;
class Node
{
public:
	Node* leftC;
	Node* rightC;
	Node* parent;
	string label;
	int freqs;
	Node(string label);
	Node(string label,int freqs);
	bool isLeftChild();
	bool isRightChild();
	string prefixCode;
	string toString();
	int getTotalBits();
};

