#ifndef FILEHEADER_H
#define FILEHEADER_H

#include <string>
#include <iostream>

using namespace std;

class FileHeader {
	public:
		int offset;
		string name;

		FileHeader();


		void write(ostream &os);
		void read(istream &is);
};

#endif // FILEHEADER_H
