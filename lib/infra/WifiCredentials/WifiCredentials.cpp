// =============================================================================
// WifiCredentials.cpp — implementación NVS (Preferences) para ESP32.
// =============================================================================

#ifndef UNIT_TEST  // Arduino-only: no se compila en env:native.
#if defined(ARDUINO_ARCH_ESP32)

#include "WifiCredentials.h"
#include "Logger.h"
#include <Preferences.h>
#include <string.h>

namespace {
// Namespace NVS propio, separado de cualquier otro uso de Preferences. Máx 15
// caracteres (límite de NVS). Las claves también ≤ 15 caracteres.
constexpr const char* kNs       = "furbi_wifi";
constexpr const char* kKeySsid  = "ssid";
constexpr const char* kKeyPass  = "pass";
}  // namespace

namespace WifiCredentials {

bool load(char* ssid, size_t ssidCap, char* pass, size_t passCap) {
    if (ssid && ssidCap) ssid[0] = '\0';
    if (pass && passCap) pass[0] = '\0';

    Preferences prefs;
    // readonly=true: no crea el namespace si no existe (primer arranque).
    if (!prefs.begin(kNs, /*readOnly=*/true)) {
        return false;  // namespace inexistente → sin credenciales guardadas
    }

    // getString escribe como mucho cap-1 bytes + '\0'. Devuelve longitud real.
    size_t n = prefs.getString(kKeySsid, ssid, ssidCap);
    if (pass && passCap) prefs.getString(kKeyPass, pass, passCap);
    prefs.end();

    return n > 0 && ssid[0] != '\0';
}

bool save(const char* ssid, const char* pass) {
    if (!ssid || ssid[0] == '\0') {
        LOG_WARN("[wifi-cred] save ignorado: SSID vacio");
        return false;
    }
    if (!pass) pass = "";

    Preferences prefs;
    if (!prefs.begin(kNs, /*readOnly=*/false)) {
        LOG_ERROR("[wifi-cred] no se pudo abrir NVS para escritura");
        return false;
    }
    // putString devuelve el nº de bytes escritos. Password NUNCA se loguea.
    bool ok = prefs.putString(kKeySsid, ssid) > 0;
    prefs.putString(kKeyPass, pass);
    prefs.end();

    if (ok) LOG_INFO("[wifi-cred] credenciales guardadas en NVS para SSID=%s", ssid);
    else    LOG_ERROR("[wifi-cred] fallo escribiendo SSID en NVS");
    return ok;
}

void clear() {
    Preferences prefs;
    if (!prefs.begin(kNs, /*readOnly=*/false)) return;
    prefs.clear();
    prefs.end();
    LOG_INFO("[wifi-cred] credenciales borradas de NVS — arrancara en modo portal");
}

bool has() {
    char ssid[kSsidBufLen];
    return load(ssid, sizeof(ssid), nullptr, 0);
}

}  // namespace WifiCredentials

#endif  // ARDUINO_ARCH_ESP32
#endif  // UNIT_TEST
