#include <QQmlExtensionPlugin>

class AsciiquariumPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

class AsciiquariumFakeItem : public QObject
{
    Q_OBJECT
public:
    // Empty type used to fool QML into thinking this plugin does something
    // more than just providing an image provider
    AsciiquariumFakeItem(QObject *parent = 0);
};
