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
        time_t lastAccessTime;

        FileHeader();
        FileHeader(string name);

        void readStatus();

		void write(ostream &os);
		void read(istream &is);
};

#endif // FILEHEADER_H
