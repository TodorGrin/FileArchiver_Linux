#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
}

MainWindow::~MainWindow() {
	delete ui;
}

bool MainWindow::startsWith(string str, string toStart) {
    if (str.size() < toStart.size())
        return false;

    for (int i = 0; i < toStart.size(); ++i)
        if (str[i] != toStart[i])
            return false;

    return true;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    ui->fileList->setRowCount(1);
    folders.clear();
    files.clear();

    for (FileHeader fh : archive->centralDirectory.files) {
        if (!startsWith(fh.name, currentFolder))
            continue;

        string name = fh.name.substr(currentFolder.size(), string::npos);
        auto firstSlashPos = name.find_first_of("/");

        if (firstSlashPos == string::npos) {
            files.insert(name);
        }
        else {
            folders.insert(name.substr(0, firstSlashPos));
        }
    }

    QTableWidgetItem *it = new QTableWidgetItem("...");
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    ui->fileList->setItem(0, 0, it);

    for (string folderName : folders) {
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(folderName));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->fileList->insertRow(ui->fileList->rowCount());
        ui->fileList->setItem(ui->fileList->rowCount() - 1, 0, item);
    }

    for (string fileName : files) {
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(fileName));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->fileList->insertRow(ui->fileList->rowCount());
        ui->fileList->setItem(ui->fileList->rowCount() - 1, 0, item);
    }
}

void MainWindow::on_fileList_cellDoubleClicked(int row, int column) {
    QTableWidgetItem *item = ui->fileList->item(row, column);
    string name = item->text().toStdString();

    if (row == 0) {
        if (currentFolder.find("/") == string::npos)
            return;

        currentFolder = currentFolder.substr(0, currentFolder.size() - 2);
        currentFolder = currentFolder.substr(0, currentFolder.find_last_of("/"));
        if (currentFolder.size() > 0) currentFolder += "/";
        this->update();
    }

    if (folders.find(name) != folders.end()) {
        currentFolder += name + "/";
        this->update();
    }
}
