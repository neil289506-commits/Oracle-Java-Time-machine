#include "environmentchecker.h"

#include <QProcess>
#include <QMessageBox>
#include <QProgressDialog>
#include <QEventLoop>
#include <QDesktopServices>
#include <QUrl>
#include <QWidget>

EnvironmentChecker::EnvironmentChecker(QObject *parent)
    : QObject(parent)
{
}

bool EnvironmentChecker::detectPython(QString &pythonCommand, QStringList &extraArgs) const
{
    // 依序嘗試常見的 Python 啟動指令
    struct Candidate { QString cmd; QStringList args; };
    const QList<Candidate> candidates = {
        {"python",  {}},
        {"python3", {}},
        {"py",      {"-3"}},
    };

    for (const auto &c : candidates) {
        QProcess proc;
        proc.start(c.cmd, QStringList(c.args) << "--version");
        if (!proc.waitForStarted(1500)) continue;
        proc.waitForFinished(3000);
        if (proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0) {
            pythonCommand = c.cmd;
            extraArgs = c.args;
            return true;
        }
    }
    return false;
}

bool EnvironmentChecker::detectGdown(const QString &pythonCommand, const QStringList &extraArgs) const
{
    QProcess proc;
    QStringList args = extraArgs;
    args << "-m" << "gdown" << "--version";
    proc.start(pythonCommand, args);
    if (!proc.waitForStarted(1500)) return false;
    proc.waitForFinished(5000);
    return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

void EnvironmentChecker::openPythonDownloadPage() const
{
    QDesktopServices::openUrl(QUrl("https://www.python.org/downloads/"));
}

bool EnvironmentChecker::installGdown(QWidget *parentWidget, const QString &pythonCommand, const QStringList &extraArgs) const
{
    QProgressDialog progress("正在安裝 gdown，請稍候...", QString(), 0, 0, parentWidget);
    progress.setWindowTitle("環境設定");
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(nullptr);
    progress.setMinimumDuration(0);
    progress.show();

    QProcess proc;
    QEventLoop loop;
    connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            &loop, &QEventLoop::quit);

    QStringList args = extraArgs;
    args << "-m" << "pip" << "install" << "--upgrade" << "gdown";
    proc.start(pythonCommand, args);
    if (!proc.waitForStarted(3000)) {
        progress.close();
        return false;
    }
    loop.exec();
    progress.close();

    return proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
}

bool EnvironmentChecker::ensureReady(QWidget *parentWidget, QString &outPythonCommand, QStringList &outExtraArgs)
{
    QString pythonCommand;
    QStringList extraArgs;

    if (!detectPython(pythonCommand, extraArgs)) {
        auto reply = QMessageBox::question(
            parentWidget,
            "缺少 Python 執行環境",
            "偵測不到系統中的 Python。\n\n"
            "本工具需要 Python 搭配 gdown 才能下載檔案。\n"
            "是否要開啟 Python 官方網站進行下載安裝？\n\n"
            "（安裝完成後請重新啟動本程式）",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (reply == QMessageBox::Yes) {
            openPythonDownloadPage();
        }
        return false;
    }

    if (!detectGdown(pythonCommand, extraArgs)) {
        auto reply = QMessageBox::question(
            parentWidget,
            "缺少 gdown 套件",
            QString("已偵測到 Python（%1），但尚未安裝 gdown 套件。\n\n"
                    "是否要自動執行安裝：\n\"%1 -m pip install gdown\"？")
                .arg(pythonCommand),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (reply != QMessageBox::Yes) {
            return false;
        }

        if (!installGdown(parentWidget, pythonCommand, extraArgs)) {
            QMessageBox::critical(parentWidget, "安裝失敗",
                "gdown 安裝失敗，請確認網路連線或手動執行：\n"
                + pythonCommand + " -m pip install gdown");
            return false;
        }

        if (!detectGdown(pythonCommand, extraArgs)) {
            QMessageBox::critical(parentWidget, "安裝後仍無法偵測",
                "已嘗試安裝 gdown，但仍無法偵測到套件，請手動檢查安裝結果。");
            return false;
        }

        QMessageBox::information(parentWidget, "安裝完成", "gdown 套件已成功安裝！");
    }

    outPythonCommand = pythonCommand;
    outExtraArgs = extraArgs;
    return true;
}
