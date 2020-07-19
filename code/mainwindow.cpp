#include "mainwindow.h"
#include "csv_util.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    csvModel = new QStandardItemModel(this);     // a data model that displays a table from a CSV file
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(csvModel);


    // embed model into table
    ui->tableView->setModel(csvModel);
    ui->tableView->resizeColumnsToContents();

    // Giving the user the freedom to open any csv they wish to.
    int attempt = 0;
    while(attempt<=maxTries ) {
        attempt++;
        QString fileAddress = QFileDialog::getOpenFileName(this,
                                                           tr("Open CSV File"), "./../", tr("CSV Files (*.csv)"));
        // return if user escapes when looking for a CSV file
        if(fileAddress.isEmpty()) {
            return;
        }

        file = new QFile(fileAddress);
        if (file->exists()) {
            break;
        } else {
            QMessageBox::critical(this, "File", QString("File does not exist. (Attempt %1 of %2)").
                                  arg(attempt).arg(maxTries));

            // allow maxTries attempts and return if unable to find a readable CSV
            if (attempt == maxTries) {
                return;
            }
        }
    }

    if (file->open(QFile::ReadOnly | QIODevice::Text)) {
        QTextStream in(file);
        bool firstRun = true;
        while (!in.atEnd())    // Read data up to the end of the file
        {
            QString line = in.readLine();

            // set up the horizontal header label from first line of CSV
            if (firstRun) {
                firstRun = false;
                csvModel->setHorizontalHeaderLabels(line.split(","));
                continue;
            }

            // add rows to the model line by line
            QList<QStandardItem *> standardItemsList;
            for (QString item : line.split(CSV_Util::delim)) {
                standardItemsList.append(new QStandardItem(item));
            }
            csvModel->insertRow(csvModel->rowCount(), standardItemsList);
        }
        file->close();

        // update CSV whenever a cell is edited
        connect(ui->tableView->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
    } else {
        QMessageBox::critical(this, "File", "Could not open file.");
        return;
    }
    this->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete csvModel;
    delete proxyModel;
    delete file;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier) {
        if (char(e->key()) == 'S') {
            if(!CSV_Util::exportCSV(csvModel, file)) {
                qWarning() << "We hit a snag. Couldn't save the file.";
            } else {
                QMessageBox::information(this, "File Saved", "CSV Saved Successfully.");
            }
        } else if (char(e->key()) == 'F') {
            ui->lineEdit->setFocus(Qt::OtherFocusReason);
        }
    } else if(e->key() == Qt::Key_Escape) {
        QCoreApplication::quit();
    }
}

void MainWindow::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED (topLeft)
    Q_UNUSED (bottomRight)

    if(!CSV_Util::exportCSV(csvModel, file)) {
        qWarning() << "We hit a snag. Couldn't save the file.";
    } else {
        qDebug() << "CSV Updated.";
        // [TODO]: Searching for a string changes the model while coloring and triggers exportCSV().
        //         Replace current implementation with a delegate(?).
    }
}

void MainWindow::on_lineEdit_textEdited()
{
    searchForString();
}

void MainWindow::on_lineEdit_returnPressed()
{
    searchForString(true);
}

void MainWindow::on_pushButton_clicked()
{
    searchForString(true);
}

void MainWindow::searchForString(bool override) {
    {
        if(matchingIndex.isValid()) {
            ui->tableView->model()->setData(matchingIndex, QBrush(Qt::white), Qt::BackgroundRole);
        }

        QString input = ui->lineEdit->text();
        if (input.isEmpty()) {
            return;
        }

        QRegExp regExp;
        if (caseInsensitive) {
            regExp = QRegExp(input, Qt::CaseInsensitive);
        } else {
            regExp = QRegExp(input);
        }
        proxyModel->setFilterRegExp(regExp);

        int num_cols = proxyModel->columnCount();
        // search for key in all columns one at a time
        for (int i=0; i<num_cols; ++i) {
            proxyModel->setFilterKeyColumn(i);

            // int num_rows = proxyModel->rowCount();
            // qDebug() << QString("rows in column %1 = %2").arg(i).arg(num_rows);

            matchingIndex = proxyModel->mapToSource(proxyModel->index(0,i));
            if(matchingIndex.isValid()){
                ui->tableView->model()->setData(matchingIndex, QBrush(Qt::green), Qt::BackgroundRole);
                ui->tableView->scrollTo(matchingIndex);
                break;
            }
        }

        if(matchingIndex.isValid())
        {
            firstNotFound = false;
        }
        else
        {
            if(!firstNotFound || override) {
                firstNotFound = true;
                QMessageBox::information(this, "Could Not Find", "Search query returned 0 matches. This dialog will only show up next time after you hit a query with some match(es).");
            }
        }
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    caseInsensitive = (arg1 == Qt::Checked) ? true : false;
    qDebug() << "caseInsensitive : " << caseInsensitive;
    searchForString();
}
