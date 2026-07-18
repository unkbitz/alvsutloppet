#include "displaywindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QTimer>
#include <QMessageBox>

void MainWindow::setCurrentTimeTextAtStop(const QString& timeText)
{
    ui->currentTimeLabel_2->setText(timeText);
}

void MainWindow::on_stopButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Stoppa tidtagning",
                                  "Är du säker på att du vill stoppa tidtagningen?",
                                  QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::No)
    {
        return;
    }

    clearPressed = false;
    timer->stop();
    timerOn = false;

    updateButtonsEnabled();

    saveAutosave();
}

void MainWindow::on_clearButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Töm listan",
                                  "Listan innehåller resultat.\nÄr du säker på att du vill tömma den?",
                                  QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::No)
    {
        return;
    }

    clearPressed = true;

    ui->resultTable->setRowCount(0);
    timer->stop();
    timerOn = false;
    runnerCount = 0;
    raceStartDateTime = QDateTime();
    raceTimer.invalidate();

    ui->currentTimeLabel->setText("00:00.0");
    ui->currentTimeLabel_2->setText("00:00.0");

    if(displayWindow)
    {
        displayWindow->setCurrentTimeText("00:00.0");
        displayWindow->setLatestTimeText("");
        displayWindow->setLatestFourTimeText("");
    }
    enableAddWalkersToggle();
    updateExportTable();
    updateButtonsEnabled();
    updateAmountOfFinishedRunners();
    saveAutosave();
}

void MainWindow::on_undoStopButton_clicked()
{
    raceTimer.start();
    timer->start(10);
    timerOn = true;

    updateButtonsEnabled();
    updateAmountOfFinishedRunners();
    saveAutosave();
}
