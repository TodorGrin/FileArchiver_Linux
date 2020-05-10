#ifndef FOLDER_H
#define FOLDER_H

#include <vector>
#include <memory>
#include "fileheader.h"

class Folder : public enable_shared_from_this<Folder> {
    private:
        string name;

        shared_ptr<Folder> parentFolder;
        vector<shared_ptr<Folder>> subfolders;
        vector<FileHeader> files;

    public:
        Folder(string name);
        Folder(shared_ptr<Folder> parentFolder, string name);
        ~Folder();

        void addFile(FileHeader fileHeader);
        void clear();
        void extract(istream &is, string extractPath);

        string getName() const;
        vector<shared_ptr<Folder>>& getSubfolders();
        vector<FileHeader>& getFiles();
        shared_ptr<Folder> getParentFolder() const;
};

#endif // FOLDER_H
