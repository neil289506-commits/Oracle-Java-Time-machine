#include "mainwindow.h"
#include "environmentchecker.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QIcon>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QTimer>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), process(new QProcess(this))
{
    setupData();
    buildUi();
    applyTheme();

    connect(pathBtn, &QPushButton::clicked, this, &MainWindow::selectSavePath);
    connect(versionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateComponentList);
    connect(downloadBtn, &QPushButton::clicked, this, &MainWindow::handleDownload);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::updateLog);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::processFinished);

    // 視窗顯示後再檢查環境，避免阻擋主視窗初次繪製
    QTimer::singleShot(150, this, &MainWindow::checkEnvironmentSilently);
}

void MainWindow::applyTheme()
{
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        qApp->setStyleSheet(QString::fromUtf8(styleFile.readAll()));
        styleFile.close();
    }
}

void MainWindow::buildUi()
{
    if (QFile::exists(":/app.ico")) setWindowIcon(QIcon(":/app.ico"));

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("CentralWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    // ---------- Header ----------
    QLabel *titleLabel = new QLabel("Oracle Java Time Machine");
    titleLabel->setObjectName("TitleLabel");
    QLabel *subtitleLabel = new QLabel("歷史版本 JDK / JRE 下載工具");
    subtitleLabel->setObjectName("SubtitleLabel");

    QVBoxLayout *headerTextLayout = new QVBoxLayout();
    headerTextLayout->setSpacing(2);
    headerTextLayout->addWidget(titleLabel);
    headerTextLayout->addWidget(subtitleLabel);

    mainLayout->addLayout(headerTextLayout);

    QFrame *headerLine = new QFrame();
    headerLine->setObjectName("HeaderLine");
    headerLine->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(headerLine);

    // ---------- 下載位置 ----------
    QGroupBox *pathGroup = new QGroupBox("下載位置");
    QHBoxLayout *pathLayout = new QHBoxLayout(pathGroup);

    savePath = QCoreApplication::applicationDirPath() + "/Downloads";
    QDir dir(savePath);
    if (!dir.exists()) dir.mkpath(".");

    pathEdit = new QLineEdit(savePath);
    pathEdit->setReadOnly(true);
    pathBtn = new QPushButton("瀏覽...");
    pathBtn->setObjectName("PathButton");
    pathBtn->setCursor(Qt::PointingHandCursor);

    pathLayout->addWidget(pathEdit, 1);
    pathLayout->addWidget(pathBtn);

    mainLayout->addWidget(pathGroup);

    // ---------- 版本 / 組件選擇 ----------
    QGroupBox *selectGroup = new QGroupBox("版本選擇");
    QGridLayout *selectLayout = new QGridLayout(selectGroup);
    selectLayout->setHorizontalSpacing(12);
    selectLayout->setVerticalSpacing(6);

    QLabel *versionFieldLabel = new QLabel("Java 版本");
    versionFieldLabel->setObjectName("FieldLabel");
    QLabel *componentFieldLabel = new QLabel("組件類型");
    componentFieldLabel->setObjectName("FieldLabel");

    versionCombo = new QComboBox();
    versionCombo->addItem("--請選擇版本--");
    versionCombo->addItems(sortedVersions);

    componentCombo = new QComboBox();
    componentCombo->setEnabled(false);

    selectLayout->addWidget(versionFieldLabel, 0, 0);
    selectLayout->addWidget(componentFieldLabel, 0, 1);
    selectLayout->addWidget(versionCombo, 1, 0);
    selectLayout->addWidget(componentCombo, 1, 1);
    selectLayout->setColumnStretch(0, 1);
    selectLayout->setColumnStretch(1, 1);

    mainLayout->addWidget(selectGroup);

    // ---------- 下載按鈕 + 進度條 ----------
    downloadBtn = new QPushButton("開始下載");
    downloadBtn->setObjectName("DownloadButton");
    downloadBtn->setMinimumHeight(46);
    downloadBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(downloadBtn);

    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    mainLayout->addWidget(progressBar);

    // ---------- 記錄視窗 ----------
    QGroupBox *logGroup = new QGroupBox("下載記錄");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logArea = new QTextEdit();
    logArea->setObjectName("LogArea");
    logArea->setReadOnly(true);
    logLayout->addWidget(logArea);
    mainLayout->addWidget(logGroup, 1);

    // ---------- 狀態列 ----------
    statusLabel = new QLabel("就緒");
    statusLabel->setObjectName("StatusLabel");
    mainLayout->addWidget(statusLabel);

    setCentralWidget(centralWidget);
    setWindowTitle("Oracle Java Time Machine");
    resize(760, 620);
    setMinimumSize(620, 520);
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

void MainWindow::checkEnvironmentSilently()
{
    EnvironmentChecker checker;
    environmentReady = checker.ensureReady(this, pythonCommand, pythonExtraArgs);

    if (environmentReady) {
        statusLabel->setText(QString("環境就緒：使用 %1 執行 gdown").arg(pythonCommand));
    } else {
        statusLabel->setText("環境尚未就緒：缺少 Python 或 gdown，下載前將再次提示。");
    }
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

    if (!environmentReady) {
        EnvironmentChecker checker;
        environmentReady = checker.ensureReady(this, pythonCommand, pythonExtraArgs);
        if (!environmentReady) {
            statusLabel->setText("下載已取消：Python / gdown 環境未就緒。");
            return;
        }
        statusLabel->setText(QString("環境就緒：使用 %1 執行 gdown").arg(pythonCommand));
    }

    QString ver = versionCombo->currentText();
    QString type = componentCombo->currentText();
    QString fileId = (type == "JDK") ? javaData[ver].first : javaData[ver].second;
    QString fullPath = QDir(savePath).filePath(ver + "_" + type + ".exe");

    logArea->clear();
    progressBar->setValue(0);
    logArea->append("啟動下載任務...");
    statusLabel->setText(QString("下載中：%1 (%2)").arg(ver, type));

    // 移除不支援的 fuzzy 參數，並透過 Monkey Patch 停用 SSL 驗證以防解密失敗
    QString pyScript = QString(
        "import ssl, urllib3\n"
        "try:\n"
        "    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)\n"
        "    ssl._create_default_https_context = ssl._create_unverified_context\n"
        "except AttributeError:\n"
        "    pass\n"
        "import gdown\n"
        "gdown.download(id='%1', output=r'%2', quiet=False)\n"
    ).arg(fileId, fullPath);

    QStringList args = pythonExtraArgs;
    args << "-c" << pyScript;

    process->start(pythonCommand, args);
    downloadBtn->setEnabled(false);
}

void MainWindow::updateLog() {
    QString rawOutput = QString::fromLocal8Bit(process->readAllStandardError());
    QStringList lines = rawOutput.split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);

    static QRegularExpression re("([\\d\\.]+\\s*[kMG]?i?B?)\\s*/\\s*([\\d\\.]+\\s*[kMG]?i?B?).*?([\\d\\.]+\\s*[kMG]?i?B/s)");
    static QRegularExpression percentRe("(\\d{1,3})%");

    for (const QString &line : lines) {
        QRegularExpressionMatch percentMatch = percentRe.match(line);
        if (percentMatch.hasMatch()) {
            int pct = percentMatch.captured(1).toInt();
            progressBar->setValue(qBound(0, pct, 100));
        }

        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString cleanInfo = QString("%1 / %2  %3").arg(match.captured(1), match.captured(2), match.captured(3));
            statusLabel->setText(cleanInfo);
            logArea->append(cleanInfo);
        } else if (!line.contains("%") && !line.contains("|")) {
            logArea->append(line.trimmed());
        }
    }
}

void MainWindow::processFinished(int exitCode) {
    downloadBtn->setEnabled(true);
    if (exitCode == 0) {
        progressBar->setValue(100);
        logArea->append("<br><font color='#4CD9C0'>[SUCCESS] 下載完成！</font>");
        statusLabel->setText("下載完成 - Oracle Java Time Machine");
    } else {
        logArea->append("<br><font color='#FF6B5E'>[ERROR] 下載失敗。</font>");
        statusLabel->setText("下載失敗，請檢查記錄或網路狀態。");
    }
}

MainWindow::~MainWindow() {}