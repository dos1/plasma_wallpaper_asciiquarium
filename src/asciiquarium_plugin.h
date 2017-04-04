#include <Plasma/Applet>

class __attribute__((visibility ("default"))) AsciiquariumPlugin : public Plasma::Applet
{
    Q_OBJECT

public:
    AsciiquariumPlugin(QObject *parent, const QVariantList &args);
    ~AsciiquariumPlugin() override;

    void init() override;
};
