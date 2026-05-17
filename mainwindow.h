#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
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

private:
    void setupData();
    QMap<QString, QPair<QString, QString>> javaData; 
    QStringList sortedVersions;
    QComboBox *versionCombo;
    QComboBox *componentCombo;
    QLineEdit *pathEdit;
    QPushButton *downloadBtn;
    QTextEdit *logArea;
    QProcess *process;
    QString savePath;
};

#endif