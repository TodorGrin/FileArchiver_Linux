#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    folder = make_shared<Folder>("");

	ui->setupUi(this);
    ui->fileList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::setTableText(int row, int column, string text) {
    QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(text));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ui->fileList->setItem(row, column, item);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    ui->fileList->setRowCount(1);
    this->setWindowTitle(QString::fromStdString(folder->getName() + " - Archiver"));

    setTableText(0, 0, "...");
    setTableText(0, 1, "");

    for (shared_ptr<Folder> &subfolder : folder->getSubfolders()) {
        int row = ui->fileList->rowCount();
        ui->fileList->insertRow(row);

        setTableText(row, 0, subfolder->getName());
    }

    for (FileHeader &file : folder->getFiles()) {
        int row = ui->fileList->rowCount();
        ui->fileList->insertRow(row);

        setTableText(row, 0, file.name);
        setTableText(row, 1, to_string(file.size));
        setTableText(row, 2, to_string(file.compressedSize));

        char buffer[32];
        tm *ptm = localtime(&file.lastAccessTime);
        strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
        setTableText(row, 3, string(buffer));
    }
}

void MainWindow::on_fileList_cellDoubleClicked(int row, int column) {
    QTableWidgetItem *item = ui->fileList->item(row, 0);
    string name = item->text().toStdString();

    if (row == 0) {
        if (folder->getParentFolder() != nullptr)
            folder = folder->getParentFolder();

        this->update();
        return;
    }

    auto it = find_if(folder->getSubfolders().begin(), folder->getSubfolders().end(), [&](const shared_ptr<Folder> &f){return f->getName() == name;});

    if (it != folder->getSubfolders().end()) {
        folder = *it;
        this->update();
    }
}

void MainWindow::setArchive(shared_ptr<Archive> archive) {
    this->archive = archive;

    for (FileHeader &fh : archive->centralDirectory.files)
        folder->addFile(fh);
}
