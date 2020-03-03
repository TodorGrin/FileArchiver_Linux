#include "archive.h"

Archive::Archive() {

}

void Archive::addFile(string path) {
	files.push_back(path);

	FileHeader fileHeader;
	fileHeader.name = path;
	fileHeader.offset = 0;

	centralDirectory.files.push_back(move(fileHeader));
}

void Archive::write(ostream &os) {
	Huffman huffman;

	for (int i = 0; i < files.size(); ++i) {
		huffman.compress(files[i], os);
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
