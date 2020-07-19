#ifndef CSV_UTIL_H
#define CSV_UTIL_H
#include <QString>
#include <QStandardItemModel>
#include <QFile>
namespace CSV_Util {

const QString delim = ","; // Assuming all sample CSVs will have comma-separated values, as in provided sample.

bool exportCSV(QStandardItemModel *csvModel, QFile *csvFile);

}
#endif // CSV_UTIL_H
