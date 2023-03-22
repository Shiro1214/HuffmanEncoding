// TextCompression301.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma
#include <iostream>
#include <fstream>
#include <string>	
#include <bits.h>
#include <bitset>
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
	
	if (argc != 2) {
		cout << "Please input plain text file path!" << endl;
		exit(0);
	}
	else { //type check
		string plainText = argv[1];
		int txtLen = plainText.length();
		string txt = ".txt"; 
		string fileType = plainText.substr(txtLen-4,4);

		if (fileType != txt) {
			cout << "Input .txt file!" << endl;
			exit(0);
		}
	}


	ifstream ifs;
	ifs.open(argv[1], ios::binary);

	double totalTimeTaken = 0;
	clock_t tStart = clock();
	cout << "Generating frequency . . .";
	int freq[256] = { 0 };

	string wholetext;
	ifs.seekg(0, ios::end);
	wholetext.reserve(ifs.tellg());
	ifs.seekg(0, ios::beg);

	wholetext.assign((std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>());
	
	/*file.seekg(0, file.end);
string fileContents(file.tellg(), 0);
file.seekg(0);
file.read(fileContents.data(), fileContents.size());*/


	for (char ch : wholetext) {
		int ascii = ch;
		++freq[ascii];

	}


	ifs.close();

	for (int i = 0; i < 256; i++) {
		if (freq[i] > 0) {
			string ch;
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

	double duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;
	//cout << "reading freqs done" << endl;


	cout << "Building Huffman Tree . . .";
	tStart = clock();

	sort(0, nodes.size() - 1,nodes);
	vector<Node*> tree(nodes);

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
	//cout << "tree done" << endl;
	Node* root = tree[0];
	
	duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;


	cout << "Encoding the documents . . .";
	tStart = clock();
	for (auto n : nodes) {
		auto x = n;
		while (x != root) {
			if (x->isLeftChild()) { //1011
				n->prefixCode = "0" + n->prefixCode;       // 0
				//output <<= 1;
			}
			else if (x->isRightChild()) {
				n->prefixCode = "1" + n->prefixCode;       //001 | 010 -> 011, 0110 | 1000
				
				//output |= (1 << bitCount);
			}
			//if (bitCount == 8) {
			//	ofs << output;
			//	output = 0;
			//	bitCount = 0;
			//}
			//bitCount++;
			x = x->parent;
		}
	}


	duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;



	//output path
	string ofpath = argv[1];
	string ofspath = ofpath.substr(0, ofpath.length() - 4) + ".zip301";
	string shortPath = ofspath.substr(ofspath.find_last_of("/\\") + 1); //base_name(ofspath);


	ofstream ofs(ofspath, ios::binary); //write the correct byte! very important


	cout << "Outputing to file . . .";

	tStart = clock();

	

	for (auto n : nodes) { //nodes
		ofs << n->toString() << endl;
	}
	ofs << "*****" << endl;

	long totalBits = 0;
	for (auto n : nodes) { //nodes
		totalBits += n->getTotalBits();
	}

	
	ofs << totalBits << endl;
	
	cout << "done before raw bytes ..." << endl;

	unordered_map<string, Node*> stringToNodesTable;
	for (auto n : nodes) {
		stringToNodesTable[n -> label] = n;
		/*
		stringToNodesTable.insert({ n->label, n });*/
	}
	//cout << nodes.size() << endl;
	/*string encoding;
	encoding.reserve(totalBits + totalBits%8);*/

	cout << "wholetext len : " << wholetext.length() << endl;

	uint8_t output = 0; 
	int bitCount = 0;
	for (auto &c : wholetext) {
		string label = "";
		if (c == 32) label = SPACE;
		else if (c == 10) label = NEWLINE;
		else if (c == 13) label = RETURN;
		else if (c == 9) label = TAB;
		else label += c;

		Node* n = stringToNodesTable[label];
		if (n != nullptr) {
			for (char &ch : n->prefixCode) {
				output <<= 1;
				if (ch != '0') output |= 1;
				bitCount++;
				//Write out if 1 byte completed, reset
				if (bitCount == 8) {
					ofs << output;
					output = 0;
					bitCount = 0;
				}
			}

		}
	}
	//in case not enough bits
	if (bitCount % 8 != 0) {
		output *= pow(2,8 - bitCount);
		ofs << output;
	}
	
	ofs.close();

	duration = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	totalTimeTaken += duration;
	cout << (" %.3fs\n", duration) << "s" << endl;

	cout << "Wrote out put to file : " << shortPath << endl;
	cout << "Total time : . . . "<<totalTimeTaken << "s" << endl;
}




//Incase amount of bits not divisible by 8
//if (totalBits % 8 != 0) {
//	for (int i = 0; i < totalBits % 8; i++) {
//		encoding += '0';
//		//encoding.insert(e, 1, '0');
//		//e++;
//	}

//}

//for (char ch : encoding) {
//	output <<= 1;
//	if (ch != '0') output |= 1;
//	bitCount++;
//	if (bitCount == 8) {
//		ofs << output;
//		output = 0;
//		bitCount = 0;
//	}
//}



/*
	//Incase amount of bits not divisible by 8
	if (totalBits % 8 != 0) {
		for (int i = 0; i < totalBits % 8; i++) {
			encoding += '0';
			//encoding.insert(e, 1, '0');
			//e++;
		}

	}
	cout << "encoding is done ... " << endl;
	bitset<totalBits + totalBits % 8> testBits(encoding);
	//output binary string
	int num_bytes = (totalBits + 7) / 8; // calculate number of bytes
	//cout << num_bytes << endl;
	for (int i = 0; i < num_bytes; i++) {
		string byte_str = encoding.substr(i * 8, 8); // extract next 8 bits
		//cout << &byte_str << " : byteStr mem" << endl;
		bitset<8> bits(byte_str); //sizeofbits fixed set to 4bytes (1-32bits)
		//for (int j = 0; j < 8; j++) {
		//	bits[j] = encoding[(i * 8) + j];
		//}
		//if (i == num_bytes - 1)
			//cout << "This is the last bit string which shouldnt have been complete: " << byte_str << endl;
		ofs.write(reinterpret_cast<const char*>(&bits), 1); // Write bitset to file
	}
	*/