#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <vector>
#include "centraldirectory.h"
#include "huffman.h"

class Archive {
	public:
		vector<string> files;
		CentralDirectory centralDirectory;

		Archive();

		void addFile(string path);

		void write(ostream &os);
		void decompress(istream &is);
};

#endif // ARCHIVE_H
