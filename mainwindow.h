#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <set>
#include "archive.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

    private:
        Ui::MainWindow *ui;
        set<string> folders, files;

        bool startsWith(string str, string toStart);

    protected:
        void paintEvent(QPaintEvent *event) override;

	public:
        shared_ptr<Archive> archive;
        string currentFolder = "";

		MainWindow(QWidget *parent = nullptr);
		~MainWindow();
private slots:
        void on_fileList_cellDoubleClicked(int row, int column);
};
#endif // MAINWINDOW_H
