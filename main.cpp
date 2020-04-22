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

        for (int k = 0; k < 2; ++k) //2000
            for (int j = 0; j < 256; ++j)
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

    createFiles("1/Test");
    createFiles("2/Test");
    createFiles("Test");

    QElapsedTimer timer;
    timer.start();

    {
        Archive ar;

        addFiles(ar, "1/Test");
        addFiles(ar, "2/Test");
        addFiles(ar, "Test");

        ofstream os("out.taf", ios::binary);
        ar.write(os);
        os.close();
    }

    cout << timer.elapsed() << endl;
    timer.restart();

    shared_ptr<Archive> ar = make_shared<Archive>("out.taf");

    //ifstream is("out.taf", ios::binary);
    //ar->decompress(is);
    //is.close();

    //cout << timer.elapsed() << endl;

	MainWindow w;
    w.archive = ar;
    w.show();

	return a.exec();
}
