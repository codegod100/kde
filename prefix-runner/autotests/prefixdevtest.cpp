// SPDX-License-Identifier: MIT
#include "prefixdev.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTest>

using namespace Qt::StringLiterals;

class PrefixDevTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void searchTerm()
    {
        QCOMPARE(PrefixDev::searchTerm(u"pixi numpy"_s), u"numpy"_s);
        QCOMPARE(PrefixDev::searchTerm(u"Conda  ripgrep "_s), u"ripgrep"_s);
        QCOMPARE(PrefixDev::searchTerm(u"pixi n"_s), QString());
        QCOMPARE(PrefixDev::searchTerm(u"numpy"_s), QString());
        QCOMPARE(PrefixDev::searchTerm(u"pixinumpy"_s), QString());
    }

    void requestBody()
    {
        const QJsonObject body = QJsonDocument::fromJson(PrefixDev::requestBody(u"NumPy"_s)).object();
        QVERIFY(body[u"query"_s].toString().contains(u"packages("_s));
        QCOMPARE(body[u"variables"_s][u"q"_s].toString(), u"numpy"_s);
    }

    void parseRanksExactAndCondaForgeFirst()
    {
        const QByteArray json = R"({"data":{"packages":{"page":[
            {"name":"numpyro","summary":"Pyro on NumPy","channel":{"name":"conda-forge"},"latestVersion":{"version":"0.15"}},
            {"name":"numpy","summary":null,"channel":{"name":"emscripten-forge-4x"},"latestVersion":{"version":"2.5.4"}},
            {"name":"numpy","summary":"  The fundamental\n package ","channel":{"name":"conda-forge"},"latestVersion":{"version":"2.5.4"}}
        ]}}})";
        QString error;
        const auto packages = PrefixDev::parseResponse(json, u"numpy"_s, &error);
        QVERIFY(error.isEmpty());
        QCOMPARE(packages.size(), 3);
        QCOMPARE(packages[0].channel, u"conda-forge"_s);
        QCOMPARE(packages[0].name, u"numpy"_s);
        QCOMPARE(packages[0].summary, u"The fundamental package"_s);
        QCOMPARE(packages[1].channel, u"emscripten-forge-4x"_s);
        QVERIFY(packages[1].summary.isEmpty());
        QCOMPARE(packages[2].name, u"numpyro"_s);
    }

    void parseErrors()
    {
        QString error;
        QVERIFY(PrefixDev::parseResponse("not json", u"x"_s, &error).isEmpty());
        QVERIFY(!error.isEmpty());
        error.clear();
        PrefixDev::parseResponse(R"({"errors":[{"message":"boom"}]})", u"x"_s, &error);
        QCOMPARE(error, u"boom"_s);
    }

    void commandsAndUrl()
    {
        const PrefixDev::Package cf{u"numpy"_s, u"conda-forge"_s, {}, {}};
        QCOMPARE(cf.pageUrl().toString(), u"https://prefix.dev/channels/conda-forge/packages/numpy"_s);
        QCOMPARE(cf.pixiAddCommand(), u"pixi add numpy"_s);
        QCOMPARE(cf.pixiGlobalCommand(), u"pixi global install numpy"_s);
        const PrefixDev::Package gh{u"ripgrep"_s, u"github-releases"_s, {}, {}};
        QCOMPARE(gh.pixiAddCommand(), u"pixi workspace channel add https://prefix.dev/github-releases && pixi add ripgrep"_s);
        QCOMPARE(gh.pixiGlobalCommand(), u"pixi global install -c https://prefix.dev/github-releases ripgrep"_s);
    }
};

QTEST_GUILESS_MAIN(PrefixDevTest)

#include "prefixdevtest.moc"
