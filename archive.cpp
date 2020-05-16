#include "archive.h"
#include <fstream>
#include <QErrorMessage>
#include <stdio.h>
#include <unistd.h>

Archive::Archive() {
    parentFolder = make_shared<Folder>("");
}

Archive::Archive(string path) : Archive() {
    this->path = path;

    ifstream is(path, ios::binary);
    centralDirectory.read(is);

    updateHierarchy();
}

Archive::~Archive() {
    parentFolder->clear();
}

void Archive::updateHierarchy() {
    parentFolder->clear();

    for (auto &f : centralDirectory.files)
        parentFolder->addFile(f);
}

void Archive::addFile(string path) {
    FileHeader fileHeader(path);
    fileHeader.readStatus();
	fileHeader.offset = 0;

    centralDirectory.files.push_back(make_shared<File>(nullptr, fileHeader));
    //TODO
}

void Archive::write(string path) {
    ofstream os(path, ios::binary);

    if (!os.is_open())
        throw std::runtime_error("Failed to create " + path);

    for (auto &f : centralDirectory.files) {
        f->getHeader().offset = os.tellp();

        Huffman::compress(f->getHeader().name, os);

        f->getHeader().compressedSize = os.tellp();
        f->getHeader().compressedSize -= f->getHeader().offset;
	}

	centralDirectory.write(os);
    os.close();
}

void Archive::decompress(string path) {
    ifstream is(this->path, ios::binary);

    if (!is.is_open())
        throw std::runtime_error("Failed to open " + this->path);

    parentFolder->extract(is, path);

    is.close();
}

void Archive::deleteFile(shared_ptr<File> file) {
    int shiftStart = centralDirectory.offset, shiftEnd = centralDirectory.offset;

    for (auto &f : centralDirectory.files) {
        if (f->getHeader().offset > file->getHeader().offset && f->getHeader().offset < shiftStart)
            shiftStart = f->getHeader().offset;
    }

    fstream fs(this->path, ios::in | ios::out | ios::binary);

    if (!fs.is_open()) {
        QErrorMessage err;
        err.setWindowTitle("Error");
        err.showMessage(QString::fromStdString("Can't open " + this->path));
        err.exec();
    }

    int writePos = file->getHeader().offset;

    while (shiftStart < shiftEnd) {
        char c;

        fs.seekg(shiftStart);
        fs.get(c);

        fs.seekp(writePos);
        fs.put(c);

        shiftStart++;
        writePos++;
    }

    parentFolder->deleteFile(file);
    centralDirectory.deleteFile(file);
    centralDirectory.write(fs);
    int fileSize = fs.tellp();
    fs.close();

    FILE *f = fopen(this->path.c_str(), "a");
    ftruncate(fileno(f), fileSize);
    fclose(f);
}

void Archive::deleteFolder(shared_ptr<Folder> folder) {
    if (folder->getParentFolder() == nullptr)
        return;

    while (!folder->getSubfolders().empty())
        deleteFolder(folder->getSubfolders().front());

    while (!folder->getFiles().empty()) {
        cout << folder->getFiles().front()->getName() << endl;
        deleteFile(folder->getFiles().front());
    }

    shared_ptr<Folder> parentFolder = folder->getParentFolder();
    parentFolder->getSubfolders().erase(find(parentFolder->getSubfolders().begin(), parentFolder->getSubfolders().end(), folder));
    folder->clear();
}

shared_ptr<Folder> Archive::getParentFolder() {
    return parentFolder;
}
