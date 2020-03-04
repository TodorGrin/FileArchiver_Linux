#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <fstream>
#include <QElapsedTimer>
#include "archive.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

    for (int i = 1; i <= 16; ++i) {
        ofstream os((QString("1\\Test") + QString::number(i) + ".txt").toStdString().c_str(), ios::binary);

        for (int k = 0; k < 1000; ++k)
            for (int j = 0; j < 256; ++j)
                os.put(j);

        os.close();
    }

    QElapsedTimer timer;
    timer.start();

    {
        Archive ar;

        for (int i = 1; i <= 16; ++i)
            ar.addFile((QString("1\\Test") + QString::number(i) + ".txt").toStdString().c_str());

        ofstream os("out.taf", ios::binary);
        ar.write(os);
        os.close();
    }

    cout << timer.elapsed() << endl;
    timer.restart();

    Archive ar;

    ifstream is("out.taf", ios::binary);
    ar.decompress(is);
    is.close();

    cout << timer.elapsed() << endl;

	MainWindow w;
	w.show();

	return a.exec();
}
