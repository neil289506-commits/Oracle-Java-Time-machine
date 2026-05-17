#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QIcon>
#include <QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), process(new QProcess(this))
{
    setupData();

    // 樣式設定：黑底白字, 14px, 微軟正黑體
    QString mainStyle = "QWidget { background-color: #000000; color: #FFFFFF; font-family: 'Microsoft JhengHei', '微軟正黑體'; font-size: 14px; }"
                        "QComboBox, QLineEdit { background-color: #222222; border: 1px solid #444444; padding: 5px; color: #FFFFFF; }"
                        "QPushButton { background-color: #333333; border: 1px solid #555555; padding: 8px; color: #FFFFFF; }"
                        "QPushButton:hover { background-color: #444444; }"
                        "QTextEdit { background-color: #111111; border: 1px solid #333333; color: #FFFFFF; }";
    this->setStyleSheet(mainStyle);

    if (QFile::exists(":/app.ico")) setWindowIcon(QIcon(":/app.ico"));

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 預設路徑設定
    savePath = QCoreApplication::applicationDirPath() + "/Downloads";
    QDir dir(savePath); if (!dir.exists()) dir.mkpath("."); 

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathEdit = new QLineEdit(savePath);
    pathEdit->setReadOnly(true);
    QPushButton *pathBtn = new QPushButton("選擇路徑");
    pathLayout->addWidget(new QLabel("下載位置:"));
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(pathBtn);

    QHBoxLayout *selectLayout = new QHBoxLayout();
    versionCombo = new QComboBox();
    versionCombo->addItem("--請選擇版本--");
    versionCombo->addItems(sortedVersions);
    componentCombo = new QComboBox();
    componentCombo->setEnabled(false);
    selectLayout->addWidget(new QLabel("版本:"));
    selectLayout->addWidget(versionCombo);
    selectLayout->addWidget(new QLabel("組件:"));
    selectLayout->addWidget(componentCombo);

    downloadBtn = new QPushButton("開始下載 (壓榨頻寬模式)");
    downloadBtn->setMinimumHeight(45);
    logArea = new QTextEdit();
    logArea->setReadOnly(true);

    mainLayout->addLayout(pathLayout);
    mainLayout->addLayout(selectLayout);
    mainLayout->addWidget(downloadBtn);
    mainLayout->addWidget(logArea);

    setCentralWidget(centralWidget);
    setWindowTitle("Oracle Java Time Machine v2.4");
    resize(700, 500);

    connect(pathBtn, &QPushButton::clicked, this, &MainWindow::selectSavePath);
    connect(versionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateComponentList);
    connect(downloadBtn, &QPushButton::clicked, this, &MainWindow::handleDownload);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::updateLog);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::processFinished);
}

void MainWindow::setupData() {
    sortedVersions = {"Java 7", "Java 8", "Java 9", "Java 10", "Java 11", "Java 12", "Java 13", "Java 14", "Java 15", "Java 16", "Java 17", "Java 18", "Java 19", "Java 20", "Java 21", "Java 22", "Java 23", "Java 24", "Java 25", "Java 26"};
    javaData["Java 7"]  = qMakePair("1iPE6lgPvQskBUTK_G3Ur0NlS0NIAvei7", "1XwP0gqMLt6a7y-khjFE_ovDekpTyYNdP");
    javaData["Java 8"]  = qMakePair("1hGWiTMd_BC4m7VYkbKUNKU162SL5p6UG", "1_1TmndhmNxtH0BmEdBqqxOrdDMsFiunb");
    javaData["Java 9"]  = qMakePair("1dAdwnQr_usKjxrwCBtAa1hROUejLUhGd", "19C5AHuNlpZ-xZksVzw6pRv6aRKDdQ8qB");
    javaData["Java 10"] = qMakePair("1GEn-OjZNAR2W7hPBqBbSjrd6KmUWxWPm", "1MZpv5RMcBHLN6wyTixB4h3_8P-zA71iJ");
    javaData["Java 11"] = qMakePair("1x6sxyI0CyNy0nULp0Y8Vi5yw2l0d6oXf", "");
    javaData["Java 12"] = qMakePair("12Ggmc0TR5biSA6MMaX6C34wHySrBkuEp", "");
    javaData["Java 13"] = qMakePair("1blXi_szIjjb4V-tWcjxK2ZaMuovnSbmw", "");
    javaData["Java 14"] = qMakePair("1aQGa7VuxKY-ZrXp1vdzLbjTSw6MjpJjS", "");
    javaData["Java 15"] = qMakePair("1nHT4EvjP1OgCp46zMLASJKPh15wVGNYa", "");
    javaData["Java 16"] = qMakePair("1oKdz5sLfmPqOaTvXHbMaeJTe4h3jczkj", "");
    javaData["Java 17"] = qMakePair("1o_5ax5D8z1f-3OrVRx7YktS4BFJbEzIi", "");
    javaData["Java 18"] = qMakePair("1ZAKWeDGKqME52885bL-F7_cE7CWe9Q0j", "");
    javaData["Java 19"] = qMakePair("1jvK7UUNm9ZfKZp_Pzpf5nx9eIL0sOKhQ", "");
    javaData["Java 20"] = qMakePair("10PG_45Yw_lu5xZttZlwJhlppN7aLrYUN", "");
    javaData["Java 21"] = qMakePair("1qfJvPN1QKXnbU1M0dPRJgMJnN3u_iVjE", "");
    javaData["Java 22"] = qMakePair("1_ITx1OrdpTGsICG-mQNtx--Dqe7DG1zP", "");
    javaData["Java 23"] = qMakePair("1O72m0WibXcSPJoDvXFSYokWLmWdeiO9e", "");
    javaData["Java 24"] = qMakePair("1CqzJYUxWsKbUeCPqlHRcB36Wybq2SvJE", "");
    javaData["Java 25"] = qMakePair("1f5BhvEiZ9j2y7vWra0rcxL4XpX-izaFr", "");
    javaData["Java 26"] = qMakePair("1kz0DZfmC8gFVWcsXQGR_blSb_zcMEz1h", "");
}

void MainWindow::updateComponentList(int index) {
    componentCombo->clear();
    if (index <= 0) { componentCombo->setEnabled(false); return; }
    componentCombo->setEnabled(true);
    QString version = versionCombo->currentText();
    componentCombo->addItem("JDK");
    if (!javaData[version].second.isEmpty()) componentCombo->addItem("JRE");
}

void MainWindow::selectSavePath() {
    QString dir = QFileDialog::getExistingDirectory(this, "選擇路徑", savePath);
    if (!dir.isEmpty()) { savePath = dir; pathEdit->setText(savePath); }
}

void MainWindow::handleDownload() {
    if (versionCombo->currentIndex() <= 0 || process->state() == QProcess::Running) return;
    QString ver = versionCombo->currentText();
    QString type = componentCombo->currentText();
    QString fileId = (type == "JDK") ? javaData[ver].first : javaData[ver].second;
    QString fullPath = QDir(savePath).filePath(ver + "_" + type + ".exe");
    logArea->clear();
    logArea->append("啟動下載任務...");
    process->start("python", QStringList() << "-m" << "gdown" << fileId << "-O" << fullPath);
    downloadBtn->setEnabled(false);
}

void MainWindow::updateLog() {
    QString rawOutput = QString::fromLocal8Bit(process->readAllStandardError());
    // 解決 \r 重疊問題，拆分行數據
    QStringList lines = rawOutput.split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);

    static QRegularExpression re("([\\d\\.]+\\s*[kMG]?i?B?)\\s*/\\s*([\\d\\.]+\\s*[kMG]?i?B?).*?([\\d\\.]+\\s*[kMG]?i?B/s)");

    for (const QString &line : lines) {
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString cleanInfo = QString("%1 / %2  %3").arg(match.captured(1), match.captured(2), match.captured(3));
            this->setWindowTitle(cleanInfo);
            logArea->append(cleanInfo); 
        } else if (!line.contains("%") && !line.contains("|")) {
            logArea->append(line.trimmed());
        }
    }
}

void MainWindow::processFinished(int exitCode) {
    downloadBtn->setEnabled(true);
    if (exitCode == 0) {
        logArea->append("<br><font color='#00FFFF'>[SUCCESS] 下載完成！</font>");
        this->setWindowTitle("下載完成 - Oracle Java Time Machine");
    } else {
        logArea->append("<br><font color='#FF0000'>[ERROR] 下載失敗。</font>");
    }
}

MainWindow::~MainWindow() {}