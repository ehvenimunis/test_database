# Otomotiv Motor Kontrol Sistemi Dataset

## Proje Yapısı

```
dataset/
├── engine_control/                          [MAIN MODULE - 40+ fonksiyon]
│   ├── engine_control.h                     (Header - API tanımları)
│   ├── engine_control.c                     (Implementation)
│   └── README.md                            (Modül dokümantasyonu)
│
├── sensor_interface/                        [MODULE 2 - 20+ fonksiyon]
│   ├── sensor_interface.h                   (Header - Sensör API)
│   ├── sensor_interface.c                   (Implementation)
│   └── README.md                            (Modül dokümantasyonu)
│
└── vehicle_diagnostics/                     [MODULE 3 - 20+ fonksiyon]
    ├── vehicle_diagnostics.h                (Header - Tanı API)
    ├── vehicle_diagnostics.c                (Implementation)
    └── README.md                            (Modül dokümantasyonu)
```

## Genel Bakış

Bu dataset, **otomotiv endüstrisinde kullanılan gerçekçi motor kontrol sistemi**nin C dilinde implementasyonunu içerir. Sistem, LLM tabanlı test geliştirme çerçevesine girdi olarak sunulmaktadır.

### Ana Özellikler

- **Toplam 80+ Public Fonksiyon**: Üç modül üzerinde
- **Orta-İleri Karmaşıklık**: Gerçek-dünya senaryoları
- **Kapsamlı Bağımlılıklar**: Modüller arası çağrılar
- **Hata Yönetimi**: Kapsamlı hata kodlama ve recovery
- **Durum Yönetimi**: Static değişkenler ile gerçek-zamanlı durum takibi

## Modülöğründe Detaylar

### 1. Engine Control (Ana Modül) - 40 Fonksiyon

Motor kontrol sisteminin ana modülüdür. Tüm motor operasyonlarını koordine eder.

**Fonksiyon Kategorileri:**
- Başlangıç/Kapanma (5)
- RPM Kontrolü (8)
- Yakıt Enjeksiyonu (7)
- Sıcaklık Kontrolü (6)
- Turbo Sistemi (4)
- Tutuşturma/Ignition (5)
- Motor Durumu & Sensörler (5)
- Hata Yönetimi & Recovery (4)
- Diğer Fonksiyonlar (3)

**Bağımlılıkları:**
- `sensor_interface.h` → Sensör okuma/yazma
- `vehicle_diagnostics.h` → Hata teşhisi ve sistem sağlığı

**Test Zorluk Seviyesi:** ⭐⭐⭐⭐⭐ (En zorlu)
- Uzun yollar (long execution paths)
- Çok sayıda bağımlılık çağrısı
- Karmaşık durum geçişleri
- Hata handling mantığı

### 2. Sensor Interface (Modül 2) - 20+ Fonksiyon

Tüm sensörlerin okuma ve yazma işlemlerinin arayüzü.

**Fonksiyon Kategorileri:**
- Başlangıç/Kapanma (3)
- RPM Sensörü (4)
- Sıcaklık Sensörü (4)
- Yakıt Basıncı (4)
- Yakıt Enjeksiyonu (3)
- Oksijen Sensörü (3)
- Turbo Kontrol (5)
- Tutuşturma (5)
- Soğutma Sistemi (3)
- Hava Alımı & Throttle (3)
- Toplu İşlemler (2)

**Test Zorluk Seviyesi:** ⭐⭐⭐ (Orta)
- Basit input/output mappings
- Parametre validasyonu
- Sınır değer kontrolü

### 3. Vehicle Diagnostics (Modül 3) - 20+ Fonksiyon

Hata teşhisi, sistem sağlığı kontrol ve raporlama.

**Fonksiyon Kategorileri:**
- Başlangıç/Kapanma (2)
- Hata Raporlama (5)
- Sistem Sağlığı (6)
- Performans Analizi (6)
- Tarama & Tetikleme (4)
- İstatistik & Raporlama (5)
- Uyarı Yönetimi (5)
- Kütüphane Versiyonu (3)

**Test Zorluk Seviyesi:** ⭐⭐⭐⭐ (İleri)
- Karmaşık state machine
- Hata kod yönetimi
- İstatistik hesaplamaları

## Hata Kodları

Sistem-çapında hata kodlama şeması:

```
0x1XXX - Engine Control hataları
0x2XXX - Mod ve durum hataları
0x3XXX - RPM kontrol hataları
0x4XXX - Yakıt enjeksiyonu hataları
0x5XXX - Sıcaklık kontrol hataları
0x6XXX - Turbo sistem hataları
0x7XXX - Tutuşturma sistemi hataları
0x8XXX - Motor durumu ve sensör hataları
0x9XXX - Kritik hata yönetimi hataları
```

## Veri Tipleri ve Yapıları

### Engine Control

- `EngineState_t` - Motor durumu enumu (IDLE, RUNNING, etc.)
- `RPMStatus_t` - RPM durumu enumu
- `FuelInjectionMode_t` - Yakıt modu enumu
- `IgnitionTimingMode_t` - Tutuşturma modu enumu
- `EngineStatus_t` - Mevcut motor parametreleri yapısı
- `EngineCommand_t` - Motor komutları yapısı
- `ErrorLog_t` - Hata günlüğü yapısı
- `DiagnosticData_t` - Tanı verileri yapısı

### Sensor Interface

- `SensorStatus_t` - Sensör durumu enumu
- `SensorReadings_t` - Tüm sensör değerleri yapısı

### Vehicle Diagnostics

- `DiagnosticSeverity_t` - Hata şiddeti enumu (INFO, WARNING, ERROR, CRITICAL)
- `DiagnosticCode_t` - Hata kodu bilgileri yapısı
- `DiagnosticSummary_t` - Tanı özeti yapısı

## Parametre Aralıkları

| Parametre | Min | Max | Birim | Validasyon |
|-----------|-----|-----|-------|-----------|
| RPM | 500 | 8000 | RPM | Limit kontrolü |
| Sıcaklık | -40 | 150 | °C | Aralık kontrolü |
| Yakıt Basıncı | 200 | 600 | kPa | Aralık kontrolü |
| Oksijen | 0 | 100 | % | Aralık kontrolü |
| Turbo Boost | 0 | 300 | kPa | Aralık kontrolü |
| Tutuşturma Advance | -15 | +15 | derece | Aralık kontrolü |
| Fan Hızı | 0 | 100 | % | Aralık kontrolü |
| Hava Alımı | 10 | 100 | % | Aralık kontrolü |
| Throttle | 0 | 100 | % | Aralık kontrolü |
| Yakıt Enjeksiyonu | 10 | 100 | % | Aralık kontrolü |

## Bağımlılık Grafiği

```
engine_control.c
    ├── #include "sensor_interface.h"
    │   ├── sensor_read_rpm_sensor()
    │   ├── sensor_write_rpm_command()
    │   ├── sensor_read_temperature_sensor()
    │   ├── sensor_write_fuel_pressure_command()
    │   ├── sensor_write_fuel_injection_command()
    │   ├── sensor_read_oxygen_sensor()
    │   ├── sensor_enable_turbo() / sensor_disable_turbo()
    │   ├── sensor_write_turbo_boost_command()
    │   ├── sensor_write_ignition_timing_command()
    │   ├── sensor_write_cooling_fan_command()
    │   ├── sensor_write_air_intake_command()
    │   ├── sensor_write_throttle_command()
    │   └── [diğer sensör fonksiyonları...]
    │
    └── #include "vehicle_diagnostics.h"
        ├── diagnostic_report_error()
        ├── diagnostic_initialize()
        ├── diagnostic_shutdown()
        ├── diagnostic_log_emergency_event()
        └── [diğer tanı fonksiyonları...]
```

## Test Sürütmek İçin Önerilen Yapı

```c
/* Ceedling test dosyası örneği */
#include "unity.h"
#include "engine_control.h"
#include "mock_sensor_interface.h"
#include "mock_vehicle_diagnostics.h"

void test_engine_initialize_with_valid_rpm(void) {
    // Test: engine_initialize(1000) başarılı olmalı
    // Mock: sensor_initialize() true döner
    // Mock: diagnostic_initialize() true döner
    // Assert: engine_initialize(1000) == true
}

void test_engine_set_target_rpm_with_invalid_value(void) {
    // Test: engine_set_target_rpm(9000) başarısız olmalı
    // Assert: engine_set_target_rpm(9000) == false
}

// ... 40+ test fonksiyonu
```

## Dosya Boyutları

| Dosya | Satır | Fonksiyon | Karmaşıklık |
|-------|-------|-----------|----------|
| engine_control.h | ~400 | 40 | Yüksek |
| engine_control.c | ~600 | 40 | Yüksek |
| sensor_interface.h | ~350 | 20+ | Orta |
| sensor_interface.c | ~400 | 20+ | Orta |
| vehicle_diagnostics.h | ~350 | 20+ | Orta-Yüksek |
| vehicle_diagnostics.c | ~500 | 20+ | Orta-Yüksek |

**Toplam:** ~2,600 satır koddan oluşan gerçekçi, test edilebilir dataset

## Kullanım Senaryoları

### Senaryo 1: Motor Başlatma ve RPM Kontrolü

```c
engine_initialize(1000);
engine_set_target_rpm(3000);
engine_increase_rpm(25);  // %25 artış
uint16_t current = engine_get_current_rpm();
```

### Senaryo 2: Yakıt Sistemi Kontrolü

```c
engine_set_fuel_injection_mode(FUEL_INJECTION_NORMAL);
engine_set_fuel_injection_percentage(80);
engine_set_fuel_pressure(350);
float consumption = engine_calculate_fuel_consumption();
bool health = engine_check_fuel_system_health();
```

### Senaryo 3: Turbo Kontrolü

```c
engine_set_turbo_enabled(true);
engine_set_turbo_boost_pressure(150);
uint16_t boost = engine_get_turbo_boost_pressure();
bool health = engine_check_turbo_system_health();
```

### Senaryo 4: Hata Yönetimi

```c
EngineStatus_t status;
engine_get_status(&status);

if (status.temperature_celsius > 120) {
    engine_attempt_recovery();
    ErrorLog_t error;
    engine_get_last_error(&error);
}
```

## Test Stratejisi

Bu dataset, LLM test geliştirme çerçevesi tarafından şunlar için kullanılacaktır:

1. **Birim Test Oluşturma**: Her fonksiyon için otomatik test üretimi
2. **Mock Oluşturma**: CMock kullanarak bağımlılıkları mock'lama
3. **Kapsama Analizi**: Statement, branch, path coverage hesaplama
4. **Regresyon Testi**: Test setinin çalıştırılabilirliği doğrulama

## Notlar

- Dataset, gerçek-dünya otomotiv sistemlerinin semplifying edilmiş versiyonudur
- Static değişkenler, gerçek sensör verilerini simüle eder
- Hata kodlaması, OBD-II standartlarına yakındır
- Tüm fonksiyonlar, NULL pointer kontrolleri ve parametre validasyonu içerir

## Sorumluluk

Bu dataset, **LLM test geliştirme çerçevesi** tarafından işlenecektir. Test kodları otomatik olarak üretilecektir.

---

**Dataset Oluşturuldu:** 2025  
**Versiyon:** 1.0  
**Durum:** Test Hazır ✅

