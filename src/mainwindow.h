#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QMap>
#include <QProcess>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleDownload();
    void updateLog();
    void processFinished(int exitCode);
    void selectSavePath();
    void updateComponentList(int index);
    void checkEnvironmentSilently();

private:
    void setupData();
    void buildUi();
    void applyTheme();

    QMap<QString, QPair<QString, QString>> javaData;
    QStringList sortedVersions;

    QComboBox *versionCombo;
    QComboBox *componentCombo;
    QLineEdit *pathEdit;
    QPushButton *downloadBtn;
    QPushButton *pathBtn;
    QTextEdit *logArea;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    QProcess *process;
    QString savePath;

    // Python / gdown 執行環境
    QString pythonCommand;
    QStringList pythonExtraArgs;
    bool environmentReady = false;
};

#endif
