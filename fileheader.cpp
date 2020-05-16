#include "fileheader.h"
#include <sys/stat.h>

FileHeader::FileHeader() {

}

FileHeader::FileHeader(string name) {
    this->name = name;
}

void FileHeader::readStatus(string filePathOnDisk) {
    struct stat st;
    stat(filePathOnDisk.c_str(), &st);

    size = st.st_size;
    lastAccessTime = st.st_atime;
}

void FileHeader::write(ostream &os) {
	os.write((char*) &offset, sizeof(offset));
    os.write((char*) &size, sizeof(size));
    os.write((char*) &compressedSize, sizeof(compressedSize));
    os.write((char*) &lastAccessTime, sizeof(lastAccessTime));

	int nameSize = name.size();
	os.write((char*) &nameSize, sizeof(nameSize));
	os.write(name.c_str(), nameSize);
}

void FileHeader::read(istream &is) {
	is.read((char*) &offset, sizeof(offset));
    is.read((char*) &size, sizeof(size));
    is.read((char*) &compressedSize, sizeof(compressedSize));
    is.read((char*) &lastAccessTime, sizeof(lastAccessTime));

	int nameSize;
	is.read((char*) &nameSize, sizeof(nameSize));
	name.resize(nameSize);

	for (int i = 0; i < nameSize; ++i) {
		is.get(name[i]);
	}
}

bool FileHeader::operator==(FileHeader &file) {
    return (name == file.name);
}
