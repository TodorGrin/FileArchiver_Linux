#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <vector>
#include "centraldirectory.h"
#include "huffman.h"

class Archive {
	public:
		CentralDirectory centralDirectory;

		Archive();
        Archive(string path);

		void addFile(string path);

		void write(ostream &os);
		void decompress(istream &is);
};

#endif // ARCHIVE_H
