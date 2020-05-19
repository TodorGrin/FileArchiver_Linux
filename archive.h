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
        void compressFile(shared_ptr<File> file, string filePathOnDisk, ostream &os);
        void truncateArchive(int size);

	public:

		Archive();
        Archive(string path);
        ~Archive();

        void addFile(shared_ptr<Folder> folder, string filePath);
        void deleteFile(shared_ptr<File> file);
        void renameFile(shared_ptr<File> file, string newName);
        void extractFiles(vector<shared_ptr<File>> files, string path);

        void deleteFolder(shared_ptr<Folder> folder);
        void renameFolder(shared_ptr<Folder> folder, string newName);
        void extractFolders(vector<shared_ptr<Folder>> folders, string path);

        void write(string path);
        void extract(string path);

        shared_ptr<Folder> getParentFolder();
};

#endif // ARCHIVE_H
