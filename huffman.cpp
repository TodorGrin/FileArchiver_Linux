#include "huffman.h"
#include <stdio.h>
#include <iostream>
#include <deque>
#include <algorithm>

Huffman::Huffman() {

}

int* Huffman::getCharFrequency(const char* fileName) {
	FILE *file;
	int *frequency = new int[256]();
	int c;

	fopen_s(&file, fileName, "rb");

	while ((c = fgetc(file)) != EOF)
		++frequency[c];

	return frequency;
}

bool Huffman::compareTrees(Tree *a, Tree *b) {
	return a->frequency < b->frequency;
}

Huffman::Tree* Huffman::buildTree(int *charFrequency) {
	std::deque<Tree*> pq(256, nullptr);

	for (int c = 0; c < 256; ++c) {
		pq[c] = new Tree(c, charFrequency[c]);
	}

	sort(pq.begin(), pq.end(), Huffman::compareTrees);

	while (pq.size() > 1) {
		Tree *n1, *n2;
		n1 = pq.front();
		pq.pop_front();
		n2 = pq.front();

		Tree *tree = new Tree(n1, n2, n1->frequency + n2->frequency);
		pq.front() = tree;
	}

	return pq[0];
}

void Huffman::getCharCodesRec(string *codes, Tree *tree, string code) {
	if (!tree->left && !tree->right) {
		codes[tree->c] = code;
	}
	else {
		getCharCodesRec(codes, tree->left, code + "0");
		getCharCodesRec(codes, tree->right, code + "1");
	}
}

string* Huffman::getCharCodes(Tree* tree) {
	string *codes = new string[256];

	getCharCodesRec(codes, tree);

	return codes;
}

void Huffman::compress(const char* inFile, const char* outFile) {
	int *f = getCharFrequency(inFile);
	Tree *t = buildTree(f);
	string *codes = getCharCodes(t);

	FILE *inF, *outF;
	fopen_s(&inF, inFile, "rb");
	fopen_s(&outF, outFile, "wb");

	int bytes = 0, inC = 0, outC = 0;

	while ((inC = fgetc(inF)) != EOF) {
		for (int i = 0; i < codes[inC].size(); ++i) {
			outC = outC | ((codes[inC][i] - '0') << bytes);
			++bytes;

			if (bytes == 8) {
				fputc(outC, outF);
				outC = 0;
				bytes = 0;
			}
		}
	}
}
