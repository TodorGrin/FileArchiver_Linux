#ifndef CENTRALDIRECTORY_H
#define CENTRALDIRECTORY_H

#include <vector>
#include "fileheader.h"
#include "file.h"

using namespace std;

class CentralDirectory {
    private:
        const long long MAGIC_NUMBER = 4452863974852415491;

        vector<shared_ptr<File>> files;
        int offset;

	public:

		CentralDirectory();

		void write(ostream &os);
		void read(istream &is);

        void deleteFile(shared_ptr<File> file);

        vector<shared_ptr<File>>& getFiles();
        int getOffset();
};

#endif // CENTRALDIRECTORY_H
