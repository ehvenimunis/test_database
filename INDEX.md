# Dataset İndeksi ve Gezinti Kılavuzu

## 📑 Tüm Dosyalar

### 📋 Dokümantasyon Dosyaları

#### Ana Dokümantasyon
1. **README.md** (Ana Dokümantasyon)
   - Projenin genel tanımı ve yapısı
   - Modül mimarisi ve bağımlılıkları
   - Hata kodları ve veri tipleri
   - Başlaması için gereken tüm bilgi

2. **QUICK_START.md** (Hızlı Başlangıç)
   - Dataset yapısının özeti
   - Modüllerin kısa açıklaması
   - Örnek kullanım kodları
   - 5 dakikada başlamak için

3. **PROJECT_SUMMARY.md** (Proje Özeti)
   - Kapsamlı proje tanımı
   - İstatistikler ve metrikler
   - 7 adımlı framework detayları
   - Kod kalitesi metrikleri

4. **DIALOGUE_BASED_TESTING_FRAMEWORK.md** (Test Framework)
   - 7 adımlı diyalog akışı
   - Her adım için LLM promptları
   - Beklenen LLM yanıtları
   - Örnek test kodları

5. **INDEX.md** (Bu Dosya)
   - Tüm dosyaları gezinti haritası

### 🔧 Modül Dosyaları

#### Engine Control Module (ANA MODÜL - 40 Fonksiyon)
- **engine_control/engine_control.h** (400+ satır)
  - 40 public fonksiyon tanımı
  - Veri yapıları ve enum'lar
  - Detaylı fonksiyon dökümentasyonu

- **engine_control/engine_control.c** (600+ satır)
  - 40 fonksiyonun implementasyonu
  - Static durum yönetimi
  - İçsel yardımcı fonksiyonlar

- **engine_control/README.md** (150+ satır)
  - Modülün detaylı açıklaması
  - Fonksiyon kategorileri
  - Hata kodları tanımı
  - Kullanım örnekleri

#### Sensor Interface Module (MODÜL 2 - 20+ Fonksiyon)
- **sensor_interface/sensor_interface.h** (350+ satır)
  - 20+ sensör fonksiyonu
  - Sensör veri yapıları
  - Kalibrasyon fonksiyonları

- **sensor_interface/sensor_interface.c** (400+ satır)
  - Sensör implementasyonları
  - Static sensör durumu
  - Mock-ready tasarım

- **sensor_interface/README.md** (150+ satır)
  - Sensör modülü detayları
  - Sensör değer aralıkları
  - Modül mimarisi

#### Vehicle Diagnostics Module (MODÜL 3 - 20+ Fonksiyon)
- **vehicle_diagnostics/vehicle_diagnostics.h** (350+ satır)
  - 20+ tanı fonksiyonu
  - Hata kodlama sistemi
  - Uyarı yönetimi

- **vehicle_diagnostics/vehicle_diagnostics.c** (500+ satır)
  - Tanı implementasyonları
  - Hata yönetimi mantığı
  - Performans analizi

- **vehicle_diagnostics/README.md** (150+ satır)
  - Tanı sistemi detayları
  - Uyarı seviyeleri
  - Hata yönetimi

## 🗺️ Dosya Yapısı

```
dataset/
├── 📄 INDEX.md (Bu dosya - gezinti haritası)
├── 📄 README.md (Ana dokümantasyon)
├── 📄 QUICK_START.md (Hızlı başlangıç)
├── 📄 PROJECT_SUMMARY.md (Proje özeti)
├── 📄 DIALOGUE_BASED_TESTING_FRAMEWORK.md (Test framework)
│
├── 📁 engine_control/
│   ├── engine_control.h (400+ satır, 40 fonksiyon)
│   ├── engine_control.c (600+ satır, implementasyon)
│   └── README.md (Modül dokümantasyonu)
│
├── 📁 sensor_interface/
│   ├── sensor_interface.h (350+ satır, 20+ fonksiyon)
│   ├── sensor_interface.c (400+ satır, implementasyon)
│   └── README.md (Modül dokümantasyonu)
│
└── 📁 vehicle_diagnostics/
    ├── vehicle_diagnostics.h (350+ satır, 20+ fonksiyon)
    ├── vehicle_diagnostics.c (500+ satır, implementasyon)
    └── README.md (Modül dokümantasyonu)
```

## 🎯 Amacına Göre Okuma Sırası

### Senaryo 1: 5 Dakika İçinde Başlangıç
1. **QUICK_START.md** ← Başla
2. **README.md** ← Detay
3. **engine_control/engine_control.h** ← API

### Senaryo 2: Kapsamlı Öğrenme
1. **README.md** ← Genel bakış
2. **PROJECT_SUMMARY.md** ← Detaylar
3. **engine_control/README.md** ← Modül 1
4. **sensor_interface/README.md** ← Modül 2
5. **vehicle_diagnostics/README.md** ← Modül 3
6. **engine_control/engine_control.h** ← API Detayları

### Senaryo 3: Test Geliştirme
1. **DIALOGUE_BASED_TESTING_FRAMEWORK.md** ← Framework
2. **engine_control/engine_control.h** ← Header
3. **sensor_interface/sensor_interface.h** ← Header
4. **vehicle_diagnostics/vehicle_diagnostics.h** ← Header
5. ADIM 1-7 promptlarını LLM'e ver

### Senaryo 4: Implementation Derinliği
1. **engine_control/engine_control.c** ← Kod inceleme
2. **sensor_interface/sensor_interface.c** ← Kod inceleme
3. **vehicle_diagnostics/vehicle_diagnostics.c** ← Kod inceleme
4. **README.md** ← Hata kodları referansı

## 📊 Dosya Istatistikleri

| Dosya | Satır | Tip | Amaç |
|-------|-------|-----|------|
| README.md | 300+ | Markdoun | Genel proje tanımı |
| QUICK_START.md | 250+ | Markdown | Hızlı başlangıç |
| PROJECT_SUMMARY.md | 300+ | Markdown | Proje özeti |
| DIALOGUE_BASED_TESTING_FRAMEWORK.md | 500+ | Markdown | Test framework |
| engine_control.h | 400+ | C Header | Main API |
| engine_control.c | 600+ | C Code | Main Implementation |
| engine_control/README.md | 150+ | Markdown | Module docs |
| sensor_interface.h | 350+ | C Header | Sensor API |
| sensor_interface.c | 400+ | C Code | Sensor Implementation |
| sensor_interface/README.md | 150+ | Markdown | Module docs |
| vehicle_diagnostics.h | 350+ | C Header | Diag API |
| vehicle_diagnostics.c | 500+ | C Code | Diag Implementation |
| vehicle_diagnostics/README.md | 150+ | Markdown | Module docs |
| **TOPLAM** | **4,100+** | | |

## 🔍 Hızlı Referans

### Engine Control Fonksiyonları
- **Başlatma**: `engine_initialize()`, `engine_shutdown()`
- **RPM**: `engine_set_target_rpm()`, `engine_increase_rpm()`, `engine_decrease_rpm()`
- **Yakıt**: `engine_set_fuel_injection_percentage()`, `engine_set_fuel_pressure()`
- **Sıcaklık**: `engine_get_temperature()`, `engine_set_cooling_fan_speed()`
- **Turbo**: `engine_set_turbo_enabled()`, `engine_set_turbo_boost_pressure()`
- **Tutuşturma**: `engine_set_ignition_timing()`, `engine_set_ignition_coil_power()`

### Sensor Interface Fonksiyonları
- **RPM**: `sensor_read_rpm_sensor()`, `sensor_write_rpm_command()`
- **Sıcaklık**: `sensor_read_temperature_sensor()`, `sensor_calibrate_temperature_sensor()`
- **Yakıt**: `sensor_read_fuel_pressure_sensor()`, `sensor_write_fuel_injection_command()`
- **Oksijen**: `sensor_read_oxygen_sensor()`, `sensor_calibrate_oxygen_sensor()`
- **Turbo**: `sensor_enable_turbo()`, `sensor_write_turbo_boost_command()`

### Diagnostics Fonksiyonları
- **Hata**: `diagnostic_report_error()`, `diagnostic_clear_error_code()`
- **Sağlık**: `diagnostic_check_system_health()`, `diagnostic_check_engine_health()`
- **Analiz**: `diagnostic_analyze_engine_performance()`, `diagnostic_calculate_fuel_efficiency()`
- **Uyarı**: `diagnostic_trigger_critical_warning()`, `diagnostic_dismiss_warning()`

## 🔗 Bağımlılıklar

```
engine_control
    ├── #include "sensor_interface.h"
    │   └── 12+ direktif çağrısı
    │
    └── #include "vehicle_diagnostics.h"
        └── 8+ direktif çağrısı
```

## 📌 Önemli Notlar

1. **Static Durum Yönetimi**: Her modül static değişkenlerde durum tutmaktadır
2. **Mock Hazırlığı**: Tüm fonksiyonlar CMock ile mock'lanmak için tasarlandı
3. **Parametre Validasyonu**: Tüm fonksiyonlar girdilerini valide eder
4. **Hata Kodlama**: 0xXYZZ formatında kapsamlı hata kodları
5. **Dokümantasyon**: Her fonksiyon Doxygen formatında dokümante edilmiş

## ✅ Dataset Özellikleri

- ✅ **80+ Fonksiyon**: Test etmeye hazır
- ✅ **~2,600 Satır Kod**: Realističtir
- ✅ **Kapsamlı Dokümantasyon**: 6 markdown dosyası
- ✅ **7 Adımlı Test Framework**: LLM-uyumlu
- ✅ **Mock-Ready**: CMock ile uyumlu
- ✅ **Modüler Tasarım**: Bağımlılık yönetimi
- ✅ **Gerçekçi**: Motor kontrol sistemlerine dayalı

## 🚀 Başlamak için

### Option 1: 5 Dakika
```
1. QUICK_START.md oku
2. engine_control/engine_control.h incele
3. Yapı anla
```

### Option 2: 30 Dakika
```
1. README.md oku
2. 3 modülün README'sini oku
3. Header dosyalarını incele
```

### Option 3: Test Geliştirme
```
1. DIALOGUE_BASED_TESTING_FRAMEWORK.md oku
2. ADIM 1-7 promptlarını LLM'e ver
3. Otomatik test kodları al
```

## 📞 Sık Sorulan Sorular

**S: Hangi dosyayı ilk olarak okumalıyım?**  
C: Amacınıza bağlı:
- Hızlı başlama: QUICK_START.md
- Kapsamlı anlama: README.md
- Test yazma: DIALOGUE_BASED_TESTING_FRAMEWORK.md

**S: Kaç test senaryosu yazılabilir?**  
C: 55+ test senaryosu oluşturulabilir (başarı, hata, sınır değer, mock)

**S: Modüller arası bağımlılıklar nelerdir?**  
C: `engine_control` → `sensor_interface` + `vehicle_diagnostics`

**S: Hangi framework'ü kullanmalıyım?**  
C: Ceedling/Unity + CMock (önerilir)

**S: Static durum değişkenleri test edilebilir mi?**  
C: Evet, fonksiyonların dönüş değerleri ve yan etkileri üzerinden

## 📚 Ek Kaynaklar

- **Ceedling**: http://www.throwtheswitch.org/ceedling
- **Unity**: http://www.throwtheswitch.org/unity
- **CMock**: http://www.throwtheswitch.org/cmock
- **C Programlama**: Standart C referansı

---

**Dataset Version**: 1.0  
**Last Updated**: 2025  
**Status**: ✅ COMPLETE  
**Files**: 17  
**Total Lines**: 4,100+  
**Functions**: 80+  
**Test Potential**: 55+

