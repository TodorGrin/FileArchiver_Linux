#include "file.h"
#include "folder.h"

File::File(shared_ptr<Folder> parentFolder, FileHeader header) {
    this->parentFolder = parentFolder;
    this->header = header;

    auto lastSlashPos = header.name.find_last_of("/");

    if (lastSlashPos == string::npos)
        name = header.name;
    else
        name = header.name.substr(lastSlashPos + 1);
}

void File::setParentFolder(shared_ptr<Folder> parentFolder) {
    this->parentFolder = parentFolder;
}

string& File::getName() {
    return name;
}

shared_ptr<Folder> File::getParentFolder() {
    return parentFolder;
}

FileHeader& File::getHeader() {
    return header;
}
