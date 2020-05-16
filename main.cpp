#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <fstream>
#include <QElapsedTimer>
#include <QThread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "archive.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	MainWindow w;
    w.setArchive(make_shared<Archive>("out.taf"));
    w.show();

	return a.exec();
}
