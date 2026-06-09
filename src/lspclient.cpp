#include "lspclient.h"
#include <QFileInfo>
#include <QDir>
#include <QJsonParseError>

LspClient::LspClient(QObject *parent)
    : QObject(parent)
{
}

LspClient::~LspClient()
{
    stop();
}

bool LspClient::start(const QString &serverPath, const QString &workspaceRoot)
{
    if (process && process->state() != QProcess::NotRunning) {
        stop();
    }

    if (!QFileInfo::exists(serverPath))
        return false;

    this->serverPath = serverPath;
    this->workspaceRoot = workspaceRoot;

    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::SeparateChannels);

    connect(process, &QProcess::readyReadStandardOutput,
            this, &LspClient::onReadyReadStdout);
    connect(process, &QProcess::readyReadStandardError,
            this, &LspClient::onReadyReadStderr);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &LspClient::onProcessFinished);

    process->start(serverPath, {});

    if (!process->waitForStarted(5000))
        return false;

    QJsonObject init;
    init["jsonrpc"] = "2.0";
    init["id"] = ++messageId;
    init["method"] = "initialize";

    QJsonObject params;
    QJsonObject clientInfo;
    clientInfo["name"] = "Elitra IDE";
    clientInfo["version"] = "1.0.0";
    params["processId"] = QJsonValue::Null;
    params["clientInfo"] = clientInfo;

    QJsonObject capabilities;
    QJsonObject textDocument;
    QJsonObject completion;
    completion["completionItem"] = QJsonObject();
    completion["contextSupport"] = true;
    textDocument["completion"] = completion;
    textDocument["definition"] = QJsonObject();
    textDocument["hover"] = QJsonObject();
    capabilities["textDocument"] = textDocument;
    params["capabilities"] = capabilities;

    QJsonObject workspace;
    QJsonObject didChangeWatchedFiles;
    didChangeWatchedFiles["dynamicRegistration"] = true;
    workspace["didChangeWatchedFiles"] = didChangeWatchedFiles;
    capabilities["workspace"] = workspace;

    init["params"] = params;
    sendMessage(init);

    QJsonObject initialized;
    initialized["jsonrpc"] = "2.0";
    initialized["method"] = "initialized";
    initialized["params"] = QJsonObject();
    sendMessage(initialized);

    return true;
}

void LspClient::stop()
{
    if (process) {
        if (process->state() != QProcess::NotRunning) {
            QJsonObject shutdown;
            shutdown["jsonrpc"] = "2.0";
            shutdown["id"] = ++messageId;
            shutdown["method"] = "shutdown";
            sendMessage(shutdown);

            QJsonObject exit;
            exit["jsonrpc"] = "2.0";
            exit["method"] = "exit";
            sendMessage(exit);

            process->waitForFinished(2000);
            if (process->state() != QProcess::NotRunning)
                process->kill();
        }
        process->deleteLater();
        process = nullptr;
    }
}

bool LspClient::isRunning() const
{
    return process && process->state() == QProcess::Running;
}

void LspClient::sendMessage(const QJsonObject &msg)
{
    if (!process || process->state() != QProcess::Running) return;

    QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact);
    QByteArray header = QString("Content-Length: %1\r\n\r\n").arg(data.size()).toUtf8();
    process->write(header + data);
}

void LspClient::onReadyReadStdout()
{
    QByteArray data = process->readAllStandardOutput();
    pendingMessage += QString::fromUtf8(data);

    while (true) {
        int headerEnd = pendingMessage.indexOf("\r\n\r\n");
        if (headerEnd == -1) break;

        QString header = pendingMessage.left(headerEnd);
        int contentLength = 0;
        for (const QString &line : header.split("\r\n")) {
            if (line.startsWith("Content-Length:")) {
                contentLength = line.mid(15).trimmed().toInt();
                break;
            }
        }

        if (contentLength == 0) {
            pendingMessage = pendingMessage.mid(headerEnd + 4);
            continue;
        }

        int bodyStart = headerEnd + 4;
        if (pendingMessage.size() < bodyStart + contentLength) break;

        QByteArray body = pendingMessage.mid(bodyStart, contentLength).toUtf8();
        pendingMessage = pendingMessage.mid(bodyStart + contentLength);

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(body, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            handleMessage(doc.object());
        }
    }
}

void LspClient::onReadyReadStderr()
{
    QString err = QString::fromUtf8(process->readAllStandardError());
    emit serverError(err);
}

void LspClient::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    QString msg = QString("LSP server exited (code: %1, crash: %2)")
        .arg(exitCode)
        .arg(status == QProcess::CrashExit);
    emit serverOutput(msg);
}

void LspClient::handleMessage(const QJsonObject &msg)
{
    if (msg.contains("method")) {
        QString method = msg["method"].toString();
        if (method == "textDocument/publishDiagnostics") {
            handlePublishDiagnostics(msg["params"].toObject());
        }
    }

    if (msg.contains("id") && msg.contains("result")) {
        int id = msg["id"].toInt();
        QJsonObject result = msg["result"].toObject();

        if (result.contains("items")) {
            QJsonArray items = result["items"].toArray();
            QStringList completions;
            for (const QJsonValue &v : items) {
                QJsonObject item = v.toObject();
                QString label = item["label"].toString();
                if (!label.isEmpty()) completions << label;
            }
            if (!completions.isEmpty())
                emit completionsReady(completions);
        }

        if (result.contains("range")) {
            QJsonObject range = result["range"].toObject();
            QJsonObject start = range["start"].toObject();
            QString targetUri = result["uri"].toString();
            if (targetUri.isEmpty() && msg["result"].isObject()) {
                QJsonObject r = msg["result"].toObject();
                targetUri = r.contains("targetUri")
                    ? r["targetUri"].toString()
                    : result.contains("uri") ? result["uri"].toString() : "";
            }
            emit definitionResult(targetUri,
                start["line"].toInt(), start["character"].toInt());
        }
    }

    if (msg.contains("method") && msg["method"].toString() == "textDocument/hover") {
        QJsonObject params = msg["params"].toObject();
        QJsonObject contents = params["contents"].toObject();
        if (contents.contains("value")) {
            emit hoverResult(contents["value"].toString());
        }
    }
}

void LspClient::handlePublishDiagnostics(const QJsonObject &params)
{
    QString uri = params["uri"].toString();
    QJsonArray diags = params["diagnostics"].toArray();
    emit diagnosticsReady(uri, diags);
}

void LspClient::openDocument(const QString &uri, const QString &languageId, const QString &text)
{
    QJsonObject msg;
    msg["jsonrpc"] = "2.0";
    msg["method"] = "textDocument/didOpen";

    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    textDoc["languageId"] = languageId;
    textDoc["version"] = 1;
    textDoc["text"] = text;
    params["textDocument"] = textDoc;
    msg["params"] = params;
    sendMessage(msg);
}

void LspClient::changeDocument(const QString &uri, const QString &text, int version)
{
    QJsonObject msg;
    msg["jsonrpc"] = "2.0";
    msg["method"] = "textDocument/didChange";

    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    textDoc["version"] = version;
    params["textDocument"] = textDoc;

    QJsonArray contentChanges;
    QJsonObject change;
    change["text"] = text;
    contentChanges.append(change);
    params["contentChanges"] = contentChanges;

    msg["params"] = params;
    sendMessage(msg);
}

void LspClient::closeDocument(const QString &uri)
{
    QJsonObject msg;
    msg["jsonrpc"] = "2.0";
    msg["method"] = "textDocument/didClose";

    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    params["textDocument"] = textDoc;

    msg["params"] = params;
    sendMessage(msg);
}

void LspClient::requestCompletion(const QString &uri, int line, int character)
{
    QJsonObject msg;
    msg["jsonrpc"] = "2.0";
    msg["id"] = ++messageId;
    msg["method"] = "textDocument/completion";

    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    params["textDocument"] = textDoc;

    QJsonObject position;
    position["line"] = line;
    position["character"] = character;
    params["position"] = position;

    msg["params"] = params;
    sendMessage(msg);
}

void LspClient::requestDefinition(const QString &uri, int line, int character)
{
    QJsonObject msg;
    msg["jsonrpc"] = "2.0";
    msg["id"] = ++messageId;
    msg["method"] = "textDocument/definition";

    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    params["textDocument"] = textDoc;

    QJsonObject position;
    position["line"] = line;
    position["character"] = character;
    params["position"] = position;

    msg["params"] = params;
    sendMessage(msg);
}

void LspClient::requestHover(const QString &uri, int line, int character)
{
    QJsonObject msg;
    msg["jsonrpc"] = "2.0";
    msg["id"] = ++messageId;
    msg["method"] = "textDocument/hover";

    QJsonObject params;
    QJsonObject textDoc;
    textDoc["uri"] = uri;
    params["textDocument"] = textDoc;

    QJsonObject position;
    position["line"] = line;
    position["character"] = character;
    params["position"] = position;

    msg["params"] = params;
    sendMessage(msg);
}
