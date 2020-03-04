#include "fileheader.h"

FileHeader::FileHeader() {

}

void FileHeader::write(ostream &os) {
	os.write((char*) &offset, sizeof(offset));

    name = name + "_";
	int nameSize = name.size();
	os.write((char*) &nameSize, sizeof(nameSize));
	os.write(name.c_str(), nameSize);
}

void FileHeader::read(istream &is) {
	is.read((char*) &offset, sizeof(offset));

	int nameSize;
	is.read((char*) &nameSize, sizeof(nameSize));
	name.resize(nameSize);

	for (int i = 0; i < nameSize; ++i) {
		is.get(name[i]);
	}
}
