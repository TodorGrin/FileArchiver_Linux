#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <istream>

using namespace std;

class Huffman {
	private:
		struct Tree;

		static int* getCharFrequency(string fileName);
		static Tree* buildTree(int *charFrequency);
		static string* getCharCodes(Tree *tree);
		static void getCharCodesRec(string *codes, Tree *tree, string code = "");
		static bool compareTrees(Tree *a, Tree *b);

	public:
		Huffman();

        static void compress(string inFile, ostream &os);
        static void decompress(istream &is, string outFile);
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
