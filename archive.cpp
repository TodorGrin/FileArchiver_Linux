#include "archive.h"
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

Archive::Archive() {
    parentFolder = make_shared<Folder>("");
}

Archive::Archive(string path) : Archive() {
    this->path = path;

    ifstream is(path, ios::binary);
    if (!is.is_open())
        throw std::runtime_error("Can't open \"" + path + "\"");

    centralDirectory.read(is);

    updateHierarchy();
}

Archive::~Archive() {
    parentFolder->clear();
}

shared_ptr<Archive> Archive::create(string path) {
    shared_ptr<Archive> archive = make_shared<Archive>();
    archive->path = path;

    ofstream os(path, ios::binary);

    if (!os.is_open())
        throw std::runtime_error("Failed to create \"" + path + "\"");

    archive->centralDirectory.write(os);
    os.close();

    return archive;
}

void Archive::updateHierarchy() {
    parentFolder->clear();

    for (auto &f : centralDirectory.getFiles())
        parentFolder->addFile(f);
}

void Archive::truncateArchive(int size) {
    FILE *f = fopen(this->path.c_str(), "a");
    ftruncate(fileno(f), size);
    fclose(f);
}

void Archive::addFile(shared_ptr<Folder> folder, string filePath) {
    auto lastSlashPos = filePath.find_last_of("/");
    string fileName;

    if (lastSlashPos == string::npos)
        fileName = filePath;
    else
        fileName = filePath.substr(lastSlashPos + 1);

    for (auto file : folder->getFiles())
        if (file->getName() == filePath.substr(lastSlashPos + 1))
            throw runtime_error("File \"" + fileName + "\" already exists in folder");

    string pathInArchive = folder->getPath() + "/" + fileName;

    FileHeader fileHeader(pathInArchive);
    fileHeader.readMetadata(filePath);
    fileHeader.offset = centralDirectory.getOffset();

    fstream os(path, ios::out | ios::in | ios::binary);
    if (!os.is_open())
        throw runtime_error("Can't open file \"" + path + "\"");

    shared_ptr<File> file = make_shared<File>(nullptr, fileHeader);

    os.seekp(centralDirectory.getOffset());
    compressFile(file, filePath, os);

    centralDirectory.getFiles().push_back(file);
    parentFolder->addFile(file);

    centralDirectory.write(os);
    os.close();
}

void Archive::addFolder(shared_ptr<Folder> folder, string folderPath) {
    DIR *d;
    struct dirent *dir;

    d = opendir(folderPath.c_str());
    if (!d) throw runtime_error("Can't open folder \"" + folderPath + "\"");

    string folderName = (folderPath.find_last_of("/") == string::npos) ? folderPath : folderPath.substr(folderPath.find_last_of("/") + 1);
    shared_ptr<Folder> newFolder = folder->createSubfolder(folderName);

    while ((dir = readdir(d))) {
        struct stat st;
        string direntPath = folderPath + "/" + dir->d_name;

        if (stat(direntPath.c_str(), &st) != 0)
            throw runtime_error("Can't read stats of \"" + direntPath + "\": " + strerror(errno));

        if (S_ISREG(st.st_mode))
            Archive::addFile(newFolder, direntPath);
        else if (S_ISDIR(st.st_mode)) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
                Archive::addFolder(newFolder, direntPath);
        }
        else
            throw runtime_error("Unknown file type: \"" + direntPath + "\"");
    }

    closedir(d);
}

void Archive::write(string path) {
    ofstream os(path, ios::binary);

    if (!os.is_open())
        throw std::runtime_error("Failed to create \"" + path + "\"");

    for (auto &f : centralDirectory.getFiles()) {
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

void Archive::extract(string path) {
    ifstream is(this->path, ios::binary);

    if (!is.is_open())
        throw std::runtime_error("Failed to open " + this->path);

    parentFolder->extract(is, path);

    is.close();
}

void Archive::extractFiles(vector<shared_ptr<File>> files, string path) {
    ifstream is(this->path, ios::binary);

    if (!is.is_open())
        throw std::runtime_error("Failed to open " + this->path);

    for (auto file : files)
        file->extract(is, path);

    is.close();
}

void Archive::extractFolders(vector<shared_ptr<Folder> > folders, string path) {
    ifstream is(this->path, ios::binary);

    if (!is.is_open())
        throw std::runtime_error("Failed to open " + this->path);

    for (auto folder : folders)
        folder->extract(is, path + "/" + folder->getName());

    is.close();
}

void Archive::deleteFile(shared_ptr<File> file) {
    int shiftStart = centralDirectory.getOffset(), shiftEnd = centralDirectory.getOffset();

    for (auto &f : centralDirectory.getFiles()) {
        if (f->getHeader().offset > file->getHeader().offset && f->getHeader().offset < shiftStart)
            shiftStart = f->getHeader().offset;
    }

    fstream fs(this->path, ios::in | ios::out | ios::binary);
    if (!fs.is_open())
        throw runtime_error("Can't open \"" + this->path + "\"");

    int writePos = file->getHeader().offset;
    fs.seekp(writePos);

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

    truncateArchive(fileSize);
}

void Archive::renameFile(shared_ptr<File> file, string newName) {
    file->setName(newName);

    fstream fs(this->path, ios::in | ios::out | ios::binary);
    if (!fs.is_open())
        throw runtime_error("Can't open \"" + this->path + "\"");

    fs.seekp(centralDirectory.getOffset());
    centralDirectory.write(fs);
    int fileSize = fs.tellp();
    fs.close();

    truncateArchive(fileSize);
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

void Archive::renameFolder(shared_ptr<Folder> folder, string newName) {
    folder->setName(newName);

    fstream fs(this->path, ios::in | ios::out | ios::binary);
    if (!fs.is_open())
        throw runtime_error("Can't open \"" + this->path + "\"");

    fs.seekp(centralDirectory.getOffset());
    centralDirectory.write(fs);
    int fileSize = fs.tellp();
    fs.close();

    truncateArchive(fileSize);
}


shared_ptr<Folder> Archive::getParentFolder() {
    return parentFolder;
}
