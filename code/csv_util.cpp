#include "csv_util.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>


bool CSV_Util::exportCSV(QStandardItemModel *model, QFile *csvFile)
{
    if(csvFile->open(QFile::WriteOnly)) {
        // Collect model data into QString row-by-row
        int rows = model->rowCount();
        int columns = model->columnCount();
        QTextStream out(csvFile);

        if(out.status() == QTextStream::Ok) {
            for (int i = -1; i < rows; i++) {
                QString string;
                for (int j = 0; j < columns; j++) {
                    if (i == -1) { // to get the header from the CSV
                        string += model->horizontalHeaderItem(j)->text();
                    } else {
                        string += model->data(model->index(i,j)).toString();
                    }
                    if (j != columns-1) {
                        string += delim;
                    }
                }
                out << string;
                out << '\n';
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
    csvFile->close();
    return true;
}
