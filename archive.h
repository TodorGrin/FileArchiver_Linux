#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <vector>
#include "centraldirectory.h"
#include "huffman.h"
#include "folder.h"

class Archive {
    private:
        CentralDirectory centralDirectory;
        shared_ptr<Folder> parentFolder;
        string path;

	public:

		Archive();
        Archive(string path);

		void addFile(string path);

        void write(string path);
        void decompress(string path);

        shared_ptr<Folder> getParentFolder();
};

#endif // ARCHIVE_H
