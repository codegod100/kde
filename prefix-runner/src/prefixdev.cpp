// SPDX-License-Identifier: MIT
#include "prefixdev.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <algorithm>

namespace PrefixDev
{

static const QString DefaultChannel = QStringLiteral("conda-forge");

QUrl Package::pageUrl() const
{
    QUrl url(QStringLiteral("https://prefix.dev"));
    url.setPath(QStringLiteral("/channels/%1/packages/%2").arg(channel, name));
    return url;
}

QString Package::pixiAddCommand() const
{
    if (channel == DefaultChannel) {
        return QStringLiteral("pixi add %1").arg(name);
    }
    return QStringLiteral("pixi add %1::%2").arg(channel, name);
}

QString Package::pixiGlobalCommand() const
{
    if (channel == DefaultChannel) {
        return QStringLiteral("pixi global install %1").arg(name);
    }
    return QStringLiteral("pixi global install -c %1 %2").arg(channel, name);
}

QUrl endpoint()
{
    return QUrl(QStringLiteral("https://prefix.dev/api/graphql"));
}

QStringList triggerWords()
{
    return {QStringLiteral("pixi "), QStringLiteral("conda ")};
}

QString searchTerm(const QString &query)
{
    for (const QString &word : triggerWords()) {
        if (query.startsWith(word, Qt::CaseInsensitive)) {
            const QString term = query.mid(word.size()).trimmed();
            return term.size() >= MinTermLength ? term : QString();
        }
    }
    return {};
}

QByteArray requestBody(const QString &term)
{
    static const QString query = QStringLiteral(
        "query($q: String!, $limit: Int!) {"
        " packages(filters: {name: {contains: $q}},"
        " orderBy: {bySimilarity: {field: NAME, matches: $q, direction: DESC}},"
        " limit: $limit) {"
        " page { name summary channel { name } latestVersion { version } } } }");
    const QJsonObject body{
        {QStringLiteral("query"), query},
        {QStringLiteral("variables"),
         QJsonObject{{QStringLiteral("q"), term.toLower()}, {QStringLiteral("limit"), MaxResults * 2}}},
    };
    return QJsonDocument(body).toJson(QJsonDocument::Compact);
}

QList<Package> parseResponse(const QByteArray &json, const QString &term, QString *error)
{
    auto fail = [error](const QString &message) {
        if (error) {
            *error = message;
        }
        return QList<Package>();
    };

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (!doc.isObject()) {
        return fail(parseError.errorString());
    }
    const QJsonArray errors = doc[QStringLiteral("errors")].toArray();
    if (!errors.isEmpty()) {
        return fail(errors.first()[QStringLiteral("message")].toString());
    }
    const QJsonValue page = doc[QStringLiteral("data")][QStringLiteral("packages")][QStringLiteral("page")];
    if (!page.isArray()) {
        return fail(QStringLiteral("response has no data.packages.page"));
    }

    QList<Package> packages;
    for (const QJsonValue &item : page.toArray()) {
        Package p{
            item[QStringLiteral("name")].toString(),
            item[QStringLiteral("channel")][QStringLiteral("name")].toString(),
            item[QStringLiteral("latestVersion")][QStringLiteral("version")].toString(),
            item[QStringLiteral("summary")].toString().simplified(),
        };
        if (!p.name.isEmpty() && !p.channel.isEmpty()) {
            packages << p;
        }
    }

    // prefix.dev ranks by name similarity; keep that order, but put the exact
    // name first and conda-forge ahead of the many mirrors of the same package.
    const QString wanted = term.trimmed().toLower();
    auto rank = [&wanted](const Package &p) {
        return (p.name.compare(wanted, Qt::CaseInsensitive) == 0 ? 0 : 2) + (p.channel == DefaultChannel ? 0 : 1);
    };
    std::stable_sort(packages.begin(), packages.end(), [&rank](const Package &a, const Package &b) {
        return rank(a) < rank(b);
    });
    if (packages.size() > MaxResults) {
        packages.resize(MaxResults);
    }
    return packages;
}

}
