#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "huffman.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	Huffman h;
	//h.compress("FPU.zip", "out.txt");
	//h.decompress("out.txt", "FPU2.zip");

	MainWindow w;
	w.show();

	return a.exec();
}
