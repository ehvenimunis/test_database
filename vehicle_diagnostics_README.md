# Vehicle Diagnostics Module (Modül 3)

## Genel Bilgi

Bu modül, araç tanı ve sistem sağlığı kontrol fonksiyonlarını sağlar. Hata kodlama, uyarı yönetimi, performans analizi ve raporlama işlemlerini yönetir.

## Özellikler

- **20+ Public Fonksiyon** ile tanı sistemi yönetimi
- **Hata Raporlama**: Hata kodlama, temizleme, aktif hata listesi
- **Sistem Sağlığı**: Genel, motor, yakıt, tutuşturma, emisyon, turbo
- **Performans Analizi**: Motor performans skoru, yakıt verimliliği, emisyon seviyesi
- **Tarama Fonksiyonları**: Tam sistem taraması, sensör taraması, OBD verileri
- **İstatistik & Raporlama**: Toplam çalışma saati, hata sayısı, bakım aralığı
- **Uyarı Yönetimi**: Kritik uyarılar, uyarı seviyeleri, aktif uyarılar
- **Self-test**: Tanı sisteminin kendi kontrol fonksiyonları

## Bağımlılıklar

Bağımsız modül olarak tasarlanmış. Engine Control modülü tarafından kullanılır.

## Modül Mimarisi

### Veri Yapıları

```c
DiagnosticSeverity_t    - Hata şiddeti seviyeleri
DiagnosticCode_t        - Hata kodu bilgileri
DiagnosticSummary_t     - Tanı özet bilgileri
```

### Fonksiyon Kategorileri

1. **Başlangıç/Kapanma (2)**: `diagnostic_initialize`, `diagnostic_shutdown`

2. **Hata Raporlama (5)**: `diagnostic_report_error`, `diagnostic_clear_error_code`, `diagnostic_clear_all_error_codes`, `diagnostic_get_active_error_codes`, `diagnostic_get_last_error_code`

3. **Sistem Sağlığı Kontrol (6)**: `diagnostic_check_system_health`, `diagnostic_check_engine_health`, `diagnostic_check_fuel_system_health`, `diagnostic_check_ignition_system_health`, `diagnostic_check_emission_system_health`, `diagnostic_check_turbo_system_health`

4. **Performans Analiz (6)**: `diagnostic_analyze_engine_performance`, `diagnostic_calculate_fuel_efficiency`, `diagnostic_measure_emission_level`, `diagnostic_check_temperature_stability`, `diagnostic_check_rpm_stability`, `diagnostic_verify_sensor_calibration`

5. **Tarama ve Tetikleme (4)**: `diagnostic_perform_full_scan`, `diagnostic_scan_all_sensors`, `diagnostic_read_obd_data`, `diagnostic_log_emergency_event`

6. **İstatistik ve Raporlama (5)**: `diagnostic_get_summary`, `diagnostic_get_total_runtime_hours`, `diagnostic_get_total_error_count`, `diagnostic_calculate_maintenance_interval`, `diagnostic_save_report_to_file`

7. **Uyarı ve Kontrol (5)**: `diagnostic_trigger_critical_warning`, `diagnostic_dismiss_warning`, `diagnostic_get_active_warnings`, `diagnostic_get_last_warning`, `diagnostic_set_warning_level`

8. **Kütüphane Versiyonu ve Kontrol (3)**: `diagnostic_get_library_version`, `diagnostic_perform_self_test`, `diagnostic_is_system_active`

## Hata Kodu Kategorileri

Hata kodları, şunlar olarak kategorize edilir:

- `0x1XXX` - Motor hataları
- `0x4XXX` - Yakıt sistemi hataları
- `0x7XXX` - Tutuşturma sistemi hataları
- `0x6XXX` - Turbo sistemi hataları
- `0x9XXX` - Kritik hata kodları

## Uyarı Seviyeleri

| Seviye | Açıklama |
|--------|----------|
| 0 | BILGI (Info) |
| 1 | UYARI (Warning) |
| 2 | HATA (Error) |
| 3 | KRİTİK (Critical) |

## Performans Puanları

| Metrik | Aralık | Açıklama |
|--------|--------|----------|
| Engine Performance | 0-100 | Yüksek daha iyi |
| Fuel Efficiency | L/100km | Düşük daha iyi |
| Emission Level | 0-100 | Düşük daha iyi |

## Örnek Kullanım

```c
#include "vehicle_diagnostics.h"

// Tanı sistemini başlat
if (diagnostic_initialize()) {
    // Hata raporla
    diagnostic_report_error(0x1001, "Engine overheat detected");
    
    // Sistem sağlığını kontrol et
    if (!diagnostic_check_system_health()) {
        // Sorun var
    }
    
    // Aktif hata kodlarını al
    uint16_t errors[20];
    uint8_t error_count = diagnostic_get_active_error_codes(errors, 20);
    
    // Performans analizi
    uint8_t performance = diagnostic_analyze_engine_performance();
    float fuel_eff = diagnostic_calculate_fuel_efficiency();
    
    // Tanı özetini al
    DiagnosticSummary_t summary;
    if (diagnostic_get_summary(&summary)) {
        // summary.active_codes, summary.system_healthy vb.
    }
    
    diagnostic_shutdown();
}
```

## Hata Yönetimi

Tanı sistemi, şunları takip eder:

- **Aktif Hata Sayısı**: Maksimum 20 aktivatif hata
- **Toplam Hata Sayısı**: Sistem başlatıldığından beri toplam hata sayısı
- **Son Hata Kodu**: En son raporlanan hata
- **Toplam Çalışma Saati**: Runtime verisi

## Durum Yönetimi

Modül, şunları içsel olarak yönetir:

- Aktif hata kod listesi
- Toplam hata ve çalışma istatistikleri
- Aktif uyarı listesi
- Uyarı seviyesi ayarları
- Genel sistem sağlık durumu

## Notlar

- Tanı sistemi başlatılmadıkça fonksiyonlar çalışmaz
- Tüm sağlık kontrol fonksiyonları hata kodlarını kontrol ederek karar verir
- Performans puanları hata sayısına göre otomatik olarak hesaplanır
- OBD (On-Board Diagnostics) verileri standart format kullanır

