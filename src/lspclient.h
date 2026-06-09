#ifndef LSPCLIENT_H
#define LSPCLIENT_H

#include <QObject>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMap>
#include <QTextEdit>
#include <QCompleter>

class CodeEditor;

class LspClient : public QObject {
    Q_OBJECT
public:
    explicit LspClient(QObject *parent = nullptr);
    ~LspClient() override;

    bool start(const QString &serverPath, const QString &workspaceRoot);
    void stop();
    bool isRunning() const;

    void openDocument(const QString &uri, const QString &languageId, const QString &text);
    void changeDocument(const QString &uri, const QString &text, int version);
    void closeDocument(const QString &uri);

    void requestCompletion(const QString &uri, int line, int character);
    void requestDefinition(const QString &uri, int line, int character);
    void requestHover(const QString &uri, int line, int character);

    void setEditor(CodeEditor *editor) { currentEditor = editor; }

signals:
    void completionsReady(const QStringList &completions);
    void definitionResult(const QString &uri, int line, int character);
    void hoverResult(const QString &html);
    void diagnosticsReady(const QString &uri, const QJsonArray &diagnostics);
    void serverOutput(const QString &text);
    void serverError(const QString &text);

private slots:
    void onReadyReadStdout();
    void onReadyReadStderr();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    QProcess *process = nullptr;
    QString pendingMessage;
    int messageId = 0;

    void sendMessage(const QJsonObject &msg);
    QJsonObject readMessage(const QByteArray &data);
    void handleMessage(const QJsonObject &msg);
    void handlePublishDiagnostics(const QJsonObject &params);

    QString serverPath;
    QString workspaceRoot;
    CodeEditor *currentEditor = nullptr;
};

#endif
