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

        void updateHierarchy();

	public:

		Archive();
        Archive(string path);
        ~Archive();

        void addFile(string path);
        void deleteFile(shared_ptr<File> file);

        void deleteFolder(shared_ptr<Folder> folder);

        void write(string path);
        void decompress(string path);

        shared_ptr<Folder> getParentFolder();
};

#endif // ARCHIVE_H
