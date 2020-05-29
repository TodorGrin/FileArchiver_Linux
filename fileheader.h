#ifndef FILEHEADER_H
#define FILEHEADER_H

#include <string>
#include <iostream>
#include <sys/types.h>

using namespace std;

class FileHeader {
	public:
		int offset;
        off_t size;
        off_t compressedSize;
        string name;
        time_t lastAccessTime, lastModificationTime;

        FileHeader();
        FileHeader(std::string name);

        void readMetadata(string filePathOnDisk);
        void setMetadata(string filePathOnDisk);

        void write(std::ostream &os);
        void read(std::istream &is);

        bool operator==(FileHeader &file);
};

#endif // FILEHEADER_H
