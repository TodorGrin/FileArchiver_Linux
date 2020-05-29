#include "fileheader.h"
#include <sys/stat.h>
#include <utime.h>

FileHeader::FileHeader() {

}

FileHeader::FileHeader(string name) {
    this->name = name;
}

void FileHeader::readMetadata(string filePathOnDisk) {
    struct stat st;
    if (stat(filePathOnDisk.c_str(), &st) != 0)
        throw runtime_error("Can't read file metadata");

    size = st.st_size;
    lastAccessTime = st.st_atime;
    lastModificationTime = st.st_mtime;
}

void FileHeader::setMetadata(string filePathOnDisk) {
    struct utimbuf time;
    time.actime = lastAccessTime;
    time.modtime = lastModificationTime;

    if (utime(filePathOnDisk.c_str(), &time) != 0)
        throw runtime_error("Can't set file metadata");
}

void FileHeader::write(ostream &os) {
	os.write((char*) &offset, sizeof(offset));
    os.write((char*) &size, sizeof(size));
    os.write((char*) &compressedSize, sizeof(compressedSize));
    os.write((char*) &lastAccessTime, sizeof(lastAccessTime));
    os.write((char*) &lastModificationTime, sizeof(lastModificationTime));

	int nameSize = name.size();
	os.write((char*) &nameSize, sizeof(nameSize));
	os.write(name.c_str(), nameSize);
}

void FileHeader::read(istream &is) {
	is.read((char*) &offset, sizeof(offset));
    is.read((char*) &size, sizeof(size));
    is.read((char*) &compressedSize, sizeof(compressedSize));
    is.read((char*) &lastAccessTime, sizeof(lastAccessTime));
    is.read((char*) &lastModificationTime, sizeof(lastModificationTime));

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
