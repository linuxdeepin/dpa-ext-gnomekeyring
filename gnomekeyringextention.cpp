// 与 Qt 中的宏冲突，必须在 Qt 前 include
#include <libsecret/secret.h>
#include <gio/gio.h>

#include "gnomekeyringextention.h"

#include <QDebug>
#include <QCheckBox>

static const char *PasswordSecretValueContentType = "text/plain";
static const char *LoginKeyringPath = "/org/freedesktop/secrets/collection/login";

static const QString ActionEnableAutoLogin = "com.deepin.daemon.accounts.enable-auto-login";
static const QString ActionDisableAutoLogin = "com.deepin.daemon.accounts.disable-auto-login";
static const QString ActionEnableNopassLogin = "com.deepin.daemon.accounts.enable-nopass-login";
static const QString ActionDisableNopassLogin = "com.deepin.daemon.accounts.disable-nopass-login";
static const QString ActionEnrollFingerprint = "com.deepin.daemon.authenticate.Fingerprint.enroll";

GnomeKeyringExtention::GnomeKeyringExtention(QObject *parent)
    : QObject(parent),
      m_proxy(nullptr),
      m_checkBtn(nullptr)
{
}

void GnomeKeyringExtention::initialize(dpa::AgentExtensionProxy *proxy)
{
    const QString locale = QLocale::system().name();
    const QString filename = QString("/usr/share/dpa-ext-gnomekeyring/translations/dpa-ext-gnomekeyring_%1.qm").arg(locale);

    m_translator = new QTranslator;
    if (m_translator->load(filename)) {
        if (!qApp->installTranslator(m_translator)) {
            qWarning() << "failed to install translator of plugin gnome-keyring";
        } else {
            qDebug() << "installed translator of plugin gnome-keyring";
        }
    } else {
        qWarning() << "failed to load qm file: " << filename;
    }

    m_proxy = proxy;
}

void GnomeKeyringExtention::finalize()
{
    qApp->removeTranslator(m_translator);
    m_translator.data()->deleteLater();
}

QStringList GnomeKeyringExtention::interestedActions() const
{
    QStringList ret;

    ret << ActionEnableAutoLogin
        << ActionDisableAutoLogin
        << ActionEnableNopassLogin
        << ActionDisableNopassLogin
        << ActionEnrollFingerprint;

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

    if (actionID == ActionEnableAutoLogin ||
        actionID == ActionEnableNopassLogin ||
	actionID == ActionEnrollFingerprint) {
        m_checkBtn.data()->setText(tr("Empty keyring password"));
    } else if (actionID == ActionDisableAutoLogin || actionID == ActionDisableNopassLogin) {
        m_checkBtn.data()->setText(tr("Restore keyring password"));
    }

    m_checkBtn.data()->setChecked(true);

    QButtonGroup *group = new QButtonGroup;
    group->addButton(m_checkBtn);
    group->setExclusive(false);

    return group;
}

void GnomeKeyringExtention::extendedDo()
{
    const QString actionID = m_proxy->actionID();
    const QString password = m_proxy->password();

    if (actionID == ActionEnableAutoLogin ||
	actionID == ActionEnableNopassLogin ||
	actionID == ActionEnrollFingerprint) {
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
    GError *err = nullptr;
    SecretService *service = nullptr;
    GDBusConnection *bus = nullptr;
    SecretValue *currentValue = nullptr;
    SecretValue *newPassValue = nullptr;

    do {
        service = secret_service_get_sync(SECRET_SERVICE_OPEN_SESSION, nullptr, &err);
        if (service == nullptr) {
            qWarning() << "failed to get secret service:" << err->message;
            break;
        }

        SecretCollection *collection = secret_collection_for_alias_sync(service,
                                                                        SECRET_COLLECTION_DEFAULT,
                                                                        SECRET_COLLECTION_NONE,
                                                                        NULL,
                                                                        &err);
        if (err != nullptr) {
            qWarning() << "failed to get default secret collection:" << err->message;
            break;
        }
        if (collection == nullptr) {
            qDebug() << "default secret collection not exists";
            break;
        }

        auto currentLatin1 = current.toLatin1();
        currentValue = secret_value_new(currentLatin1.data(), currentLatin1.length(), PasswordSecretValueContentType);

        auto newPassLatin1 = newPass.toLatin1();
        newPassValue = secret_value_new(newPassLatin1.data(), newPassLatin1.length(), PasswordSecretValueContentType);

        bus = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &err);
        if (bus == nullptr) {
             qWarning() << "failed to get session bus:" << err->message;
             break;
        }

        g_dbus_connection_call_sync(bus,
                                    "org.gnome.keyring",
                                    "/org/freedesktop/secrets",
                                    "org.gnome.keyring.InternalUnsupportedGuiltRiddenInterface",
                                    "ChangeWithMasterPassword",
                                    g_variant_new("(o@(oayays)@(oayays))",
                                                  LoginKeyringPath,
                                                  secret_service_encode_dbus_secret(service, currentValue),
                                                  secret_service_encode_dbus_secret(service, newPassValue)),
                                    nullptr,
                                    G_DBUS_CALL_FLAGS_NONE,
                                    G_MAXINT,
                                    nullptr,
                                    &err);
         if (err != nullptr) {
             qWarning() << "failed to change keyring password:" << err->message;
             break;
         }
    } while (false);

    if (err != nullptr) g_error_free(err);
    if (service != nullptr) g_object_unref(service);
    if (bus != nullptr) g_object_unref(bus);
    if (currentValue != nullptr) g_object_unref(bus);
    if (newPassValue != nullptr) g_object_unref(bus);
}
