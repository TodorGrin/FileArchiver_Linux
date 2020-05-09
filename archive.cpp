#include "archive.h"
#include <fstream>

Archive::Archive() {

}

Archive::Archive(string path) {
    ifstream is(path, ios::binary);

    centralDirectory.read(is);
}

void Archive::addFile(string path) {
    FileHeader fileHeader(path);
    fileHeader.readStatus();
	fileHeader.offset = 0;

	centralDirectory.files.push_back(move(fileHeader));
}

void Archive::write(ostream &os) {
	Huffman huffman;

    for (FileHeader &fh : centralDirectory.files) {
        fh.offset = os.tellp();

        huffman.compress(fh.name, os);

        fh.compressedSize = os.tellp();
        fh.compressedSize -= fh.offset;
	}

	centralDirectory.write(os);
}

void Archive::decompress(istream &is) {
	Huffman huffman;

	centralDirectory.read(is);

	for (int i = 0; i < centralDirectory.files.size(); ++i) {
		FileHeader &fileHeader = centralDirectory.files[i];
		is.seekg(fileHeader.offset, ios::beg);
		huffman.decompress(is, fileHeader.name);
	}
}
