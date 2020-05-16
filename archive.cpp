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

void Archive::addFile(shared_ptr<Folder> folder, string filePath) {
    auto lastSlashPos = filePath.find_last_of("/");
    string pathInArchive = folder->getPath() + "/";

    if (lastSlashPos == string::npos)
        pathInArchive += filePath;
    else
        pathInArchive += filePath.substr(lastSlashPos + 1);

    FileHeader fileHeader(pathInArchive);
    fileHeader.readStatus(filePath);
    fileHeader.offset = centralDirectory.offset;

    fstream os(path, ios::out | ios::in | ios::binary);
    if (!os.is_open())
        throw runtime_error("Can't open file" + path);

    shared_ptr<File> file = make_shared<File>(nullptr, fileHeader);

    os.seekp(centralDirectory.offset);
    compressFile(file, filePath, os);

    centralDirectory.files.push_back(file);
    parentFolder->addFile(file);

    centralDirectory.write(os);
    os.close();
}

void Archive::write(string path) {
    ofstream os(path, ios::binary);

    if (!os.is_open())
        throw std::runtime_error("Failed to create " + path);

    for (auto &f : centralDirectory.files) {
        compressFile(f, f->getHeader().name, os);
	}

	centralDirectory.write(os);
    os.close();
}

void Archive::compressFile(shared_ptr<File> file, string filePathOnDisk, ostream &os) {
    file->getHeader().offset = os.tellp();

    Huffman::compress(filePathOnDisk, os);

    file->getHeader().compressedSize = (int) os.tellp() - file->getHeader().offset;
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
