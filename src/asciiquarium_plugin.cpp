#include "asciiquarium_plugin.h"

#include <QtQml>
#include <QQmlEngine>
#include <QQuickImageProvider>
#include <QSize>
#include <QPixmap>
#include <QString>

class ColorImageProvider : public QQuickImageProvider
{
public:
    ColorImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Pixmap)
    {
    }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        QSize onepixel(1, 1);
        if (size) {
            *size = onepixel;
        }

        QPixmap px(size);
        px.fill(Qt::black);
        return px;
    }
};

AsciiquariumPlugin::AsciiquariumPlugin(QObject *parent, const QVariantList &args)
: Plasma::Applet(parent, args)
{
    // Add our image provider to the QmlEngine
    auto engine = qmlEngine(this);
    if (!engine) {
        return;
    }

    // Qt now owns the pointer
    engine->addImageProvider(QLatin1String("org.kde.plasma.asciiquarium"), new ColorImageProvider);
}

AsciiquariumPlugin::~AsciiquariumPlugin()
{
}

void AsciiquariumPlugin::init()
{
    Plasma::Applet::init();
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(asciiquarium_plugin, AsciiquariumPlugin, "metadata.json")

#include "asciiquarium_plugin.moc"
