// SPDX-FileCopyrightText: 2017-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef GNOMEKEYRINGEXTENTION_H
#define GNOMEKEYRINGEXTENTION_H

#include <QObject>
#include <QButtonGroup>
#include <QCheckBox>
#include <QTranslator>

#include <dpa/agent-extension-proxy.h>
#include <dpa/agent-extension.h>

class  GnomeKeyringExtention : public QObject, dpa::AgentExtension
{
    Q_OBJECT
    Q_INTERFACES(dpa::AgentExtension)
    Q_PLUGIN_METADATA(IID AgentExtensionPluginIID FILE "dpa-ext-gnomekeyring.json")
public:
    explicit GnomeKeyringExtention(QObject * parent = nullptr);

    void initialize(dpa::AgentExtensionProxy *proxy) override;
    void finalize() override;

    QStringList interestedActions() const override;
    QString description() const override;

    QButtonGroup* options() override;

    void extendedDo() override;

private:
    dpa::AgentExtensionProxy *m_proxy;

    QPointer<QCheckBox> m_checkBtn;
    QPointer<QTranslator> m_translator;

    void emptyKeyringPassword(const QString &password);
    void restoreKeyringPassword(const QString &password);

    void setKeyringPassword(const QString current, const QString newPass);
};

#endif // GNOMEKEYRINGEXTENTION_H
