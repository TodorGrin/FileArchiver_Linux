#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>

using namespace std;

class Huffman {
	private:
		struct Tree;

		static int* getCharFrequency(const char* fileName);
		static Tree* buildTree(int *charFrequency);
		static string* getCharCodes(Tree *tree);
		static void getCharCodesRec(string *codes, Tree *tree, string code = "");
		static bool compareTrees(Tree *a, Tree *b);

	public:
		Huffman();

		void compress(const char* inFile, const char* outFile);
};



struct Huffman::Tree {
	Tree *left = nullptr, *right = nullptr;
	int c, frequency;

	Tree() = default;

	Tree(int c, int f) {
		this->c = c;
		frequency = f;
	}

	Tree(Tree *l, Tree *r, int f) {
		left = l;
		right = r;
		frequency = f;
	}
};

#endif // HUFFMAN_H
