#include "huffman.h"
#include <stdio.h>
#include <iostream>
#include <deque>
#include <algorithm>

Huffman::Huffman() {

}

int* Huffman::getCharFrequency(string fileName) {
    FILE *file = fopen(fileName.c_str(), "rb");
	int *frequency = new int[256]();
	int c;

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

void Huffman::compress(string inFile, ostream &os) {
	int *f = getCharFrequency(inFile);
	Tree *t = buildTree(f);
	string *codes = getCharCodes(t);

    FILE *inF = fopen(inFile.c_str(), "rb");

	int begin = os.tellp();
	os.seekp(sizeof(int), ios::cur);

	int bytes = 0, inC = 0, outC = 0;
	int size = 0;

	for (int c = 0; c < 256; ++c)
		os.write((char*) &f[c], sizeof(int));

    while ((inC = fgetc(inF)) != EOF) {
        ++size;

        for (int i = 0; i < codes[inC].size(); ++i) {
            outC = outC | ((codes[inC][i] - '0') << bytes);
            ++bytes;

            if (bytes == 8) {
                os.put(outC);
                outC = 0;
                bytes = 0;
            }
        }
    }

	if (bytes > 0)
		os.put(outC);

	os.seekp(begin);
	os.write((char*) &size, sizeof(int));
	os.seekp(0, ios::end);

	fclose(inF);
}

void Huffman::decompress(istream &is, string outFile) {
    FILE *outF = fopen(outFile.c_str(), "wb");

	int size, inC = 0, charsWritten = 0;
	int charFrequency[256];

	is.read((char*) &size, sizeof(size));

	for (int c = 0; c < 256; ++c)
		is.read((char*) &charFrequency[c], sizeof(charFrequency[c]));

	Tree *tree = buildTree(charFrequency), *cur = tree;

	while ((inC = is.get()) != EOF && charsWritten < size) {
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

	fclose(outF);
}
