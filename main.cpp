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

void createFiles(string filePath) {
    for (int i = 1; i <= 16; ++i) {
        ofstream os((QString::fromStdString(filePath) + QString::number(i) + ".txt").toStdString().c_str(), ios::binary);

        for (int k = 0; k < 200; ++k) //2000
            for (int j = 0; j < 56; ++j)
                os.put(j);

        os.close();
    }
}

void addFiles(Archive &ar, string filePath) {
    for (int i = 1; i <= 16; ++i)
        ar.addFile((QString::fromStdString(filePath) + QString::number(i) + ".txt").toStdString().c_str());
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

    shared_ptr<Archive> ar = make_shared<Archive>("out.taf");
	MainWindow w;
    w.setArchive(ar);
    w.show();

	return a.exec();
}
