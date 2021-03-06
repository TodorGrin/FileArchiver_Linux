#include "centraldirectory.h"

CentralDirectory::CentralDirectory() {

}

void CentralDirectory::write(ostream &os) {
	int beginOffset = os.tellp(), endOffset;
    offset = beginOffset;

    os.write((char*) &MAGIC_NUMBER, sizeof(MAGIC_NUMBER));

	for (int i = 0; i < files.size(); ++i) {
        files[i]->getHeader().write(os);
	}

	endOffset = os.tellp();
	int offsetFromEnd = endOffset - beginOffset + sizeof(offsetFromEnd);

	os.write((char*) &offsetFromEnd, sizeof(offsetFromEnd));
}

void CentralDirectory::read(istream &is)  {
    int startOffset = 0, endOffset = 0;

	is.seekg(-4, ios::end);
    endOffset = is.tellg();
    is.read((char*) &startOffset, sizeof(startOffset));

    if (startOffset < 0 || startOffset > endOffset)
        throw runtime_error("This is not valid archive file");

    is.seekg(-startOffset, ios::end);
    offset = is.tellg();

    long long magicNumber;
    is.read((char*) &magicNumber, sizeof(magicNumber));

    if (magicNumber != MAGIC_NUMBER)
        throw runtime_error("This is not valid archive file");

    files.clear();
    while (is.tellg() < endOffset) {
        FileHeader fileHeader;
        fileHeader.read(is);

        files.push_back(make_shared<File>(nullptr, fileHeader));
    }
}

void CentralDirectory::deleteFile(shared_ptr<File> file) {
    for (int i = 0; i < files.size(); ++i) {
        if (files[i] == file) {
            files.erase(files.begin() + i);
            break;
        }
    }

    int closestFileOffset = offset;

    for (auto &f : files) {
        if (f->getHeader().offset > file->getHeader().offset && f->getHeader().offset < closestFileOffset)
            closestFileOffset = f->getHeader().offset;
    }

    for (auto &f : files) {
        if (f->getHeader().offset > file->getHeader().offset)
            f->getHeader().offset -= closestFileOffset - file->getHeader().offset;
    }
}

vector<shared_ptr<File>>& CentralDirectory::getFiles() {
    return files;
}

int CentralDirectory::getOffset() {
    return offset;
}

