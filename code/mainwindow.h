#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFile>
#include <QSortFilterProxyModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    public slots:
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

protected:
    void keyPressEvent(QKeyEvent *e);
private slots:
    void on_lineEdit_textEdited();
    void on_lineEdit_returnPressed();
    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *csvModel;
    QSortFilterProxyModel *proxyModel;
    QFile *file;
    void onFindClick();
    bool firstNotFound = false;
    void searchForString(bool override = false);
    int maxTries = 3;
    QModelIndex matchingIndex;
    bool caseInsensitive = false;

};

#endif // MAINWINDOW_H
