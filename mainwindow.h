#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <set>
#include "archive.h"
#include "folder.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

    private:
        Ui::MainWindow *ui;
        shared_ptr<Archive> archive;
        shared_ptr<Folder> folder;

        void setTableText(int row, int column, string text);

    protected:
        void paintEvent(QPaintEvent *event) override;

	public:
		MainWindow(QWidget *parent = nullptr);
		~MainWindow();

        void setArchive(shared_ptr<Archive> archive);

    private slots:
        void on_fileList_cellDoubleClicked(int row, int column);
        void openFile_onClicked();
        void deleteFile_onClicked();
        void extractArchive_onClicked();
        void saveArchive_onClicked();
};
#endif // MAINWINDOW_H
