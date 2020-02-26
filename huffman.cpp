#include "huffman.h"
#include <stdio.h>
#include <iostream>
#include <deque>
#include <algorithm>

Huffman::Huffman() {

}

int* Huffman::getCharFrequency(string fileName) {
	FILE *file;
	int *frequency = new int[256]();
	int c;

	fopen_s(&file, fileName.c_str(), "rb");

	while ((c = fgetc(file)) != EOF)
		++frequency[c];

	return frequency;
}

bool Huffman::compareTrees(Tree *a, Tree *b) {
	return a->frequency < b->frequency;
}

Huffman::Tree* Huffman::buildTree(int *charFrequency) {
	std::deque<Tree*> pq;

	for (int c = 0; c < 256; ++c) {
		if (charFrequency[c] > 0)
			pq.push_back(new Tree(c, charFrequency[c]));
	}

	while (pq.size() > 1) {
		stable_sort(pq.begin(), pq.end(), Huffman::compareTrees);

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

void Huffman::compress(string inFile, string outFile) {
	int *f = getCharFrequency(inFile);
	Tree *t = buildTree(f);
	string *codes = getCharCodes(t);

	FILE *inF, *outF;
	fopen_s(&inF, inFile.c_str(), "rb");
	fopen_s(&outF, outFile.c_str(), "wb");
	fseek(outF, sizeof(int), SEEK_SET);

	int bytes = 0, inC = 0, outC = 0;
	int size = 0;

	for (int c = 0; c < 256; ++c)
		fwrite((char*) &f[c], 1, 4, outF);

	while ((inC = fgetc(inF)) != EOF) {
		++size;

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

	if (bytes > 0)
		fputc(outC, outF);

	fseek(outF, 0, SEEK_SET);
	fwrite((char*) &size, 1, 4, outF);

	fclose(inF);
	fclose(outF);
}

void Huffman::decompress(string inFile, string outFile) {
	FILE *inF, *outF;
	fopen_s(&inF, inFile.c_str(), "rb");
	fopen_s(&outF, outFile.c_str(), "wb");

	int size, inC = 0, charsWritten = 0;
	int charFrequency[256];

	fread((char*) &size, 1, 4, inF);

	for (int c = 0; c < 256; ++c)
		fread((char*) &charFrequency[c], 1, 4, inF);

	Tree *tree = buildTree(charFrequency), *cur = tree;

	while ((inC = fgetc(inF)) != EOF) {
		for (int i = 0; i < 8; ++i) {
			if (inC & (1 << i))
				cur = cur->right;
			else
				cur = cur->left;

			if (!cur->left && !cur->right) {
				if (charsWritten >= size)
					break;

				++charsWritten;
				fputc(cur->c, outF);
				cur = tree;
			}
		}
	}

	fclose(inF);
	fclose(outF);
}
