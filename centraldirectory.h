#ifndef CENTRALDIRECTORY_H
#define CENTRALDIRECTORY_H

#include <vector>
#include "fileheader.h"

using namespace std;

class CentralDirectory {
	public:
		vector<FileHeader> files;

		CentralDirectory();

		void write(ostream &os);
		void read(istream &is);
};

#endif // CENTRALDIRECTORY_H
