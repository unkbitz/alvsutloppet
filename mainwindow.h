#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QStringList>
#include <QDateTime>
#include <qtablewidget.h>

class QLabel;
class QTimer;
class QTableWidget;
class QTableWidgetItem;
class DisplayWindow;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct RemovedRow
{
    int row;
    QString place;
    QString time;
    QString name;
    bool checked;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // Main Buttons
    void on_timeButton_clicked();
    void on_registerButton_clicked();
    void on_randomButton_clicked();
    void on_exportButton_clicked();
    void on_stopRaceButton_clicked();

    // Register
    void on_addRunnerButton_clicked();
    void on_clearRegisterTableButton_clicked();

    // Timing
    void on_startButton_clicked();
    void on_finishButton_clicked();
    void on_undoButton_clicked();

    // Stop
    void on_stopButton_clicked();
    void on_clearButton_clicked();
    void on_undoStopButton_clicked();

    // Export
    void on_exportButton_2_clicked();

    // Random

    void on_generateButton_clicked();
    void generateWinners(QTableWidget* table, QString winner, int row);
    void addRowInRandomTable(QTableWidget* table);

    // newRecord
    void on_tryAddButton_clicked();

    void on_addWalkersButton_clicked();

private:
    Ui::MainWindow *ui;

    DisplayWindow* displayWindow = nullptr;

    QTimer* timer;
    QElapsedTimer raceTimer;
    QDateTime raceStartDateTime;

    int runnerCount;

    int currentYear = QDate::currentDate().year();
    int ageLimit = currentYear - 16;

    bool timerOn = false;
    bool clearPressed = true;
    bool updatingExportTable = false;

    QVector<RemovedRow> undoStack;

    // Registration
    void addDeleteButtonToRow(QTableWidget* table, int row, int column);
    void addCheckBoxesToRow(QTableWidget* table, int row, int startColumn, int endColumn);
    QString cellText(QTableWidget* table, int row, int col);
    bool isChecked(QTableWidget* table, int row, int col);
    void updateAmountOfRunners();

    // Timing
    QString formatTime(qint64 ms);
    void updateButtonsEnabled();
    void renumberRows();
    void removeRowWithUndo(QTableWidget* table, int row);
    void updateDisplay();
    void updateAmountOfFinishedRunners();

    // Export
    void exportDatabaseCsv();
    void updateExportTable();
    void AddWalkersToExportTable();
    void onExportTableItemChanged(QTableWidgetItem* item);

    // Random
    QStringList buildList(QString targetGroup);
    int getRandom(int min, int max);

    // Main Window
    QLabel* createLogo();
    void resizeEvent(QResizeEvent *event) override;
    void setUpTables();
    void setUpComboBoxes();
    void connectInput();
    void setUpTopLayout();
    void setUpTimingLayout();
    void setUpRegisterLayout();
    void setUpExportLayout();
    void setUpRandomLayout();
    void setUpNewRecordLayout();
    void setUpStopLayout();

    // Auto save
    void saveAutosave();
    void loadAutosave();
    QJsonArray tableToJson(QTableWidget* table);
    void jsonToTable(QTableWidget* table, const QJsonArray& rows);

    // newRecord
    bool desideNewRecord(QTableWidget* table, const QString& newTime);
    int timeToTenths(const QString& timeText);
    void insertHighScore(QTableWidget* table, const QString& name, const QString& time);
    void setTableText(QTableWidget* table, int row, int col, const QString& text);

    // stop
    void setCurrentTimeTextAtStop(const QString& text);
};
#endif // MAINWINDOW_H
