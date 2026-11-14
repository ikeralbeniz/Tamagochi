#pragma once
// =============================================================================
// WifiCredentials — almacenamiento persistente de credenciales WiFi en NVS.
//
// Motivación (feat/wifi-ap-provisioning):
//   Hasta ahora las credenciales venían SOLO de secrets.h (compile-time). Para
//   poder configurar la red desde el portal AP (sin reflashear) hacen falta
//   credenciales que sobrevivan al reinicio PERO que NO vivan en el firmware.
//   NVS (Preferences) es el sitio canónico en ESP32: una partición dedicada,
//   independiente de LittleFS y de los slots OTA, que no se borra al actualizar
//   el firmware por OTA.
//
// SEGURIDAD (rules/security.md):
//   - La password se guarda en NVS en claro (igual que haría WiFi.begin con
//     persistent(true)); NVS no está cifrado por defecto en estas placas, pero
//     es memoria local no accesible por red. NUNCA se loguea (ver .cpp).
//   - Estas credenciales jamás se commitean (viven en flash del dispositivo).
//
// Solo ESP32: usa <Preferences.h> (arduino-esp32). En ESP8266/native este
// header no debe incluirse (WifiManager lo guarda tras #if ARDUINO_ARCH_ESP32).
// =============================================================================

#include <stddef.h>

namespace WifiCredentials {

// Límites del estándar WiFi: SSID hasta 32 bytes, PSK hasta 63 bytes. Los
// buffers del llamante deben incluir el '\0' final (33 y 64).
constexpr size_t kSsidBufLen = 33;
constexpr size_t kPassBufLen = 64;

// load — copia el SSID/password guardados en NVS a los buffers del llamante.
// Devuelve true solo si hay un SSID NO vacío almacenado. Si devuelve false,
// el contenido de los buffers queda como cadena vacía ("").
// ssidCap/passCap: capacidad de cada buffer (se trunca con seguridad).
bool load(char* ssid, size_t ssidCap, char* pass, size_t passCap);

// save — persiste SSID/password en NVS. Devuelve true si se escribió.
// Un ssid vacío se rechaza (devuelve false) — usar clear() para borrar.
bool save(const char* ssid, const char* pass);

// clear — borra las credenciales guardadas (vuelve al estado "sin configurar",
// que hace que el firmware arranque en modo portal AP la próxima vez).
void clear();

// has — true si hay un SSID no vacío guardado, sin copiar la password.
bool has();

}  // namespace WifiCredentials
