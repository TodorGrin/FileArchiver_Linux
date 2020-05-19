#include "folder.h"
#include "huffman.h"
#include <sys/stat.h>
#include <algorithm>
#include <regex>

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

void Folder::updateFileNames() {
    string path = getPath();

    for (auto file : files)
        file->getHeader().name = path + "/" + file->getName();

    for (auto folder : subfolders)
        folder->updateFileNames();
}

void Folder::setName(string newName) {
    regex e("[\\/<>\\|:&]");

    if (regex_search(name, e))
        throw runtime_error("Folder name can't contain symbols /<>|:&");

    this->name = newName;
    updateFileNames();
}

void Folder::addFile(shared_ptr<File> file, string remainingPath) {
    if (remainingPath[0] == '/')
        remainingPath = remainingPath.substr(1);

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
    if (remainingPath[0] == '/')
        remainingPath = remainingPath.substr(1);

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

void Folder::extract(istream &is, string extractPath) {
    mkdir(extractPath.c_str(), 0700);

    for (shared_ptr<File> file : files)
        file->extract(is, extractPath);

    for (shared_ptr<Folder> &folder : subfolders)
        folder->extract(is, extractPath + "/" + folder->getName());
}

string Folder::getName() const {
    return name;
}

string Folder::getPath() const {
    if (parentFolder == nullptr)
        return "";
    else
        return parentFolder->getPath() + "/" + name;
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
