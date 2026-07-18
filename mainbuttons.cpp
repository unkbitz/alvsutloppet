#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_registerButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_timeButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_exportButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    //updateExportTable();
}

void MainWindow::on_randomButton_clicked()
{

    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_stopRaceButton_clicked()
{

    ui->stackedWidget->setCurrentIndex(2);
}

