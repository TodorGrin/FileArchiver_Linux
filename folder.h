#ifndef FOLDER_H
#define FOLDER_H

#include <vector>
#include <memory>
#include "fileheader.h"
#include "file.h"

using namespace std;

class Folder : public enable_shared_from_this<Folder> {
    private:
        string name;

        shared_ptr<Folder> parentFolder;
        vector<shared_ptr<Folder>> subfolders;
        vector<shared_ptr<File>> files;

        void validateName(string name);

        void addFile(shared_ptr<File> file, string remainingPath);
        void deleteFile(shared_ptr<File> file, string remainingPath);
        void updateFileNames();

    public:
        explicit Folder(string name);
        explicit Folder(shared_ptr<Folder> parentFolder, string name);
        ~Folder();

        void addFile(shared_ptr<File> file);
        void deleteFile(shared_ptr<File> file);
        shared_ptr<Folder> createSubfolder(string name);

        void clear();
        void extract(istream &is, string extractPath);

        void setName(string newName);

        string getPath() const;
        string getName() const;
        vector<shared_ptr<Folder>>& getSubfolders();
        vector<shared_ptr<File>>& getFiles();
        shared_ptr<Folder> getParentFolder() const;
};

#endif // FOLDER_H
