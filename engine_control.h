/**
 * @file engine_control.h
 * @brief Otomotiv Motor Kontrol Sistemi - Ana Modül
 * @details Motorun RPM, yakıt enjeksiyonu, tutuşturma zamanlaması ve diğer parametreleri kontrol eden 40+ fonksiyonlu sistem
 * @version 1.0
 */

#ifndef ENGINE_CONTROL_H
#define ENGINE_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
   VERİ TİPLERİ VE YAPILAR
   ============================================================================ */

/* Forward declarations */
typedef struct EngineControl EngineControl_t;
typedef struct EngineConfig EngineConfig_t;
typedef struct PerformanceMetrics PerformanceMetrics_t;
typedef struct EngineCallback EngineCallback_t;

/* Callback tipleri */
typedef void (*EngineStateChangeCallback_t)(EngineState_t old_state, EngineState_t new_state);
typedef void (*EngineErrorCallback_t)(uint16_t error_code, const char* error_message);
typedef bool (*EngineValidationCallback_t)(uint16_t rpm, int16_t temp, uint16_t fuel_pressure);
typedef int (*EngineFilterFunction_t)(const void* element, void* context);

typedef enum {
    IDLE_STATE = 0,
    RUNNING_STATE = 1,
    ACCELERATING_STATE = 2,
    DECELERATING_STATE = 3,
    SHUTDOWN_STATE = 4,
    ERROR_STATE = 5
} EngineState_t;

typedef enum {
    RPM_OPTIMAL = 0,
    RPM_LOW = 1,
    RPM_HIGH = 2,
    RPM_CRITICAL = 3
} RPMStatus_t;

typedef enum {
    FUEL_INJECTION_NORMAL = 0,
    FUEL_INJECTION_RICH = 1,
    FUEL_INJECTION_LEAN = 2,
    FUEL_INJECTION_CUTOFF = 3
} FuelInjectionMode_t;

typedef enum {
    IGNITION_TIMING_OPTIMAL = 0,
    IGNITION_TIMING_ADVANCED = 1,
    IGNITION_TIMING_RETARDED = 2
} IgnitionTimingMode_t;

typedef struct {
    uint16_t current_rpm;
    int16_t temperature_celsius;
    uint16_t throttle_position;
    uint16_t fuel_pressure;
    uint8_t oxygen_level;
    uint8_t air_intake_percentage;
    bool is_running;
    EngineState_t state;
} EngineStatus_t;

typedef struct {
    uint16_t target_rpm;
    uint8_t fuel_injection_percentage;
    int8_t ignition_advance_degrees;
    uint16_t boost_pressure;
    uint8_t cooling_fan_speed;
    bool turbo_enabled;
    FuelInjectionMode_t fuel_mode;
    IgnitionTimingMode_t ignition_mode;
} EngineCommand_t;

typedef struct {
    uint16_t error_code;
    char error_message[128];
    uint32_t timestamp;
    bool is_critical;
    uint8_t recovery_attempts;
} ErrorLog_t;

typedef struct {
    uint32_t total_errors;
    uint32_t critical_errors;
    uint32_t last_error_code;
    uint64_t total_runtime_seconds;
    uint16_t max_rpm_recorded;
    int16_t min_temperature_recorded;
    int16_t max_temperature_recorded;
} DiagnosticData_t;

typedef enum {
    RESULT_OK = 0,
    RESULT_ERROR = -1,
    RESULT_INVALID_PARAM = -2,
    RESULT_TIMEOUT = -3,
    RESULT_BUSY = -4
} EngineResult_t;

typedef struct {
    uint16_t rpm;
    int16_t temperature;
    uint16_t fuel_pressure;
    uint8_t throttle;
    uint8_t boost_pressure;
} RawSensorData_t;

typedef struct {
    float efficiency;
    uint8_t performance_score;
    uint8_t health_status;
    uint32_t runtime_hours;
    uint16_t remaining_fuel_percent;
} PerformanceMetrics_t;

typedef struct {
    uint16_t max_rpm;
    int16_t max_temp;
    uint16_t max_fuel_pressure;
    uint8_t max_boost;
    EngineValidationCallback_t validator;
} EngineConfig_t;

typedef struct {
    EngineStateChangeCallback_t on_state_change;
    EngineErrorCallback_t on_error;
} EngineCallback_t;

/* ============================================================================
   BAŞLANGIC VE KAPANMA FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Motor sistemi başlatıldığında çağrılır
 * @param initial_rpm İlk RPM seviyesi (500-1000)
 * @return true başarılı, false başarısız
 */
bool engine_initialize(uint16_t initial_rpm);

/**
 * @brief Motor sistemini güvenli şekilde kapatan fonksiyon
 * @return true başarılı, false başarısız
 */
bool engine_shutdown(void);

/**
 * @brief Motor durumunu sıfırladığında çağrılır
 * @return true başarılı, false başarısız
 */
bool engine_reset_state(void);

/**
 * @brief Acil durum durdurma yapan fonksiyon
 * @return true başarılı, false başarısız
 */
bool engine_emergency_stop(void);

/**
 * @brief Motor mod değiştirme (normal, ekonami, sport vs)
 * @param mode Mod numarası (0-3)
 * @return true başarılı, false başarısız
 */
bool engine_set_mode(uint8_t mode);

/* ============================================================================
   RPM KONTROL FONKSİYONLARI (8 fonksiyon)
   ============================================================================ */

/**
 * @brief Hedef RPM değerini ayarlar
 * @param target_rpm Hedef RPM (500-8000)
 * @return true başarılı, false başarısız
 */
bool engine_set_target_rpm(uint16_t target_rpm);

/**
 * @brief Mevcut RPM değerini döndürür
 * @return Mevcut RPM değeri
 */
uint16_t engine_get_current_rpm(void);

/**
 * @brief RPM artırma komutu
 * @param increment_percentage Artış yüzdesi (1-100)
 * @return Yeni RPM değeri
 */
uint16_t engine_increase_rpm(uint8_t increment_percentage);

/**
 * @brief RPM azaltma komutu
 * @param decrement_percentage Azalış yüzdesi (1-100)
 * @return Yeni RPM değeri
 */
uint16_t engine_decrease_rpm(uint8_t decrement_percentage);

/**
 * @brief RPM limitini ayarlar
 * @param max_rpm Maksimum RPM (5000-8000)
 * @return true başarılı, false başarısız
 */
bool engine_set_rpm_limit(uint16_t max_rpm);

/**
 * @brief RPM hızlandırma ivmesini ayarlar
 * @param acceleration_rate İvme oranı (1-100)
 * @return true başarılı, false başarısız
 */
bool engine_set_rpm_acceleration_rate(uint8_t acceleration_rate);

/**
 * @brief RPM yavaşlatma ivmesini ayarlar
 * @param deceleration_rate Yavaşlatma oranı (1-100)
 * @return true başarılı, false başarısız
 */
bool engine_set_rpm_deceleration_rate(uint8_t deceleration_rate);

/**
 * @brief RPM stabilitesi kontrolü
 * @return true stabil, false stabil değil
 */
bool engine_check_rpm_stability(void);

/* ============================================================================
   YAKIT ENJEKSİYONU KONTROL FONKSİYONLARI (7 fonksiyon)
   ============================================================================ */

/**
 * @brief Yakıt enjeksiyonu modunu ayarlar
 * @param mode Enjeksiyonu modu (0-3)
 * @return true başarılı, false başarısız
 */
bool engine_set_fuel_injection_mode(uint8_t mode);

/**
 * @brief Yakıt enjeksiyonu miktarını ayarlar (yüzde)
 * @param percentage Enjeksiyonu miktarı yüzdesi (10-100)
 * @return true başarılı, false başarısız
 */
bool engine_set_fuel_injection_percentage(uint8_t percentage);

/**
 * @brief Yakıt basıncını ayarlar
 * @param pressure_kpa Basınç kPa cinsinden (200-600)
 * @return true başarılı, false başarısız
 */
bool engine_set_fuel_pressure(uint16_t pressure_kpa);

/**
 * @brief Mevcut yakıt basıncını döndürür
 * @return Yakıt basıncı kPa cinsinden
 */
uint16_t engine_get_fuel_pressure(void);

/**
 * @brief Yakıt enjeksiyonu zamanlaması ayarları
 * @param timing_degrees Zamanlaması derece cinsinden (-10 - +10)
 * @return true başarılı, false başarısız
 */
bool engine_set_fuel_injection_timing(int8_t timing_degrees);

/**
 * @brief Yakıt tüketimi hesaplama
 * @return Saatlik yakıt tüketimi (L/h)
 */
float engine_calculate_fuel_consumption(void);

/**
 * @brief Yakıt durumu kontrolü
 * @return true sağlıklı, false sorun var
 */
bool engine_check_fuel_system_health(void);

/* ============================================================================
   MOTOR SICAKLIĞI KONTROL FONKSİYONLARI (6 fonksiyon)
   ============================================================================ */

/**
 * @brief Mevcut motor sıcaklığını döndürür
 * @return Motor sıcaklığı Celsius cinsinden
 */
int16_t engine_get_temperature(void);

/**
 * @brief Maksimum sıcaklık limitini ayarlar
 * @param max_temp_celsius Maksimum sıcaklık derecesi (-40 - 150)
 * @return true başarılı, false başarısız
 */
bool engine_set_max_temperature_limit(int16_t max_temp_celsius);

/**
 * @brief Soğutma fanı hızını ayarlar
 * @param fan_speed Fanı hızı yüzdesi (0-100)
 * @return true başarılı, false başarısız
 */
bool engine_set_cooling_fan_speed(uint8_t fan_speed);

/**
 * @brief Motor sıcaklığına dayanarak otomatik soğutma kontrolü
 * @return true başarılı, false başarısız
 */
bool engine_auto_cooling_control(void);

/**
 * @brief Sıcaklık sensörü kalibrasyonu
 * @param reference_temp Referans sıcaklık değeri (-40 - 150)
 * @return true başarılı, false başarısız
 */
bool engine_calibrate_temperature_sensor(int16_t reference_temp);

/**
 * @brief Sıcaklık aralığında uyarı kontrolü
 * @return true normal, false uyarı durumunda
 */
bool engine_check_temperature_warning(void);

/* ============================================================================
   TURBO KONTROL FONKSİYONLARI (4 fonksiyon)
   ============================================================================ */

/**
 * @brief Turbo sistemi etkinleştirme/devre dışı bırakma
 * @param enable true etkinleştir, false devre dışı bırak
 * @return true başarılı, false başarısız
 */
bool engine_set_turbo_enabled(bool enable);

/**
 * @brief Turbo boost basıncını ayarlar
 * @param boost_pressure_kpa Boost basıncı kPa cinsinden (0-300)
 * @return true başarılı, false başarısız
 */
bool engine_set_turbo_boost_pressure(uint16_t boost_pressure_kpa);

/**
 * @brief Mevcut turbo boost basıncını döndürür
 * @return Turbo boost basıncı kPa cinsinden
 */
uint16_t engine_get_turbo_boost_pressure(void);

/**
 * @brief Turbo sistem sağlık kontrolü
 * @return true sağlıklı, false hatalı
 */
bool engine_check_turbo_system_health(void);

/* ============================================================================
   TUTUŞTURMAİGNİTION) KONTROL FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Tutuşturma modunu ayarlar
 * @param mode Tutuşturma modu (0-2)
 * @return true başarılı, false başarısız
 */
bool engine_set_ignition_mode(uint8_t mode);

/**
 * @brief Tutuşturma zamanlamasını ayarlar
 * @param advance_degrees Zamanlaması derece cinsinden (-15 - +15)
 * @return true başarılı, false başarısız
 */
bool engine_set_ignition_timing(int8_t advance_degrees);

/**
 * @brief Tutuşturma coil güçünü ayarlar
 * @param coil_power Coil gücü yüzdesi (50-100)
 * @return true başarılı, false başarısız
 */
bool engine_set_ignition_coil_power(uint8_t coil_power);

/**
 * @brief Spark plug durumu kontrolü
 * @return true sağlıklı, false değiştirilmesi gerekli
 */
bool engine_check_spark_plug_health(void);

/**
 * @brief Tutuşturma sistemi sağlık kontrolü
 * @return true sağlıklı, false hatalı
 */
bool engine_check_ignition_system_health(void);

/* ============================================================================
   MOTOR DURUMU VE SENSÖR FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Mevcut motor durumunu döndürür
 * @param status Motor durumunu içerecek yapı
 * @return true başarılı, false başarısız
 */
bool engine_get_status(EngineStatus_t *status);

/**
 * @brief Motor komutlarını uygulamak için fonksiyon
 * @param command Uygulanacak motor komutları
 * @return true başarılı, false başarısız
 */
bool engine_apply_command(const EngineCommand_t *command);

/**
 * @brief Hava alımı yüzdesini ayarlar
 * @param air_intake_percentage Hava alımı yüzdesi (10-100)
 * @return true başarılı, false başarısız
 */
bool engine_set_air_intake_percentage(uint8_t air_intake_percentage);

/**
 * @brief Açısı (throttle) konumunu ayarlar
 * @param throttle_position Throttle konumu yüzdesi (0-100)
 * @return true başarılı, false başarısız
 */
bool engine_set_throttle_position(uint16_t throttle_position);

/**
 * @brief Oksijen seviyesini ölçer
 * @return Oksijen seviyesi yüzdesi (0-100)
 */
uint8_t engine_get_oxygen_level(void);

/* ============================================================================
   HATA YÖNETİMİ VE RECOVERY FONKSİYONLARI (4 fonksiyon)
   ============================================================================ */

/**
 * @brief Son hatayı raporlayan fonksiyon
 * @param error_log Hata bilgilerini içerecek yapı
 * @return true hata var, false hata yok
 */
bool engine_get_last_error(ErrorLog_t *error_log);

/**
 * @brief Hata kodu temizleme
 * @param error_code Temizlenecek hata kodu
 * @return true başarılı, false başarısız
 */
bool engine_clear_error_code(uint16_t error_code);

/**
 * @brief Motorun kurtarma işlemini başlatan fonksiyon
 * @return true başarılı, false başarısız
 */
bool engine_attempt_recovery(void);

/**
 * @brief Diagnostik verileri döndürür
 * @param diag_data Diagnostik verilerini içerecek yapı
 * @return true başarılı, false başarısız
 */
bool engine_get_diagnostic_data(DiagnosticData_t *diag_data);

/* ============================================================================
   İLERİ SEVİYE FONKSİYONLAR (CALLBACK & POINTER)
   ============================================================================ */

/**
 * @brief Motor için callback'leri kaydeden fonksiyon
 * @param callbacks Callback yapısına işaretçi
 * @return Kayıt sonucu (RESULT_OK, RESULT_INVALID_PARAM)
 */
EngineResult_t engine_register_callbacks(const EngineCallback_t *callbacks);

/**
 * @brief Motor konfigürasyonunu ayarlayan fonksiyon
 * @param config Yapılandırma parametreleri (pointer)
 * @return Başarılı true, başarısız false
 */
bool engine_set_configuration(const EngineConfig_t *config);

/**
 * @brief Ham sensör verilerini işleyen fonksiyon
 * @param raw_data Ham sensör verileri yapısına işaretçi
 * @param output İşlenmiş veriler (pointer)
 * @return İşlem sonucu (RESULT_OK, RESULT_ERROR)
 */
EngineResult_t engine_process_raw_sensor_data(const RawSensorData_t *raw_data, 
                                               PerformanceMetrics_t *output);

/**
 * @brief Performans metriklerini hesaplayan fonksiyon
 * @param out_metrics Hesaplanan metrikler (pointer)
 * @return Başarılı true, başarısız false
 */
bool engine_calculate_performance_metrics(PerformanceMetrics_t *out_metrics);

/**
 * @brief Batch RPM ayarlama (döngü işlemesi)
 * @param rpm_values RPM değerleri dizisi
 * @param count Dizi eleman sayısı
 * @param filter Filtreleme fonksiyonu (NULL olabilir)
 * @return İşlenen eleman sayısı
 */
uint16_t engine_batch_set_rpm_values(const uint16_t *rpm_values, 
                                     uint16_t count,
                                     EngineFilterFunction_t filter);

/**
 * @brief Hata callback'ini tetikleyen iç fonksiyon
 * @param error_code Hata kodu
 * @param message Hata mesajı
 * @return Başarılı true, başarısız false
 */
bool engine_trigger_error_callback(uint16_t error_code, const char *message);

/**
 * @brief Durum değişikliği callback'ini tetikleyen iç fonksiyon
 * @param new_state Yeni motor durumu
 * @return Başarılı true, başarısız false
 */
bool engine_trigger_state_change_callback(EngineState_t new_state);

/**
 * @brief Özel validasyon fonksiyonu çalıştıran fonksiyon
 * @param rpm RPM değeri
 * @param temp Sıcaklık değeri
 * @param fuel_pressure Yakıt basıncı
 * @return Validasyon sonucu (custom validator varsa)
 */
bool engine_run_custom_validation(uint16_t rpm, int16_t temp, uint16_t fuel_pressure);

/**
 * @brief Motor veri yapısına işaretçi döndüren fonksiyon
 * @return Motor kontrolü yapısına işaretçi (NULL başarısız)
 */
EngineControl_t* engine_get_control_structure(void);

/**
 * @brief Sensor dizisini işleyen işlev
 * @param sensors Sensör veri dizisi
 * @param size Dizi boyutu
 * @param callback Veri işleme callback'i
 * @return İşlenen sensör sayısı
 */
uint8_t engine_process_sensor_array(RawSensorData_t *sensors, 
                                    uint8_t size,
                                    EngineFilterFunction_t callback);

/**
 * @brief Konfigürasyon yapısını kopyalayan fonksiyon
 * @param dest Hedef yapı işaretçisi
 * @param src Kaynak yapı işaretçisi
 * @param size Kopyalanacak bayt sayısı
 * @return Kopyalanan bayt sayısı
 */
uint16_t engine_copy_configuration(void *dest, const void *src, uint16_t size);

/**
 * @brief Hata günlüğünü döndüren fonksiyon
 * @return Hata günlüğü yapısına işaretçi
 */
const ErrorLog_t* engine_get_error_log_pointer(void);

/**
 * @brief Çoklu error kodlarını bir seferde temizleyen fonksiyon
 * @param error_codes Temizlenecek hata kodları dizisi
 * @param count Dizi eleman sayısı
 * @return Temizlenen kod sayısı
 */
uint8_t engine_clear_multiple_error_codes(const uint16_t *error_codes, uint8_t count);

/**
 * @brief Motor konfigürasyonunu döndüren fonksiyon
 * @param out_config Konfigürasyon yapısına işaretçi (çıktı)
 * @return Başarılı true, başarısız false
 */
bool engine_get_current_configuration(EngineConfig_t *out_config);

/* ============================================================================
   UYUMLULUK VE STANDART FONKSİYONLARI
   ============================================================================ */

/**
 * @brief Motoru belirli bir saniye için çalıştıran test fonksiyonu
 * @param seconds Çalışma süresi saniye cinsinden (1-3600)
 * @return Tamamlanan gerçek çalışma süresi saniye cinsinden
 */
uint32_t engine_run_for_seconds(uint32_t seconds);

/**
 * @brief Sistem versiyon bilgisini döndürür
 * @return Sistem versiyon numarası
 */
uint32_t engine_get_system_version(void);

 /**
  * @brief Sistem bilgisini yapı olarak döndüren fonksiyon
  * @param out_info Sistem bilgileri yapısına işaretçi
  * @return Başarılı true, başarısız false
  */
 bool engine_get_system_info(struct EngineSystemInfo *out_info);

/* ============================================================================
   İLERİ STATE MACHINE VE KONTROL AKIŞI FONKSİYONLARI
   ============================================================================ */

/**
 * @brief Motor durumuna bağlı olarak kendini ayarlayan STATE MACHINE
 * @return Yeni motor durumu
 */
EngineState_t engine_auto_adjust_by_state(void);

/**
 * @brief Tüm sensörleri döngüsel olarak okuyup işleyen NESTED LOOP
 * @return İşlenen sensör sayısı
 */
uint8_t engine_process_all_sensors_loop(void);

/**
 * @brief RPM seviyesine göre kapsamlı ayarlamalar yapan IF-ELSE CHAIN
 * @param rpm_level RPM seviyesi (0-100%)
 * @return Başarılı true, başarısız false
 */
bool engine_adjust_by_rpm_level(uint8_t rpm_level);

/**
 * @brief Sıcaklık seviyeleri için SWITCH-CASE kontrolü (7 farklı durum)
 * @param temp Mevcut sıcaklık
 * @return Önerilen eylem
 */
uint8_t engine_handle_temperature_levels(int16_t temp);

/**
 * @brief Hata durumu ve recovery STATE MACHINE (kompleks mantık)
 * @return Sonraki beklenen durum
 */
EngineState_t engine_handle_error_recovery_state(void);

/**
 * @brief Ardışık performans optimizasyonu (NESTED LOOP: 3x4x3 = 36 iterasyon)
 * @param iterations Optimizasyon iterasyonu sayısı
 * @return Toplam optimizasyon skoru
 */
uint16_t engine_sequential_performance_optimization(uint8_t iterations);

/**
 * @brief Dinamik throttle kontrolü (RPM ve sıcaklığa göre çok seviyeli)
 * @return Yeni throttle pozisyonu
 */
uint16_t engine_dynamic_throttle_control(void);

/**
 * @brief Turbo boost adaptif kontrolü (state-based kompleks mantık)
 * @return Yeni boost basıncı
 */
uint16_t engine_adaptive_turbo_control(void);

/**
 * @brief Yakıt enjeksiyonu STATE MACHINE (4 modlu geçiş sistemi)
 * @return Enjeksiyonu modunun doğrulanması
 */
bool engine_fuel_injection_state_machine(void);

/**
 * @brief Tüm limitleri kontrol eden güvenlik fonksiyonu (5+ seviyeli kontrol)
 * @return Güvenli çalışma durumu
 */
bool engine_check_all_safety_limits(void);

 #endif /* ENGINE_CONTROL_H */

