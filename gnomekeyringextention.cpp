#include "gnomekeyringextention.h"

#include <QDebug>
#include <QCheckBox>

#include <gnome-keyring-1/gnome-keyring.h>

static const QString ActionEnableAutoLogin = "com.deepin.daemon.accounts.enable-auto-login";
static const QString ActionDisableAutoLogin = "com.deepin.daemon.accounts.disable-auto-login";
static const QString ActionEnableNopassLogin = "com.deepin.daemon.accounts.enable-nopass-login";
static const QString ActionDisableNopassLogin = "com.deepin.daemon.accounts.disable-nopass-login";

GnomeKeyringExtention::GnomeKeyringExtention(QObject *parent)
    : QObject(parent),
      m_proxy(nullptr),
      m_checkBtn(nullptr)
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

    ret << ActionEnableAutoLogin
        << ActionDisableAutoLogin
        << ActionEnableNopassLogin
        << ActionDisableNopassLogin;

    return ret;
}

QString GnomeKeyringExtention::description() const
{
    return "";
}

QButtonGroup *GnomeKeyringExtention::options()
{
    const QString actionID = m_proxy->actionID();

    if (m_checkBtn.isNull())
        m_checkBtn = new QCheckBox;

    if (actionID == ActionEnableAutoLogin || actionID == ActionEnableNopassLogin) {
        m_checkBtn.data()->setText(tr("Empty keyring password"));
    } else if (actionID == ActionDisableAutoLogin || actionID == ActionDisableNopassLogin) {
        m_checkBtn.data()->setText(tr("Restore keyring password"));
    }

    m_checkBtn.data()->setChecked(true);

    QButtonGroup *group = new QButtonGroup;
    group->addButton(m_checkBtn);

    return group;
}

void GnomeKeyringExtention::extendedDo()
{
    const QString actionID = m_proxy->actionID();
    const QString password = m_proxy->password();

    if (actionID == ActionEnableAutoLogin || actionID == ActionEnableNopassLogin) {
        if (!m_checkBtn.isNull() && m_checkBtn.data()->checkState() == Qt::Checked) {
            emptyKeyringPassword(password);
        }
    } else if (actionID == ActionDisableAutoLogin || actionID == ActionDisableNopassLogin) {
        if (!m_checkBtn.isNull() && m_checkBtn.data()->checkState() == Qt::Checked) {
            restoreKeyringPassword(password);
        }
    }
}

void GnomeKeyringExtention::emptyKeyringPassword(const QString &password)
{
    qDebug() << "empty keyring password";
    setKeyringPassword(password, "");
}

void GnomeKeyringExtention::restoreKeyringPassword(const QString &password)
{
    qDebug() << "restore keyring password";
    setKeyringPassword("", password);
}

void GnomeKeyringExtention::setKeyringPassword(const QString current, const QString newPass)
{

    char *defaultKeyring = new char[1024];

    GnomeKeyringResult result = gnome_keyring_get_default_keyring_sync(&defaultKeyring);
    if (result == GNOME_KEYRING_RESULT_OK || strcmp(defaultKeyring, "login")) {
        result = gnome_keyring_change_password_sync(defaultKeyring,
                                                    current.toStdString().c_str(),
                                                    newPass.toStdString().c_str());

        if (result == GNOME_KEYRING_RESULT_OK)
            qDebug() << "successfully change keyring password.";
        else
            qWarning() << "failed to change keyring password: " << result;

    } else {
        qWarning() << "failed to query default keyring, or maybe the default keyring is not login.";
    }

    delete(defaultKeyring);
}
