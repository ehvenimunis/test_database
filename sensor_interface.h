/**
 * @file sensor_interface.h
 * @brief Sensör Arayüzü Modülü - Motor Kontrol Sisteminin Bağımlılığı
 * @details Tüm sensörlerin okuma/yazma işlemlerinin arayüzü
 * @version 1.0
 */

#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
   SENSOR TİPLERİ VE YAPILARI
   ============================================================================ */

typedef enum {
    SENSOR_STATUS_OK = 0,
    SENSOR_STATUS_ERROR = 1,
    SENSOR_STATUS_NOT_READY = 2,
    SENSOR_STATUS_CALIBRATION_NEEDED = 3
} SensorStatus_t;

typedef enum {
    SENSOR_RESULT_OK = 0,
    SENSOR_RESULT_ERROR = -1,
    SENSOR_RESULT_OUT_OF_RANGE = -2,
    SENSOR_RESULT_CALIBRATION_FAILED = -3
} SensorResult_t;

typedef struct {
    uint32_t timestamp;
    uint16_t sensor_id;
    int32_t raw_value;
    int32_t calibrated_value;
    uint8_t quality;
} SensorDataPoint_t;

typedef struct {
    uint16_t rpm_value;
    int16_t temperature_value;
    uint16_t fuel_pressure_value;
} SensorBatch_t;

typedef struct {
    uint16_t rpm_value;
    int16_t temperature_value;
    uint16_t fuel_pressure_value;
    uint8_t oxygen_level;
    uint16_t boost_pressure;
    bool turbo_active;
} SensorReadings_t;

/* ============================================================================
   BAŞLANGIC VE KAPANMA FONKSİYONLARI (3 fonksiyon)
   ============================================================================ */

/**
 * @brief Sensör sistemini başlatır
 * @return true başarılı, false başarısız
 */
bool sensor_initialize(void);

/**
 * @brief Sensör sistemini kapatır
 * @return true başarılı, false başarısız
 */
bool sensor_shutdown(void);

/**
 * @brief Tüm sensörleri kalibre eder
 * @return true başarılı, false başarısız
 */
bool sensor_calibrate_all(void);

/* ============================================================================
   RPM SENSÖRÜ FONKSİYONLARI (4 fonksiyon)
   ============================================================================ */

/**
 * @brief RPM sensöründen değer okur
 * @return Mevcut RPM değeri
 */
uint16_t sensor_read_rpm_sensor(void);

/**
 * @brief RPM komutunu sensöre gönderir
 * @param rpm_value Hedef RPM değeri
 * @return true başarılı, false başarısız
 */
bool sensor_write_rpm_command(uint16_t rpm_value);

/**
 * @brief RPM sensörü durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_rpm_sensor_status(void);

/**
 * @brief RPM sensörünü kalibre eder
 * @param reference_rpm Referans RPM değeri
 * @return true başarılı, false başarısız
 */
bool sensor_calibrate_rpm_sensor(uint16_t reference_rpm);

/* ============================================================================
   SICAKLIK SENSÖRÜ FONKSİYONLARI (4 fonksiyon)
   ============================================================================ */

/**
 * @brief Sıcaklık sensöründen değer okur
 * @return Mevcut sıcaklık Celsius cinsinden
 */
int16_t sensor_read_temperature_sensor(void);

/**
 * @brief Sıcaklık sensörü durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_temperature_sensor_status(void);

/**
 * @brief Sıcaklık sensörünü kalibre eder
 * @param reference_temp Referans sıcaklık değeri
 * @return true başarılı, false başarısız
 */
bool sensor_calibrate_temperature_sensor(int16_t reference_temp);

/**
 * @brief Sıcaklık limitini ayarlar
 * @param min_temp Minimum sıcaklık
 * @param max_temp Maksimum sıcaklık
 * @return true başarılı, false başarısız
 */
bool sensor_set_temperature_limits(int16_t min_temp, int16_t max_temp);

/* ============================================================================
   YAKIT BASINCI SENSÖRÜ FONKSİYONLARI (4 fonksiyon)
   ============================================================================ */

/**
 * @brief Yakıt basıncı sensöründen değer okur
 * @return Mevcut yakıt basıncı kPa cinsinden
 */
uint16_t sensor_read_fuel_pressure_sensor(void);

/**
 * @brief Yakıt basıncı komutunu sensöre gönderir
 * @param pressure_kpa Hedef basınç kPa cinsinden
 * @return true başarılı, false başarısız
 */
bool sensor_write_fuel_pressure_command(uint16_t pressure_kpa);

/**
 * @brief Yakıt basıncı sensörü durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_fuel_pressure_status(void);

/**
 * @brief Yakıt basıncı sensörünü kalibre eder
 * @param reference_pressure Referans basınç değeri
 * @return true başarılı, false başarısız
 */
bool sensor_calibrate_fuel_pressure_sensor(uint16_t reference_pressure);

/* ============================================================================
   YAKIT ENJEKSIYONU FONKSİYONLARI (3 fonksiyon)
   ============================================================================ */

/**
 * @brief Yakıt enjeksiyonu komutunu gönderir
 * @param percentage Enjeksiyonu yüzdesi (10-100)
 * @return true başarılı, false başarısız
 */
bool sensor_write_fuel_injection_command(uint8_t percentage);

/**
 * @brief Yakıt sensörü durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_fuel_sensor_status(void);

/**
 * @brief Yakıt akış hızını ölçer
 * @return Yakıt akış hızı mL/min cinsinden
 */
uint16_t sensor_measure_fuel_flow_rate(void);

/* ============================================================================
   OKSİJEN SENSÖRÜ FONKSİYONLARI (3 fonksiyon)
   ============================================================================ */

/**
 * @brief Oksijen seviyesini ölçer
 * @return Oksijen seviyesi yüzdesi (0-100)
 */
uint8_t sensor_read_oxygen_sensor(void);

/**
 * @brief Oksijen sensörü durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_oxygen_sensor_status(void);

/**
 * @brief Oksijen sensörünü kalibre eder
 * @param reference_oxygen Referans oksijen seviyesi
 * @return true başarılı, false başarısız
 */
bool sensor_calibrate_oxygen_sensor(uint8_t reference_oxygen);

/* ============================================================================
   TURBO KONTROL FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Turbo sistemi etkinleştirir
 * @return true başarılı, false başarısız
 */
bool sensor_enable_turbo(void);

/**
 * @brief Turbo sistemini devre dışı bırakır
 * @return true başarılı, false başarısız
 */
bool sensor_disable_turbo(void);

/**
 * @brief Turbo boost komutunu gönderir
 * @param boost_pressure_kpa Hedef boost basıncı
 * @return true başarılı, false başarısız
 */
bool sensor_write_turbo_boost_command(uint16_t boost_pressure_kpa);

/**
 * @brief Turbo sensörü durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_turbo_sensor_status(void);

/**
 * @brief Turbo boost basıncını ölçer
 * @return Mevcut boost basıncı kPa cinsinden
 */
uint16_t sensor_measure_boost_pressure(void);

/* ============================================================================
   TUTUŞTURMAİGNİTION) KONTROL FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Tutuşturma zamanlaması komutunu gönderir
 * @param advance_degrees Zamanlaması derece cinsinden
 * @return true başarılı, false başarısız
 */
bool sensor_write_ignition_timing_command(int8_t advance_degrees);

/**
 * @brief Ignition coil gücü komutunu gönderir
 * @param coil_power Coil gücü yüzdesi (50-100)
 * @return true başarılı, false başarısız
 */
bool sensor_write_ignition_coil_command(uint8_t coil_power);

/**
 * @brief Spark plug durumunu kontrol eder
 * @return true sağlıklı, false değiştirilmesi gerekli
 */
bool sensor_check_spark_plug_status(void);

/**
 * @brief Tutuşturma sistemi durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_ignition_system_status(void);

/**
 * @brief Tutuşturma sistemini kalibre eder
 * @param reference_timing Referans zamanlaması derece cinsinden
 * @return true başarılı, false başarısız
 */
bool sensor_calibrate_ignition_system(int8_t reference_timing);

/* ============================================================================
   SOĞUTMA SISTEMI FONKSİYONLARI (3 fonksiyon)
   ============================================================================ */

/**
 * @brief Soğutma fanı komutunu gönderir
 * @param fan_speed Fanı hızı yüzdesi (0-100)
 * @return true başarılı, false başarısız
 */
bool sensor_write_cooling_fan_command(uint8_t fan_speed);

/**
 * @brief Soğutma sistemi durumunu kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool sensor_check_cooling_system_status(void);

/**
 * @brief Radyatör suyu sıcaklığını ölçer
 * @return Radyatör suyu sıcaklığı Celsius cinsinden
 */
int16_t sensor_measure_coolant_temperature(void);

/* ============================================================================
   HAVA ALıMı VE THROTTLE FONKSİYONLARI (3 fonksiyon)
   ============================================================================ */

/**
 * @brief Hava alımı komutunu gönderir
 * @param air_intake_percentage Hava alımı yüzdesi (10-100)
 * @return true başarılı, false başarısız
 */
bool sensor_write_air_intake_command(uint8_t air_intake_percentage);

/**
 * @brief Throttle konumu komutunu gönderir
 * @param throttle_position Throttle konumu yüzdesi (0-100)
 * @return true başarılı, false başarısız
 */
bool sensor_write_throttle_command(uint8_t throttle_position);

/**
 * @brief Hava akış hızını ölçer
 * @return Hava akış hızı g/s cinsinden
 */
uint16_t sensor_measure_air_flow_rate(void);

/* ============================================================================
   TOPLU İŞLEMLER FONKSİYONLARI (2 fonksiyon)
   ============================================================================ */

/**
 * @brief Tüm sensörlerden okuma yapar
 * @param readings Sensör değerlerini içerecek yapı
 * @return true başarılı, false başarısız
 */
bool sensor_read_all_sensors(SensorReadings_t *readings);

/**
 * @brief Tüm sensörlerin durumunu kontrol eder
 * @return true tümü sağlıklı, false hata var
 */
bool sensor_check_all_sensor_status(void);

/* ============================================================================
   İLERİ SEVİYE FONKSİYONLAR (POINTER & TYPEDEF)
   ============================================================================ */

/**
 * @brief Sensör verisi noktasını işleyen fonksiyon
 * @param data_point Sensör veri noktasına işaretçi
 * @return İşlem sonucu (SensorResult_t)
 */
SensorResult_t sensor_process_data_point(const SensorDataPoint_t *data_point);

/**
 * @brief Batch sensör verilerini okuyan fonksiyon
 * @param batch Batch veri yapısına işaretçi (çıktı)
 * @return Başarılı true, başarısız false
 */
bool sensor_read_batch_data(SensorBatch_t *batch);

/**
 * @brief Batch sensör verilerini yazan fonksiyon
 * @param batch Batch veri yapısına işaretçi (giriş)
 * @return İşlem sonucu (SensorResult_t)
 */
SensorResult_t sensor_write_batch_data(const SensorBatch_t *batch);

/**
 * @brief Sensör kalibrasyonu yapısı döndüren fonksiyon
 * @param sensor_id Sensör ID'si
 * @param calibration_data Kalibrasyon verisi yapısına işaretçi (çıktı)
 * @return Başarılı true, başarısız false
 */
bool sensor_get_calibration_data(uint16_t sensor_id, void *calibration_data);

/**
 * @brief Sensör veri noktası dizisini işleyen fonksiyon
 * @param data_points Veri noktaları dizisi
 * @param count Dizi eleman sayısı
 * @return İşlenen nokta sayısı
 */
uint8_t sensor_process_data_point_array(const SensorDataPoint_t *data_points, uint8_t count);

/**
 * @brief Sensör hata kodunu yapıya kopyalayan fonksiyon
 * @param out_error Hata bilgisi yapısına işaretçi
 * @return Kopyalanan bayt sayısı
 */
uint16_t sensor_get_error_structure(void *out_error);

/**
 * @brief Birden fazla sensörü sıfırlayan fonksiyon
 * @param sensor_ids Sensör ID'leri dizisi
 * @param count Dizi eleman sayısı
 * @return Sıfırlanan sensör sayısı
 */
uint8_t sensor_reset_multiple_sensors(const uint16_t *sensor_ids, uint8_t count);

/**
 * @brief Sensör durumunu pointer aracılığıyla döndüren fonksiyon
 * @param out_status Durum yapısına işaretçi
 * @return İşlem sonucu (SensorResult_t)
 */
SensorResult_t sensor_get_status_structure(SensorStatus_t *out_status);

/**
 * @brief Ham sensör verisini işleyen geri çağırabilir fonksiyon
 * @param raw_data Ham veri işaretçisi
 * @param processor_context İşleyici bağlamı
 * @return İşlem başarılı true, başarısız false
 */
typedef bool (*SensorDataProcessor_t)(const int32_t *raw_data, void *processor_context);

/**
 * @brief Özel sensör işleyicisi çalıştıran fonksiyon
 * @param processor Veri işleyici callback'i
 * @param context Bağlam
 * @return İşlem sonucu
 */
bool sensor_run_custom_processor(SensorDataProcessor_t processor, void *context);

/**
 * @brief Sensör konfigürasyonunu döndüren fonksiyon
 * @param out_config Konfigürasyon yapısına işaretçi
 * @return Kopyalanan yapı boyutu
 */
uint16_t sensor_get_configuration_structure(void *out_config);

#endif /* SENSOR_INTERFACE_H */

