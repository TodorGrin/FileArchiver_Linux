#ifndef CENTRALDIRECTORY_H
#define CENTRALDIRECTORY_H

#include <vector>
#include "fileheader.h"
#include "file.h"

using namespace std;

class CentralDirectory {
	public:
        vector<shared_ptr<File>> files;
        int offset;

		CentralDirectory();

		void write(ostream &os);
		void read(istream &is);

        void deleteFile(shared_ptr<File> file);
};

#endif // CENTRALDIRECTORY_H
