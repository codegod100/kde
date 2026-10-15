/*
    SPDX-FileCopyrightText: 2026 whitelist-tray contributors

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

#include <KConfigGroup>
#include <KConfigLoader>
#include <KSharedConfig>

#include "../src/systemtraymodel.h"
#include "../src/systemtraysettings.h"

using Status = Plasma::Types::ItemStatus;

// Exposes the protected visibility rule for testing.
class TestModel : public BaseModel
{
public:
    using BaseModel::BaseModel;
    using BaseModel::calculateEffectiveStatus;
    int rowCount(const QModelIndex & = {}) const override
    {
        return 0;
    }
    QVariant data(const QModelIndex &, int) const override
    {
        return {};
    }
};

class WhitelistTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEffectiveStatus_data();
    void testEffectiveStatus();
};

void WhitelistTest::testEffectiveStatus_data()
{
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<int>("status");
    QTest::addColumn<bool>("showNeedsAttention");
    QTest::addColumn<bool>("showAll");
    QTest::addColumn<int>("expected");

    // Not whitelisted: always tucked away, whatever the item reports
    QTest::newRow("unlisted active") << QStringLiteral("other") << int(Status::ActiveStatus) << false << false << int(Status::PassiveStatus);
    QTest::newRow("unlisted passive") << QStringLiteral("other") << int(Status::PassiveStatus) << false << false << int(Status::PassiveStatus);
    QTest::newRow("unlisted attention") << QStringLiteral("other") << int(Status::NeedsAttentionStatus) << false << false << int(Status::PassiveStatus);
    QTest::newRow("unlisted attention, option on") << QStringLiteral("other") << int(Status::NeedsAttentionStatus) << true << false << int(Status::ActiveStatus);
    QTest::newRow("unlisted hidden") << QStringLiteral("other") << int(Status::HiddenStatus) << false << false << int(Status::HiddenStatus);
    QTest::newRow("unlisted disabled sni") << QStringLiteral("disabled") << int(Status::ActiveStatus) << false << false << int(Status::HiddenStatus);

    // Always shown
    QTest::newRow("shown passive") << QStringLiteral("always") << int(Status::PassiveStatus) << false << false << int(Status::ActiveStatus);
    QTest::newRow("shown hidden") << QStringLiteral("always") << int(Status::HiddenStatus) << false << false << int(Status::ActiveStatus);

    // Shown when relevant
    QTest::newRow("allowed active") << QStringLiteral("relevant") << int(Status::ActiveStatus) << false << false << int(Status::ActiveStatus);
    QTest::newRow("allowed attention") << QStringLiteral("relevant") << int(Status::NeedsAttentionStatus) << false << false << int(Status::ActiveStatus);
    QTest::newRow("allowed passive") << QStringLiteral("relevant") << int(Status::PassiveStatus) << false << false << int(Status::PassiveStatus);
    QTest::newRow("allowed hidden") << QStringLiteral("relevant") << int(Status::HiddenStatus) << false << false << int(Status::HiddenStatus);

    // Show all overrides the whitelist
    QTest::newRow("show all") << QStringLiteral("other") << int(Status::PassiveStatus) << false << true << int(Status::ActiveStatus);
}

void WhitelistTest::testEffectiveStatus()
{
    QFETCH(QString, itemId);
    QFETCH(int, status);
    QFETCH(bool, showNeedsAttention);
    QFETCH(bool, showAll);
    QFETCH(int, expected);

    QTemporaryDir dir;
    const QString configPath = dir.filePath(QStringLiteral("traysettingsrc"));
    {
        auto config = KSharedConfig::openConfig(configPath, KConfig::SimpleConfig);
        KConfigGroup general = config->group(QStringLiteral("General"));
        general.writeEntry("shownItems", QStringList{QStringLiteral("always")});
        general.writeEntry("allowedItems", QStringList{QStringLiteral("relevant")});
        general.writeEntry("disabledStatusNotifiers", QStringList{QStringLiteral("disabled")});
        general.writeEntry("showNeedsAttention", showNeedsAttention);
        general.writeEntry("showAllItems", showAll);
        config->sync();
    }

    QFile schema(QStringLiteral(SCHEMA_FILE));
    KConfigLoader loader(configPath, &schema);
    SystemTraySettings settings(&loader);
    TestModel model(&settings);

    QCOMPARE(int(model.calculateEffectiveStatus(true, Status(status), itemId)), expected);
    // Items that cannot render are always hidden
    QCOMPARE(int(model.calculateEffectiveStatus(false, Status(status), itemId)), int(Status::HiddenStatus));
}

QTEST_GUILESS_MAIN(WhitelistTest)

#include "whitelisttest.moc"
