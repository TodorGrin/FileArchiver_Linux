#include "folder.h"
#include "huffman.h"
#include <sys/stat.h>
#include <algorithm>

Folder::Folder(string name) {
    this->parentFolder = nullptr;
    this->name = name;
}

Folder::Folder(shared_ptr<Folder> parentFolder, string name) {
    this->parentFolder = parentFolder;
    this->name = name;
}

Folder::~Folder() {
    cout << name << " deleted" << endl;
}

void Folder::clear() {
    parentFolder.reset();

    for (auto folder : subfolders)
        folder->clear();

    subfolders.clear();
    files.clear();
}

void Folder::addFile(shared_ptr<File> file, string remainingPath) {
    auto firstSlashPos = remainingPath.find_first_of("/");

    if (firstSlashPos == string::npos) {
        file->setParentFolder(shared_from_this());
        this->files.push_back(file);
    }
    else {
        string subfolderName = remainingPath.substr(0, firstSlashPos);

        auto it = find_if(subfolders.begin(), subfolders.end(), [&](const shared_ptr<Folder> &folder) {return folder->getName() == subfolderName;});

        if (it == subfolders.end()) {
            shared_ptr<Folder> folder = make_shared<Folder>(shared_from_this(), subfolderName);

            folder->addFile(file, remainingPath.substr(firstSlashPos + 1));
            subfolders.push_back(folder);
        }
        else {
            (*it)->addFile(file, remainingPath.substr(firstSlashPos + 1));
        }
    }
}

void Folder::deleteFile(shared_ptr<File> file, string remainingPath) {
    auto firstSlashPos = remainingPath.find_first_of("/");

    if (firstSlashPos == string::npos) {
        auto it = find_if(files.begin(), files.end(), [&](const shared_ptr<File> &f){return f == file;});
        files.erase(it);
    }
    else {
        string subfolderName = remainingPath.substr(0, firstSlashPos);

        auto it = find_if(subfolders.begin(), subfolders.end(), [&](const shared_ptr<Folder> &folder) {return folder->getName() == subfolderName;});

        if (it != subfolders.end()) {
            (*it)->deleteFile(file, remainingPath.substr(firstSlashPos + 1));
        }
    }
}

void Folder::deleteFile(shared_ptr<File> file) {
    deleteFile(file, file->getHeader().name);
}

void Folder::addFile(shared_ptr<File> file) {
    addFile(file, file->getHeader().name);
}

string Folder::getName() const {
    return name;
}

void Folder::extract(istream &is, string extractPath) {
    mkdir(extractPath.c_str(), 0700);

    for (shared_ptr<File> file : files) {
        is.seekg(file->getHeader().offset, ios::beg);
        Huffman::decompress(is, extractPath + "/" + file->getName());
    }

    for (shared_ptr<Folder> &folder : subfolders)
        folder->extract(is, extractPath + "/" + folder->getName());
}

vector<shared_ptr<Folder>>& Folder::getSubfolders() {
    return subfolders;
}

vector<shared_ptr<File>>& Folder::getFiles() {
    return files;
}

shared_ptr<Folder> Folder::getParentFolder() const {
    return parentFolder;
}
