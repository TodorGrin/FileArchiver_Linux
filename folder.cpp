#include "folder.h"
#include <algorithm>

Folder::Folder(string name) {
    this->parentFolder = nullptr;
    this->name = name;
}

Folder::Folder(shared_ptr<Folder> parentFolder, string name) {
    this->parentFolder = parentFolder;
    this->name = name;
}

void Folder::addFile(FileHeader fileHeader) {
    auto firstSlashPos = fileHeader.name.find_first_of("/");

    if (firstSlashPos == string::npos) {
        this->files.push_back(fileHeader);
    }
    else {
        string subfolderName = fileHeader.name.substr(0, firstSlashPos);

        auto it = find_if(subfolders.begin(), subfolders.end(), [&](const shared_ptr<Folder> &folder) {return folder->getName() == subfolderName;});

        if (it == subfolders.end()) {
            shared_ptr<Folder> folder = make_shared<Folder>(shared_from_this(), subfolderName);

            FileHeader fh(fileHeader);
            fh.name = fh.name.substr(firstSlashPos + 1);

            folder->addFile(fh);

            subfolders.push_back(folder);
        }
        else {
            FileHeader fh(fileHeader);
            fh.name = fh.name.substr(firstSlashPos + 1);

            (*it)->addFile(fh);
        }
    }
}

string Folder::getName() const {
    return name;
}

vector<shared_ptr<Folder>>& Folder::getSubfolders() {
    return subfolders;
}

vector<FileHeader>& Folder::getFiles() {
    return files;
}

shared_ptr<Folder> Folder::getParentFolder() const {
    return parentFolder;
}
