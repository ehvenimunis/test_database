/**
 * @file vehicle_diagnostics.h
 * @brief Araç Tanı Sistemi Modülü - Motor Kontrol Sisteminin Bağımlılığı
 * @details Hata tanı ve sistem sağlık kontrolü fonksiyonları
 * @version 1.0
 */

#ifndef VEHICLE_DIAGNOSTICS_H
#define VEHICLE_DIAGNOSTICS_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
   TANISALAMA TİPLERİ VE YAPILARI
   ============================================================================ */

typedef enum {
    DIAG_SEVERITY_INFO = 0,
    DIAG_SEVERITY_WARNING = 1,
    DIAG_SEVERITY_ERROR = 2,
    DIAG_SEVERITY_CRITICAL = 3
} DiagnosticSeverity_t;

typedef struct {
    uint16_t code;
    DiagnosticSeverity_t severity;
    char description[128];
    uint32_t occurrence_count;
    uint32_t last_occurrence_time;
} DiagnosticCode_t;

typedef struct {
    uint8_t total_codes;
    uint8_t active_codes;
    uint8_t critical_codes;
    bool system_healthy;
} DiagnosticSummary_t;

/* ============================================================================
   BAŞLANGIC VE KAPANMA FONKSİYONLARI (2 fonksiyon)
   ============================================================================ */

/**
 * @brief Tanı sistemini başlatır
 * @return true başarılı, false başarısız
 */
bool diagnostic_initialize(void);

/**
 * @brief Tanı sistemini kapatır
 * @return true başarılı, false başarısız
 */
bool diagnostic_shutdown(void);

/* ============================================================================
   HATA RAPORLAMA FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Hata kodunu raporlar
 * @param error_code Hata kodu
 * @param error_message Hata mesajı
 * @return true başarılı, false başarısız
 */
bool diagnostic_report_error(uint16_t error_code, const char *error_message);

/**
 * @brief Hata kodunu temizler
 * @param error_code Temizlenecek hata kodu
 * @return true başarılı, false başarısız
 */
bool diagnostic_clear_error_code(uint16_t error_code);

/**
 * @brief Tüm hata kodlarını temizler
 * @return true başarılı, false başarısız
 */
bool diagnostic_clear_all_error_codes(void);

/**
 * @brief Aktif hata kodlarını alır
 * @param codes Hata kodlarını içerecek dizi
 * @param max_codes Maksimum kod sayısı
 * @return Bulunan hata kodu sayısı
 */
uint8_t diagnostic_get_active_error_codes(uint16_t *codes, uint8_t max_codes);

/**
 * @brief Son hatayı alır
 * @return Son hata kodu
 */
uint16_t diagnostic_get_last_error_code(void);

/* ============================================================================
   SISTEM SAĞLIĞI KONTROL FONKSİYONLARI (6 fonksiyon)
   ============================================================================ */

/**
 * @brief Genel sistem sağlığını kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool diagnostic_check_system_health(void);

/**
 * @brief Motor durumunu kontrol eder
 * @return true normal, false sorun var
 */
bool diagnostic_check_engine_health(void);

/**
 * @brief Yakıt sistemi sağlığını kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool diagnostic_check_fuel_system_health(void);

/**
 * @brief Tutuşturma sistemi sağlığını kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool diagnostic_check_ignition_system_health(void);

/**
 * @brief Emişyon sistemi sağlığını kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool diagnostic_check_emission_system_health(void);

/**
 * @brief Türbo sistemi sağlığını kontrol eder
 * @return true sağlıklı, false hatalı
 */
bool diagnostic_check_turbo_system_health(void);

/* ============================================================================
   PERFORMANS ANALIZ FONKSİYONLARI (6 fonksiyon)
   ============================================================================ */

/**
 * @brief Motor performansını analiz eder
 * @return Performans skoru (0-100)
 */
uint8_t diagnostic_analyze_engine_performance(void);

/**
 * @brief Yakıt verimliliğini hesaplar
 * @return Yakıt verimliliği değeri
 */
float diagnostic_calculate_fuel_efficiency(void);

/**
 * @brief Emisyon seviyesini ölçer
 * @return Emisyon skoru (0-100, düşük daha iyi)
 */
uint8_t diagnostic_measure_emission_level(void);

/**
 * @brief Motor sıcaklığı stabilitesini kontrol eder
 * @return true stabil, false değişken
 */
bool diagnostic_check_temperature_stability(void);

/**
 * @brief RPM stabilitesini kontrol eder
 * @return true stabil, false değişken
 */
bool diagnostic_check_rpm_stability(void);

/**
 * @brief Sensör kalibrasyonu doğrulaması
 * @return true doğru, false yeniden kalibrasyonu gerekli
 */
bool diagnostic_verify_sensor_calibration(void);

/* ============================================================================
   TARAMA VE TETİKLEME FONKSİYONLARI (4 fonksiyon)
   ============================================================================ */

/**
 * @brief Tam sistem taraması yapan fonksiyon
 * @return Bulunan sorun sayısı
 */
uint8_t diagnostic_perform_full_scan(void);

/**
 * @brief Sensör taraması yapan fonksiyon
 * @return Sorunlu sensör sayısı
 */
uint8_t diagnostic_scan_all_sensors(void);

/**
 * @brief Motorun OBD (On-Board Diagnostics) verilerini alır
 * @param obd_data OBD verilerini içerecek dizi (bayt cinsinden)
 * @param max_size Maksimum veri boyutu
 * @return Alınan bayt sayısı
 */
uint16_t diagnostic_read_obd_data(uint8_t *obd_data, uint16_t max_size);

/**
 * @brief Acil durum olayını kaydeder
 * @return true başarılı, false başarısız
 */
bool diagnostic_log_emergency_event(void);

/* ============================================================================
   İSTATİSTİK VE RAPORLAMA FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Tanı özetini alır
 * @param summary Tanı özet yapısı
 * @return true başarılı, false başarısız
 */
bool diagnostic_get_summary(DiagnosticSummary_t *summary);

/**
 * @brief Toplam çalışma saatini döndürür
 * @return Toplam çalışma saati
 */
uint32_t diagnostic_get_total_runtime_hours(void);

/**
 * @brief Toplam hata sayısını döndürür
 * @return Toplam hata sayısı
 */
uint32_t diagnostic_get_total_error_count(void);

/**
 * @brief Önerilen bakım dönemini hesaplar
 * @return Günler cinsinden bakım dönemi
 */
uint16_t diagnostic_calculate_maintenance_interval(void);

/**
 * @brief Tanı raporunu dosyaya kaydeder
 * @param filename Dosya adı
 * @return true başarılı, false başarısız
 */
bool diagnostic_save_report_to_file(const char *filename);

/* ============================================================================
   UYARI VE KONTROL FONKSİYONLARI (5 fonksiyon)
   ============================================================================ */

/**
 * @brief Kritik uyarıyı tetikler
 * @param warning_code Uyarı kodu
 * @return true başarılı, false başarısız
 */
bool diagnostic_trigger_critical_warning(uint16_t warning_code);

/**
 * @brief Uyarıyı devre dışı bırakır
 * @param warning_code Uyarı kodu
 * @return true başarılı, false başarısız
 */
bool diagnostic_dismiss_warning(uint16_t warning_code);

/**
 * @brief Aktif uyarıları alır
 * @param warnings Uyarı kodlarını içerecek dizi
 * @param max_warnings Maksimum uyarı sayısı
 * @return Aktif uyarı sayısı
 */
uint8_t diagnostic_get_active_warnings(uint16_t *warnings, uint8_t max_warnings);

/**
 * @brief Son uyarıyı alır
 * @return Son uyarı kodu
 */
uint16_t diagnostic_get_last_warning(void);

/**
 * @brief Uyarı seviyesini ayarlar (0-3)
 * @param level Uyarı seviyesi
 * @return true başarılı, false başarısız
 */
bool diagnostic_set_warning_level(uint8_t level);

/* ============================================================================
   KÜTÜPHANE VERSİYONU VE KONTROL FONKSİYONLARI (3 fonksiyon)
   ============================================================================ */

/**
 * @brief Tanı kütüphanesi versiyon bilgisini döndürür
 * @return Versiyon numarası
 */
uint32_t diagnostic_get_library_version(void);

/**
 * @brief Self-test yapan fonksiyon
 * @return true geçti, false başarısız
 */
bool diagnostic_perform_self_test(void);

/**
 * @brief Tanı sisteminin durumunu döndürür
 * @return true aktif, false inaktif
 */
bool diagnostic_is_system_active(void);

/* ============================================================================
   İLERİ SEVİYE FONKSİYONLAR (POINTER & CALLBACK)
   ============================================================================ */

typedef enum {
    DIAG_RESULT_OK = 0,
    DIAG_RESULT_ERROR = -1,
    DIAG_RESULT_NO_DATA = -2
} DiagnosticResult_t;

typedef struct {
    uint16_t error_code;
    uint32_t occurrence_count;
    uint32_t last_timestamp;
    DiagnosticSeverity_t severity;
} ErrorHistory_t;

typedef struct {
    uint16_t total_errors;
    uint16_t critical_errors;
    float average_error_severity;
    uint32_t timestamp;
} DiagnosticSnapshot_t;

typedef bool (*DiagnosticFilterCallback_t)(const ErrorHistory_t *error, void *context);
typedef void (*DiagnosticReportCallback_t)(const DiagnosticSnapshot_t *snapshot);

/**
 * @brief Hata tarihçesi yapısını döndüren fonksiyon
 * @param error_code Hata kodu
 * @param out_history Hata tarihçesi yapısına işaretçi
 * @return İşlem sonucu (DiagnosticResult_t)
 */
DiagnosticResult_t diagnostic_get_error_history(uint16_t error_code, 
                                                 ErrorHistory_t *out_history);

/**
 * @brief Hata tarihçe dizisini işleyen fonksiyon
 * @param histories Hata tarihçeleri dizisi
 * @param count Dizi eleman sayısı
 * @param filter Filtreleme callback'i (NULL olabilir)
 * @param context Callback bağlamı
 * @return İşlenen tarihçe sayısı
 */
uint8_t diagnostic_process_error_history(const ErrorHistory_t *histories,
                                         uint8_t count,
                                         DiagnosticFilterCallback_t filter,
                                         void *context);

/**
 * @brief Tanı anlık görüntüsü oluşturan fonksiyon
 * @param out_snapshot Anlık görüntü yapısına işaretçi
 * @return Başarılı true, başarısız false
 */
bool diagnostic_create_snapshot(DiagnosticSnapshot_t *out_snapshot);

/**
 * @brief Rapor callback'ini tetikleyen fonksiyon
 * @param callback Rapor callback'i
 * @return Başarılı true, başarısız false
 */
bool diagnostic_trigger_report_callback(DiagnosticReportCallback_t callback);

/**
 * @brief Hata kodlarını filtreleyip diziye kopyalayan fonksiyon
 * @param out_codes Çıktı hata kodu dizisi
 * @param max_codes Maksimum kod sayısı
 * @param filter Filtreleme callback'i
 * @param context Bağlam
 * @return Kopyalanan kod sayısı
 */
uint8_t diagnostic_filter_error_codes(uint16_t *out_codes,
                                      uint8_t max_codes,
                                      DiagnosticFilterCallback_t filter,
                                      void *context);

/**
 * @brief Diagnostik veri yapısını kopyalayan fonksiyon
 * @param dest Hedef yapı işaretçisi
 * @param src Kaynak yapı işaretçisi
 * @param size Kopyalanacak bayt sayısı
 * @return Kopyalanan bayt sayısı
 */
uint16_t diagnostic_copy_data_structure(void *dest, const void *src, uint16_t size);

/**
 * @brief Hata özet pointerini döndüren fonksiyon
 * @return DiagnosticSummary yapısına const işaretçi
 */
const DiagnosticSummary_t* diagnostic_get_summary_pointer(void);

/**
 * @brief Çoklu hata tanısı yapan fonksiyon
 * @param error_codes Kontrol edilecek hata kodları dizisi
 * @param count Dizi eleman sayısı
 * @param out_results Tanı sonuçları dizisi (çıktı)
 * @return İşlenen hata sayısı
 */
uint8_t diagnostic_diagnose_multiple_errors(const uint16_t *error_codes,
                                            uint8_t count,
                                            bool *out_results);

/**
 * @brief Sistem performans verileri döndüren fonksiyon
 * @param out_performance Performans verisi yapısına işaretçi
 * @return Başarılı true, başarısız false
 */
bool diagnostic_get_performance_data(struct PerformanceData *out_performance);

/**
 * @brief Hata günlüğünü dosyaya yazan fonksiyon
 * @param filename Dosya adı
 * @param error_log Hata günlüğü yapısına işaretçi
 * @return Yazılan bayt sayısı
 */
uint32_t diagnostic_write_error_log_to_file(const char *filename, 
                                            const ErrorHistory_t *error_log);

#endif /* VEHICLE_DIAGNOSTICS_H */

