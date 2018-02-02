#include "gnomekeyringextention.h"

#include <QDebug>
#include <QCheckBox>

#include <gnome-keyring-1/gnome-keyring.h>

static const char* LoginKeyring = "login";

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

    char *defaultKeyring = NULL;

    GnomeKeyringResult result = gnome_keyring_get_default_keyring_sync(&defaultKeyring);
    if (result != GNOME_KEYRING_RESULT_OK || strcmp(defaultKeyring, LoginKeyring) != 0) {
        qDebug() << "default keyring is not login keyring, create one.";

        result = gnome_keyring_create_sync(LoginKeyring, newPass.toStdString().c_str());
        if (result == GNOME_KEYRING_RESULT_OK)
            qDebug() << "successfully created login keyring";
        else
            qDebug() << "failed to create login keyring";


        result = gnome_keyring_set_default_keyring_sync(LoginKeyring);
        if (result == GNOME_KEYRING_RESULT_OK)
            qDebug() << "successfully set default keyring to login.";
        else
            qDebug() << "failed to set default keyring to login";
    } else {
        result = gnome_keyring_change_password_sync(defaultKeyring,
                                                    current.toStdString().c_str(),
                                                    newPass.toStdString().c_str());

        if (result == GNOME_KEYRING_RESULT_OK)
            qDebug() << "successfully change keyring password.";
        else
            qWarning() << "failed to change keyring password: " << result;

    }

    free(defaultKeyring);
}
