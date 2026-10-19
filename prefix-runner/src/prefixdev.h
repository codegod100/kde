// SPDX-License-Identifier: MIT
// Talking to prefix.dev's GraphQL API, kept free of KRunner so it can be tested.
#pragma once

#include <QByteArray>
#include <QList>
#include <QString>
#include <QUrl>

namespace PrefixDev
{

inline constexpr int MinTermLength = 2;
inline constexpr int MaxResults = 10;

struct Package {
    QString name;
    QString channel;
    QString version;
    QString summary;

    QUrl pageUrl() const;
    QString channelUrl() const;
    // `pixi add name`, adding the channel to the workspace outside conda-forge.
    QString pixiAddCommand() const;
    // `pixi global install name`, with -c <channel url> outside conda-forge.
    QString pixiGlobalCommand() const;
};

QUrl endpoint();

// What to search for in a KRunner query starting with one of triggerWords(),
// or an empty string when the query is not for us.
QStringList triggerWords();
QString searchTerm(const QString &query);

// The POST body searching package names for term.
QByteArray requestBody(const QString &term);

// The packages in a GraphQL response, exact name matches first and
// conda-forge before other channels, at most MaxResults. Sets error on a
// malformed response or a GraphQL error.
QList<Package> parseResponse(const QByteArray &json, const QString &term, QString *error = nullptr);

}
