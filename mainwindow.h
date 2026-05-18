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
    void on_newRecordButton_clicked();
    void on_exportButton_clicked();

    // Register
    void on_addRunnerButton_clicked();
    void on_clearRegisterTableButton_clicked();

    // Timing
    void on_startButton_clicked();
    void on_finishButton_clicked();
    void on_stopButton_clicked();
    void on_clearButton_clicked();
    void on_undoButton_clicked();

    // Export
    void on_exportButton_2_clicked();

    // Random
    void on_clearAdults_clicked();
    void on_clearFemale_clicked();
    void on_clearMale_clicked();
    void on_clearGirls_clicked();
    void on_clearBoys_clicked();
    void on_generateButton_clicked();
    void generateWinners(QTableWidget* table, QString winner, int row);
    void addRowInRandomTable(QTableWidget* table);
    void on_addToAdultsButton_clicked();
    void on_addToFemaleButton_clicked();
    void on_addToMaleButton_clicked();
    void on_addToGirlsButton_clicked();
    void on_addToBoysButton_clicked();
    void on_addToChildrenButton_clicked();
    void on_clearChildren_clicked();

    // newRecord
    void on_tryAddButton_clicked();

private:
    Ui::MainWindow *ui;

    DisplayWindow* displayWindow = nullptr;

    QTimer* timer;
    QElapsedTimer raceTimer;
    QDateTime raceStartDateTime;

    int runnerCount;
    int ageLimit = 2010;

    bool timerOn = false;
    bool updatingExportTable = false;

    QVector<RemovedRow> undoStack;

    // Registration
    void addDeleteButtonToRow(QTableWidget* table, int row, int column);
    void addCheckBoxesToRow(QTableWidget* table, int row, int startColumn, int endColumn);
    QString cellText(QTableWidget* table, int row, int col);
    bool isChecked(QTableWidget* table, int row, int col);

    // Timing
    QString formatTime(qint64 ms);
    void updateButtonsEnabled();
    void renumberRows();
    void removeRowWithUndo(QTableWidget* table, int row);
    void updateDisplay();

    // Export
    void updateExportTable();
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
};
#endif // MAINWINDOW_H
