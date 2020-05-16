#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QFileDialog>
#include <QErrorMessage>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
    ui->fileList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile_onClicked);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::deleteFile_onClicked);
    connect(ui->actionExtract, &QAction::triggered, this, &MainWindow::extractArchive_onClicked);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::extractArchive_onClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select where to extract"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    try {
        archive->decompress(dir.toStdString());
    }
    catch(std::exception &e) {
        QErrorMessage err(this);
        err.setWindowTitle("Error");
        err.showMessage(e.what());
        err.exec();
    }
}

void MainWindow::openFile_onClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open archive", "/home", "Archive files (*.taf);;All files (*.*)");
    shared_ptr<Archive> archive = make_shared<Archive>(fileName.toStdString());
    setArchive(archive);

    this->update();
}

void MainWindow::deleteFile_onClicked() {
    QModelIndexList selectedRows = ui->fileList->selectionModel()->selectedRows();

    for (auto &index : selectedRows) {
        string name = ui->fileList->item(index.row(), 0)->text().toStdString();

        auto files = folder->getFiles();
        auto it = find_if(files.begin(), files.end(), [&](shared_ptr<File> &file){return file->getName() == name;});

        if (it != files.end()) {
            archive->deleteFile(*it);
        }
        else {
            auto folders = folder->getSubfolders();
            auto it = find_if(folders.begin(), folders.end(), [&](shared_ptr<Folder> &folder){return folder->getName() == name;});

            if (it != folders.end()) {
                archive->deleteFolder(*it);
            }
        }
    }

    this->update();
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
    setTableText(0, 2, "");
    setTableText(0, 3, "");

    for (shared_ptr<Folder> &subfolder : folder->getSubfolders()) {
        int row = ui->fileList->rowCount();
        ui->fileList->insertRow(row);

        setTableText(row, 0, subfolder->getName());
    }

    for (shared_ptr<File> file : folder->getFiles()) {
        int row = ui->fileList->rowCount();
        ui->fileList->insertRow(row);

        setTableText(row, 0, file->getName());
        setTableText(row, 1, to_string(file->getHeader().size));
        setTableText(row, 2, to_string(file->getHeader().compressedSize));

        char buffer[32];
        tm *ptm = localtime(&file->getHeader().compressedSize);
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
    this->folder = archive->getParentFolder();
}
