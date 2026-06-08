#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <qtimer.h>

QJsonArray MainWindow::tableToJson(QTableWidget* table)
{
    QJsonArray rows;

    for(int r = 0; r < table->rowCount(); r++)
    {
        QJsonArray row;

        for(int c = 0; c < table->columnCount(); c++)
        {
            QJsonObject cell;

            QTableWidgetItem* item = table->item(r, c);

            cell["text"] = item ? item->text() : "";
            bool checkboxColumn =
                (table == ui->registerTable && c >= 2 && c <= 5) ||
                (table == ui->exportTable && c >= 4 && c <= 7) ||
                (table == ui->resultTable && c == 3);

            if(checkboxColumn && item)
            {
                cell["checked"] = item->checkState() == Qt::Checked;
            }

            row.append(cell);
        }

        rows.append(row);
    }

    return rows;
}

void MainWindow::saveAutosave()
{
    QJsonObject root;

    root["resultTable"] = tableToJson(ui->resultTable);
    root["exportTable"] = tableToJson(ui->exportTable);
    root["registerTable"] = tableToJson(ui->registerTable);
    root["timerOn"] = timerOn;
    root["raceStartDateTime"] = raceStartDateTime.toString(Qt::ISODate);

    QFile file("autosave.json");

    if(!file.open(QIODevice::WriteOnly))
        return;

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

void MainWindow::jsonToTable(QTableWidget* table, const QJsonArray& rows)
{
    table->setRowCount(0);

    for(const QJsonValue& rowValue : rows)
    {
        QJsonArray rowArray = rowValue.toArray();

        int row = table->rowCount();
        table->insertRow(row);

        for(int c = 0; c < rowArray.size() && c < table->columnCount(); c++)
        {
            QJsonObject cell = rowArray[c].toObject();

            QTableWidgetItem* item = new QTableWidgetItem(cell["text"].toString());

            if(cell.contains("checked"))
            {
                item->setFlags(
                    Qt::ItemIsUserCheckable |
                    Qt::ItemIsEnabled |
                    Qt::ItemIsSelectable
                    );

                item->setCheckState(
                    cell["checked"].toBool() ? Qt::Checked : Qt::Unchecked
                    );
            }

            table->setItem(row, c, item);
        }

        if(table == ui->resultTable)
            addDeleteButtonToRow(ui->resultTable, row, 4);

        if(table == ui->registerTable)
            addDeleteButtonToRow(ui->registerTable, row, 7);
    }
}

void MainWindow::loadAutosave()
{
    QFile file("autosave.json");

    if(!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

    if(!doc.isObject())
        return;

    QJsonObject root = doc.object();

    jsonToTable(ui->resultTable, root["resultTable"].toArray());
    jsonToTable(ui->exportTable, root["exportTable"].toArray());
    jsonToTable(ui->registerTable, root["registerTable"].toArray());

    renumberRows();

    timerOn = root["timerOn"].toBool(false);

    raceStartDateTime = QDateTime::fromString(
        root["raceStartDateTime"].toString(),
        Qt::ISODate
        );

    if(timerOn && raceStartDateTime.isValid())
    {
        timer->start(10);
        updateButtonsEnabled();

        qint64 ms = raceStartDateTime.msecsTo(QDateTime::currentDateTime());
        ui->currentTimeLabel->setText(formatTime(ms));
    }
    else
    {
        timerOn = false;
        ui->currentTimeLabel->setText("00:00.0");
        updateButtonsEnabled();
    }
}