#include "centraldirectory.h"

CentralDirectory::CentralDirectory() {

}

void CentralDirectory::write(ostream &os) {
	int beginOffset = os.tellp(), endOffset;

	for (int i = 0; i < files.size(); ++i) {
		files[i].write(os);
	}

	endOffset = os.tellp();
	int offsetFromEnd = endOffset - beginOffset + sizeof(offsetFromEnd);

	os.write((char*) &offsetFromEnd, sizeof(offsetFromEnd));
}

void CentralDirectory::read(istream &is)  {
    int beginOffset = 0, endOffset = 0;

	is.seekg(-4, ios::end);
    endOffset = is.tellg();
	is.read((char*) &beginOffset, sizeof(beginOffset));
	is.seekg(-beginOffset, ios::end);

    files.clear();

    while (is.tellg() < endOffset) {
        FileHeader fileHeader;
        fileHeader.read(is);
        files.push_back(move(fileHeader));
    }
}
