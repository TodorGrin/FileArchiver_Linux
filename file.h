#ifndef FILE_H
#define FILE_H

#include <memory>
#include "fileheader.h"

class Folder;

class File : public enable_shared_from_this<File>{
    private:
        shared_ptr<Folder> parentFolder;
        FileHeader header;

        string name;

    public:
        explicit File(shared_ptr<Folder> parentFolder, FileHeader header);

        string& getName();
        shared_ptr<Folder> getParentFolder();
        FileHeader& getHeader();

        void setParentFolder(shared_ptr<Folder> parentFolder);
};

#endif // FILE_H
