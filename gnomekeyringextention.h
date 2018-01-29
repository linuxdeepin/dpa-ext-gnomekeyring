#ifndef GNOMEKEYRINGEXTENTION_H
#define GNOMEKEYRINGEXTENTION_H

#include <QObject>

#include "../dde-polkit-agent/agent-extension-proxy.h"
#include "../dde-polkit-agent/agent-extension.h"

class  GnomeKeyringExtention : public QObject, dpa::AgentExtension
{
    Q_OBJECT
    Q_INTERFACES(dpa::AgentExtension)
    Q_PLUGIN_METADATA(IID AgentExtensionPluginIID FILE "dpa-ext-gnomekeyring.json")
public:
    explicit GnomeKeyringExtention(QObject * parent = 0);

    void initialize(dpa::AgentExtensionProxy *proxy) Q_DECL_OVERRIDE;
    void finalize() Q_DECL_OVERRIDE;

    QStringList interestedActions() const Q_DECL_OVERRIDE;
    QString description() const Q_DECL_OVERRIDE;

    void extendedDo() Q_DECL_OVERRIDE;

private:
    dpa::AgentExtensionProxy *m_proxy;
};

#endif // GNOMEKEYRINGEXTENTION_H
