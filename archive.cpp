#include "archive.h"
#include <fstream>

Archive::Archive() {
    parentFolder = make_shared<Folder>("");
}

Archive::Archive(string path) : Archive() {
    this->path = path;

    ifstream is(path, ios::binary);
    centralDirectory.read(is);

    for (FileHeader &fh : centralDirectory.files)
        parentFolder->addFile(fh);
}

void Archive::addFile(string path) {
    FileHeader fileHeader(path);
    fileHeader.readStatus();
	fileHeader.offset = 0;

	centralDirectory.files.push_back(move(fileHeader));
}

void Archive::write(string path) {
    ofstream os(path, ios::binary);

    if (!os.is_open())
        throw std::runtime_error("Failed to create " + path);

    for (FileHeader &fh : centralDirectory.files) {
        fh.offset = os.tellp();

        Huffman::compress(fh.name, os);

        fh.compressedSize = os.tellp();
        fh.compressedSize -= fh.offset;
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

shared_ptr<Folder> Archive::getParentFolder() {
    return parentFolder;
}
