#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QErrorMessage>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
    ui->fileList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->actionCreate, &QAction::triggered, this, &MainWindow::createArchive_onClicked);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openArchive_onClicked);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::deleteFile_onClicked);
    connect(ui->actionExtract, &QAction::triggered, this, &MainWindow::extractArchive_onClicked);
    connect(ui->actionAddFiles, &QAction::triggered, this, &MainWindow::addFile_onClicked);
    connect(ui->actionAddFolder, &QAction::triggered, this, &MainWindow::addFolder_onClicked);
    connect(ui->actionRename, &QAction::triggered, this, &MainWindow::renameFile_onClicked);
    connect(ui->actionExtractFile, &QAction::triggered, this, &MainWindow::extractFiles_onClicked);

    setArchive(nullptr);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::showError(string title, string text) {
    QErrorMessage err(this);
    err.setWindowTitle(QString::fromStdString(title));
    err.showMessage(QString::fromStdString(text));
    err.exec();
}

vector<shared_ptr<File>> MainWindow::getSelectedFiles() {
    QModelIndexList selectedRows = ui->fileList->selectionModel()->selectedRows();
    vector<shared_ptr<File>> selectedFiles;

    for (auto &index : selectedRows) {
        string name = ui->fileList->item(index.row(), 0)->text().toStdString();

        auto files = folder->getFiles();
        auto it = find_if(files.begin(), files.end(), [&](shared_ptr<File> &file){return file->getName() == name;});

        if (it != files.end())
            selectedFiles.push_back(*it);
    }

    return selectedFiles;
}

vector<shared_ptr<Folder>> MainWindow::getSelectedFolders() {
    QModelIndexList selectedRows = ui->fileList->selectionModel()->selectedRows();
    vector<shared_ptr<Folder>> selectedFolders;

    for (auto &index : selectedRows) {
        string name = ui->fileList->item(index.row(), 0)->text().toStdString();

        auto folders = folder->getSubfolders();
        auto it = find_if(folders.begin(), folders.end(), [&](shared_ptr<Folder> &folder){return folder->getName() == name;});

        if (it != folders.end())
            selectedFolders.push_back(*it);
    }

    return selectedFolders;
}

void MainWindow::extractArchive_onClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select where to extract"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    try {
        archive->extract(dir.toStdString());
    }
    catch(exception &e) {
        showError("Error while extracting archive", e.what());
    }
}

void MainWindow::extractFiles_onClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select where to extract"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    try {
        archive->extractFiles(getSelectedFiles(), dir.toStdString());
        archive->extractFolders(getSelectedFolders(), dir.toStdString());
    }
    catch(exception &e) {
        showError("Error while extracting archive", e.what());
    }
}

void MainWindow::openArchive_onClicked() {
    try {
        QString fileName = QFileDialog::getOpenFileName(this, "Open archive", "/home", "Archive files (*.taf);;All files (*.*)");
        shared_ptr<Archive> archive = make_shared<Archive>(fileName.toStdString());
        setArchive(archive);

        this->update();
    }
    catch (exception &e) {
        showError("Error while opening archive", e.what());
    }
}

void MainWindow::createArchive_onClicked() {
    try {
        QString fileName = QFileDialog::getSaveFileName(this, "Create archive", "/home", "Archive files (*.taf);;All files (*.*)");
        string name = fileName.toStdString();
        auto dotPos = name.find_last_of(".");

        if (dotPos == string::npos || name.substr(dotPos + 1) != "taf")
            name += ".taf";

        shared_ptr<Archive> archive = Archive::create(name);
        setArchive(archive);

        this->update();
    }
    catch (exception &e) {
        showError("Error while creating archive", e.what());
    }
}

void MainWindow::addFile_onClicked() {
    try {
        QStringList fileNames = QFileDialog::getOpenFileNames(this, "Add file", "/home", "All files (*.*)");

        for (QString name : fileNames)
            archive->addFile(folder, name.toStdString());

        this->update();
    }
    catch (exception &e) {
        showError("Error while adding file", e.what());
    }
}

void MainWindow::addFolder_onClicked() {
    try {
        QString dir = QFileDialog::getExistingDirectory(this, "Open folder", "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        archive->addFolder(folder, dir.toStdString());
    }
    catch (exception &e) {
        showError("Error while adding folder", e.what());
    }
}

void MainWindow::deleteFile_onClicked() {
    try {
        for (auto &file : getSelectedFiles())
            archive->deleteFile(file);

        for (auto &folder : getSelectedFolders())
            archive->deleteFolder(folder);

        ui->fileList->selectionModel()->clearSelection();
        this->update();
    }
    catch (exception &e) {
        showError("Error while deleting file", e.what());
    }
}

void MainWindow::renameFile_onClicked() {
    auto selectedFiles = getSelectedFiles();
    auto selectedFolders = getSelectedFolders();

    if (selectedFiles.size() + selectedFolders.size() > 1)
        return;

    try {
        if (selectedFiles.size() > 0) {
            bool ok = false;
            QString newName = QInputDialog::getText(this, "Rename", "New file name", QLineEdit::Normal, QString::fromStdString(selectedFiles.front()->getName()), &ok);

            if (ok && !newName.isEmpty())
                archive->renameFile(selectedFiles.front(), newName.toStdString());
        }
        else if (selectedFolders.size() > 0) {
            bool ok = false;
            QString newName = QInputDialog::getText(this, "Rename", "New folder name", QLineEdit::Normal, QString::fromStdString(selectedFolders.front()->getName()), &ok);

            if (ok && !newName.isEmpty())
                archive->renameFolder(selectedFolders.front(), newName.toStdString());
        }

        this->update();
    }
    catch (exception &e) {
        showError("Error while renaming file", e.what());
    }
}

void MainWindow::setTableText(int row, int column, string text) {
    QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(text));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ui->fileList->setItem(row, column, item);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    if (!archive || !folder)
        return;

    ui->fileList->setRowCount(1 + folder->getSubfolders().size() + folder->getFiles().size());
    this->setWindowTitle(QString::fromStdString(folder->getPath() + " - Archiver"));

    setTableText(0, 0, "...");
    setTableText(0, 1, "");
    setTableText(0, 2, "");
    setTableText(0, 3, "");

    int row = 1;
    for (shared_ptr<Folder> &subfolder : folder->getSubfolders()) {
        setTableText(row, 0, subfolder->getName());
        setTableText(row, 1, "");
        setTableText(row, 2, "");
        setTableText(row, 3, "");
        row++;
    }

    for (shared_ptr<File> file : folder->getFiles()) {
        setTableText(row, 0, file->getName());
        setTableText(row, 1, to_string(file->getHeader().size));
        setTableText(row, 2, to_string(file->getHeader().compressedSize));

        char buffer[32];
        tm *ptm = localtime(&file->getHeader().lastAccessTime);
        strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
        setTableText(row, 3, string(buffer));
        row++;
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
    if (archive) {
        this->archive = archive;
        this->folder = archive->getParentFolder();
    }

    bool nul = (archive == nullptr);

    ui->fileList->setVisible(!nul);
    ui->noArchiveLoadedLabel->setVisible(nul);
    ui->actionExtract->setEnabled(!nul);
    ui->actionAddFiles->setEnabled(!nul);
    ui->actionAddFolder->setEnabled(!nul);
    ui->menuFile->setEnabled(!nul);
}
