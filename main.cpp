#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "huffman.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	Huffman h;
	h.compress("test.txt", "out.txt");

	MainWindow w;
	w.show();

	return a.exec();
}
