// SPDX-License-Identifier: MIT
#include <QObject>
#include <QQmlExtensionPlugin>
#include <qqml.h>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusContext>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <cmath>

// One shared endpoint per plasmashell process, regardless of widget count.
class CursorBus : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.loop.Eyes.Cursor")
public:
    double x = 0, y = 0;
    bool connected = false;
    QTimer watchdog;
    QString kwinOwner;
    explicit CursorBus(QObject *parent) : QObject(parent) {
        auto bus = QDBusConnection::sessionBus();
        auto *watcher = new QDBusServiceWatcher("org.kde.KWin", bus,
            QDBusServiceWatcher::WatchForOwnerChange, this);
        connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged, this,
            [this](const QString &, const QString &, const QString &owner) {
                kwinOwner = owner; disconnectCursor();
            });
        if (bus.interface()) {
            QDBusReply<QString> reply = bus.interface()->serviceOwner("org.kde.KWin");
            if (reply.isValid()) kwinOwner = reply.value();
        }
        watchdog.setSingleShot(true);
        watchdog.setInterval(2500);
        connect(&watchdog, &QTimer::timeout, this, &CursorBus::disconnectCursor);
        // Refuse an occupied name rather than replacing another process.
        if (bus.registerService("org.loop.Eyes")) {
            if (!bus.registerObject("/org/loop/Eyes/Cursor", this, QDBusConnection::ExportScriptableSlots))
                bus.unregisterService("org.loop.Eyes");
        }
    }
    void disconnectCursor() {
        if (connected) { connected = false; emit changed(); }
    }
public slots:
    Q_SCRIPTABLE void Update(const QString &payload) {
        // Accept compositor-originated updates only; no commands or disk I/O.
        if (!calledFromDBus() || kwinOwner.isEmpty() || message().service() != kwinOwner)
            return;
        if (payload.size() > 256) return;
        const auto document = QJsonDocument::fromJson(payload.toUtf8());
        if (!document.isObject()) return;
        const auto p = document.object();
        if (!p.value("x").isDouble() || !p.value("y").isDouble()) return;
        const double nx = p.value("x").toDouble(), ny = p.value("y").toDouble();
        if (!std::isfinite(nx) || !std::isfinite(ny) || std::abs(nx)>1000000 || std::abs(ny)>1000000) return;
        const bool notify = !connected || nx != x || ny != y;
        x = nx; y = ny; connected = true;
        watchdog.start();
        if (notify) emit changed();
    }
signals:
    void changed();
};

class CursorTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(double x READ x NOTIFY changed)
    Q_PROPERTY(double y READ y NOTIFY changed)
    Q_PROPERTY(bool connected READ connected NOTIFY changed)
public:
    explicit CursorTracker(QObject *parent = nullptr) : QObject(parent) {
        static CursorBus *shared = new CursorBus(QCoreApplication::instance());
        m_bus = shared;
        connect(m_bus, &CursorBus::changed, this, &CursorTracker::changed);
    }
    double x() const { return m_bus->x; }
    double y() const { return m_bus->y; }
    bool connected() const { return m_bus->connected; }
signals:
    void changed();
private:
    CursorBus *m_bus;
};

class LoopEyesPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    void registerTypes(const char *uri) override {
        qmlRegisterType<CursorTracker>(uri, 1, 0, "CursorTracker");
    }
};
#include "plugin.moc"
