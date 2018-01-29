#include "gnomekeyringextention.h"

#include <QDebug>

GnomeKeyringExtention::GnomeKeyringExtention(QObject *parent)
    : QObject(parent),
      m_proxy(nullptr)
{
}

void GnomeKeyringExtention::initialize(dpa::AgentExtensionProxy *proxy)
{
    m_proxy = proxy;
}

void GnomeKeyringExtention::finalize()
{

}

QStringList GnomeKeyringExtention::interestedActions() const
{
    QStringList ret;
    ret << "test1" << "test2";

    return ret;
}

QString GnomeKeyringExtention::description() const
{
    return "xxxxxxxxxxxxxxxxxxxxxxx";
}

void GnomeKeyringExtention::extendedDo()
{
    const QString actionID = m_proxy->actionID();
    const QString username = m_proxy->username();
    const QString password = m_proxy->password();

    qDebug() << actionID << username << password << "---------------------";
}
