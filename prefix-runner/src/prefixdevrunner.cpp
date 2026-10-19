// SPDX-License-Identifier: MIT
// KRunner plugin: "pixi <name>" searches packages on prefix.dev.

#include "prefixdev.h"

#include <KLocalizedString>
#include <KRunner/AbstractRunner>
#include <KRunner/Action>
#include <KRunner/QueryMatch>
#include <KRunner/RunnerContext>

#include <QClipboard>
#include <QDesktopServices>
#include <QEventLoop>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

Q_LOGGING_CATEGORY(PREFIXDEV, "nandi.krunner.prefixdev", QtWarningMsg)

using namespace Qt::StringLiterals;

class PrefixDevRunner : public KRunner::AbstractRunner
{
    Q_OBJECT

public:
    PrefixDevRunner(QObject *parent, const KPluginMetaData &metaData)
        : KRunner::AbstractRunner(parent, metaData)
        , m_actions{
              KRunner::Action(u"copy-add"_s, u"edit-copy"_s, i18n("Copy pixi add command")),
              KRunner::Action(u"copy-global"_s, u"edit-copy"_s, i18n("Copy pixi global install command")),
          }
    {
        setTriggerWords(PrefixDev::triggerWords());
        addSyntax(u"pixi :q:"_s, i18n("Searches prefix.dev for conda packages whose name contains :q:"));
    }

    void match(KRunner::RunnerContext &context) override
    {
        const QString term = PrefixDev::searchTerm(context.query());
        if (term.isEmpty()) {
            return;
        }

        // Let typing settle so a fast typist doesn't send a request per letter;
        // a newer query invalidates this context meanwhile.
        QEventLoop loop;
        QTimer::singleShot(250, &loop, &QEventLoop::quit);
        loop.exec();
        if (!context.isValid()) {
            return;
        }

        QNetworkAccessManager network;
        QNetworkRequest request(PrefixDev::endpoint());
        request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/json"_s);
        request.setHeader(QNetworkRequest::UserAgentHeader, u"nandi-krunner-prefixdev/0.1"_s);
        request.setTransferTimeout(8000);
        QNetworkReply *reply = network.post(request, PrefixDev::requestBody(term));
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        reply->deleteLater();
        if (!context.isValid()) {
            return;
        }
        if (reply->error() != QNetworkReply::NoError) {
            qCWarning(PREFIXDEV) << "prefix.dev search failed:" << reply->errorString();
            return;
        }

        QString error;
        const QList<PrefixDev::Package> packages = PrefixDev::parseResponse(reply->readAll(), term, &error);
        if (!error.isEmpty()) {
            qCWarning(PREFIXDEV) << "prefix.dev returned an unexpected response:" << error;
            return;
        }

        QList<KRunner::QueryMatch> matches;
        for (qsizetype i = 0; i < packages.size(); ++i) {
            const PrefixDev::Package &p = packages.at(i);
            const bool exact = p.name.compare(term, Qt::CaseInsensitive) == 0;
            KRunner::QueryMatch match(this);
            match.setId(p.channel + u'/' + p.name);
            // KRunner's compact list shows only the text, so the channel goes there
            // too: the same name usually exists in several channels.
            match.setText(i18nc("package name and channel", "%1 · %2", p.name, p.channel));
            QString subtext = p.version.isEmpty() ? p.channel : i18nc("channel and version", "%1 · %2", p.channel, p.version);
            if (!p.summary.isEmpty()) {
                subtext = i18nc("channel/version and package summary", "%1 · %2", subtext, p.summary);
            }
            match.setSubtext(subtext);
            match.setIconName(u"package-x-generic"_s);
            match.setUrls({p.pageUrl()});
            match.setData(QStringList{p.channel, p.name});
            match.setCategoryRelevance(exact ? KRunner::QueryMatch::CategoryRelevance::High : KRunner::QueryMatch::CategoryRelevance::Moderate);
            match.setRelevance(1.0 - 0.05 * i);
            match.setActions(m_actions);
            matches << match;
        }
        context.addMatches(matches);
    }

    void run(const KRunner::RunnerContext &, const KRunner::QueryMatch &match) override
    {
        const QStringList data = match.data().toStringList();
        if (data.size() != 2) {
            return;
        }
        const PrefixDev::Package p{data.at(1), data.at(0), {}, {}};
        const QString action = match.selectedAction().id();
        if (action == u"copy-add"_s) {
            QGuiApplication::clipboard()->setText(p.pixiAddCommand());
        } else if (action == u"copy-global"_s) {
            QGuiApplication::clipboard()->setText(p.pixiGlobalCommand());
        } else {
            QDesktopServices::openUrl(p.pageUrl());
        }
    }

private:
    const QList<KRunner::Action> m_actions;
};

K_PLUGIN_CLASS_WITH_JSON(PrefixDevRunner, "prefixdevrunner.json")

#include "prefixdevrunner.moc"
