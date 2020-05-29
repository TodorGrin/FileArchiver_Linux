#include <regex>
#include "file.h"
#include "folder.h"
#include "huffman.h"

File::File(shared_ptr<Folder> parentFolder, FileHeader header) {
    this->parentFolder = parentFolder;
    this->header = header;

    auto lastSlashPos = header.name.find_last_of("/");

    if (lastSlashPos == string::npos)
        name = header.name;
    else
        name = header.name.substr(lastSlashPos + 1);
}

void File::extract(istream &is, string extractPath) {
    is.seekg(header.offset, ios::beg);
    Huffman::decompress(is, extractPath + "/" + name);
    header.setMetadata(extractPath + "/" + name);
}

void File::setParentFolder(shared_ptr<Folder> parentFolder) {
    this->parentFolder = parentFolder;
}

void File::setName(string name) {
    regex e("[\\/<>\\|:&]");

    if (regex_search(name, e))
        throw runtime_error("File name can't contain symbols /<>|:&");

    auto lastSlashPos = header.name.find_last_of("/");

    if (lastSlashPos == string::npos)
        lastSlashPos = 0;

    header.name = header.name.substr(0, lastSlashPos) + "/" + name;
    this->name = name;
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
