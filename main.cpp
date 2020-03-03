#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <fstream>
#include "archive.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	{
		Archive ar;
		ar.addFile("Test.pdf");

		ofstream os("out.txt", ios::binary);
		ar.write(os);
		os.close();
	}

	Archive ar;

	ifstream is("out.txt", ios::binary);
	ar.decompress(is);
	is.close();

	MainWindow w;
	w.show();

	return a.exec();
}
