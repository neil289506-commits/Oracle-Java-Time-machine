#ifndef ENVIRONMENTCHECKER_H
#define ENVIRONMENTCHECKER_H

#include <QObject>
#include <QString>

// 負責偵測系統是否具備 Python 與 gdown 套件，
// 若缺少則詢問使用者是否自動補齊安裝。
class EnvironmentChecker : public QObject
{
    Q_OBJECT

public:
    explicit EnvironmentChecker(QObject *parent = nullptr);

    // 依序嘗試 "python" / "python3" / "py -3"，回傳可用的執行指令與參數前綴。
    // 若找到，pythonCommand 會被設為可執行檔名稱（例如 "python" 或 "py"），
    // extraArgs 會被設為需要附加的固定參數（例如 "py" 需要 "-3"）。
    // 回傳 true 表示找到可用的 Python。
    bool detectPython(QString &pythonCommand, QStringList &extraArgs) const;

    // 檢查指定 Python 環境是否已安裝 gdown 模組。
    bool detectGdown(const QString &pythonCommand, const QStringList &extraArgs) const;

    // 對使用者互動：檢查並在缺少時詢問是否安裝，回傳最終是否具備完整環境。
    // 若使用者同意安裝，會顯示進度對話框並同步等待安裝完成。
    // 成功後 outPythonCommand / outExtraArgs 會回填可用的 Python 執行方式。
    bool ensureReady(QWidget *parentWidget, QString &outPythonCommand, QStringList &outExtraArgs);

private:
    bool installGdown(QWidget *parentWidget, const QString &pythonCommand, const QStringList &extraArgs) const;
    void openPythonDownloadPage() const;
};

#endif // ENVIRONMENTCHECKER_H
