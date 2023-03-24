// TextCompression301.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma
#include <iostream>
#include <fstream>
#include <string>	
#include <vector>
#include "Node.h"
#include <chrono>
#include <time.h>
#include <streambuf>
#include <unordered_map>
#include <math.h>

using namespace std;
using namespace std::chrono;

vector<Node*> nodes;
string SPACE = "space";
string NEWLINE = "newline";
string TAB = "tab";
string RETURN = "return";

int partition(int p, int r, vector<Node*>& tree) {
	auto x = tree.at(r);
	

	int i = p - 1;

	//loop until pivot -- n-1
	for (int j = p; j < r; j++) {

		if (tree.at(j)->freqs <= x->freqs) {
			i++;
			// exchanging
			swap(tree.at(i), tree.at(j));

		}

	}

	//move pivot to the lowest of greater side.
	swap(tree.at(i + 1), tree.at(r));


	return i + 1;
}

//Implementing sorting algorithm here
void sort(int p, int r, vector<Node*> &tree) {

	//best & average nlgn
	//worst case n^2
	if (p < r) {
		int q = partition(p, r,tree);  //n
		sort(p, q - 1,tree);
		sort(q + 1, r,tree);
	}


}

int main(int argc, char** argv)
{

	/*
	======									======
	======			File Type Check			======		
	======									======
	*/
	if (argc != 2) {
		cout << "Please input plain text file path!" << endl;
		exit(0);
	}
	else { //type check
		string plainText = argv[1];
		int txtLen = plainText.length();
		string txt = ".txt";
		string fileType = plainText.substr(txtLen - 4, 4);

		if (fileType != txt) {
			cout << "Input .txt file!" << endl;
			exit(0);
		}
	}

	/*
	======									======
	======	 Reading File/ Counting Freqs	======
	======									======
*/
	ifstream ifs;
	ifs.open(argv[1], ios::binary);

	//TIME START
	double totalTimeTaken = 0;
	clock_t tStart = clock();

	cout << "Generating frequency . . .";

	int freq[256] = { 0 }; //ASCII Table

	//READING FILE
	string wholetext;
	ifs.seekg(0, ios::end);
	wholetext.reserve(ifs.tellg());
	ifs.seekg(0, ios::beg);

	wholetext.assign((std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>());

	//Counting Freqs
	for (char ch : wholetext) {
		int ascii = ch;
		++freq[ascii];
	}

	ifs.close();
	double duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;

	/*
		======									======
		======	 Generating Huffman Tree		======
		======									======
*/


	cout << "Building Huffman Tree . . .";
	tStart = clock();

	string ch;
	ch.reserve(7);

	// Creating Nodes 
	for (int i = 0; i < 256; i++) {
		if (freq[i] > 0) {

			if (i == 32) {
				ch = SPACE;
			}
			else if (i == 10) ch = NEWLINE;
			else if (i == 13) ch = RETURN;
			else if (i == 9) ch = TAB;
			else ch = char(i);
			//cout << "There are " << freq[i] << " " << ch << endl;
			Node* n = new Node(ch, freq[i]);
			nodes.push_back(n);
		}

	}

	sort(0, nodes.size() - 1,nodes);  //Sort nodes
	
	vector<Node*> tree(nodes);	//Creating Tree
	while (tree.size() > 1) {   
		Node* n1 = tree[0];
		Node* n2 = tree[1];

		int pFreq = n1->freqs + n2->freqs;
		string newLabel = to_string(pFreq);
		Node* p = new Node(newLabel,pFreq);
		p->leftC = n1;
		p->rightC = n2;
		n1->parent = p;
		n2->parent = p;

		tree.erase(tree.begin());
		tree.erase(tree.begin());
		tree.push_back(p);
		sort(0, tree.size() - 1,tree);
	}
	Node* root = tree[0];
	
	duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;

	/*
	======									======
	======	 Encoding the documents 		======
	======									======
*/

	cout << "Encoding the documents . . .";
	tStart = clock();
	
	//Generating prefix code for each node
	for (auto n : nodes) {
		auto x = n;
		while (x != root) {
			if (x->isLeftChild()) { //1011
				n->prefixCode = "0" + n->prefixCode;       // 0
			}
			else if (x->isRightChild()) {
				n->prefixCode = "1" + n->prefixCode;       //001 | 010 -> 011, 0110 | 1000
				
			}

			x = x->parent;
		}
	}


	duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;

	/*
	======									======
	======			Outputing File			======
	======									======
*/


	//output path
	string ofpath = argv[1];
	string ofspath = ofpath.substr(0, ofpath.length() - 4) + ".zip301";
	string shortPath = ofspath.substr(ofspath.find_last_of("/\\") + 1); //base_name(ofspath);


	ofstream ofs(ofspath, ios::out|ios::binary); //write the correct byte! very important


	cout << "Outputing to file . . .";

	tStart = clock();

	//nodes prefixes
	for (auto n : nodes) { 
		ofs << n->toString() << endl;
	}
	ofs << "*****" << endl;

	long totalBits = 0; //Totalbits
	for (auto n : nodes) { 
		totalBits += n->getTotalBits();
	}

	
	ofs << totalBits << endl;


	//Outputing RAW BYTES

	unordered_map<char, Node*> stringToNodesTable; //BUILDING HASH
	for (auto n : nodes) {
		if (n->label == NEWLINE) {
			stringToNodesTable[10] = n;
		}
		else if (n->label == SPACE) {
			stringToNodesTable[32] = n;
		}
		else if (n->label == RETURN) {
			stringToNodesTable[13] = n;
		}
		else if (n->label == TAB) {
			stringToNodesTable[9] = n;
		}
		else
		stringToNodesTable[n -> label[0]] = n;
	}


	int bitCount = 0; //Track of number of bits
	uint8_t output = 0b0;
	string byteString;
	byteString.reserve(totalBits + totalBits % 8);

	for (auto &c : wholetext) {

		Node* n = stringToNodesTable[c];
		if (n != nullptr) {
			for (char &ch : n->prefixCode) {
				output <<= 1;
				if (ch != '0') output |= 1;
				bitCount++;
				//Write out if 1 byte completed, reset
				if (bitCount == 8) {
					byteString += output;
					output = 0b0;
					bitCount = 0;
				}
			}

		}
	}
	//in case not enough bits
	if (bitCount % 8 != 0) {
		output <<= 8 - bitCount; //adds more 0 at the end
		byteString += output;
	}
	
	ofs << byteString;
	

	duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;

	cout << "Wrote out put to file : " << shortPath << endl;
	cout << "Total time : . . . "<<totalTimeTaken << "s" << endl;

	ofs.close();
}
