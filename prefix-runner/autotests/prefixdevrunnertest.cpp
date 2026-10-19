// SPDX-License-Identifier: MIT
#include <KRunner/AbstractRunnerTest>
#include <KRunner/QueryMatch>

#include <QTest>

using namespace Qt::StringLiterals;

class PrefixDevRunnerTest : public KRunner::AbstractRunnerTest
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        initProperties();
    }

    void findsNumpy()
    {
        const QList<KRunner::QueryMatch> matches = launchQuery(u"pixi numpy"_s);
        QVERIFY(!matches.isEmpty());
        QCOMPARE(matches.first().text(), u"numpy · conda-forge"_s);
        QVERIFY(matches.first().subtext().startsWith(u"conda-forge"_s));
        QCOMPARE(matches.first().urls().first().toString(), u"https://prefix.dev/channels/conda-forge/packages/numpy"_s);
    }

    void ignoresOtherQueries()
    {
        QVERIFY(launchQuery(u"numpy"_s).isEmpty());
    }
};

QTEST_MAIN(PrefixDevRunnerTest)

#include "prefixdevrunnertest.moc"
