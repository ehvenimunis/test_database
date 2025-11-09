# Hızlı Başlangıç Kılavuzu

## Dataset Yapısı

```
dataset/
├── README.md                                  (Ana dokümantasyon)
├── QUICK_START.md                             (Bu dosya)
├── DIALOGUE_BASED_TESTING_FRAMEWORK.md        (7 adımlı prompt framework)
│
├── engine_control/                            [ANA MODÜL - 40 fonksiyon]
│   ├── engine_control.h                       
│   ├── engine_control.c                       
│   └── README.md                              
│
├── sensor_interface/                          [MODÜL 2 - 20+ fonksiyon]
│   ├── sensor_interface.h                     
│   ├── sensor_interface.c                     
│   └── README.md                              
│
└── vehicle_diagnostics/                       [MODÜL 3 - 20+ fonksiyon]
    ├── vehicle_diagnostics.h                  
    ├── vehicle_diagnostics.c                  
    └── README.md                              
```

## Dataset Özellikleri

| Özellik | Değer |
|---------|-------|
| **Toplam Fonksiyon** | 80+ |
| **Ana Modül (engine_control)** | 40 fonksiyon |
| **Modül 2 (sensor_interface)** | 20+ fonksiyon |
| **Modül 3 (vehicle_diagnostics)** | 20+ fonksiyon |
| **Toplam Satır Kod** | ~2,600 |
| **Domain** | Otomotiv Motor Kontrolü |
| **Karmaşıklık** | Orta - İleri |
| **Test Suitesi** | Ceedling/Unity + CMock |

## Modül Açıklamaları

### 1. Engine Control (Ana Modül)
**Dosyalar:** `engine_control.h`, `engine_control.c`

Motor kontrol sistemi için 40 fonksiyon:
- Motor başlatma/kapanma (5)
- RPM kontrolü (8)
- Yakıt enjeksiyonu (7)
- Sıcaklık kontrolü (6)
- Turbo sistemi (4)
- Tutuşturma (5)
- Motor durumu & sensörler (5)
- Hata yönetimi (4)

**Bağımlılıklar:**
- `sensor_interface.h` → Sensör okuma/yazma
- `vehicle_diagnostics.h` → Hata teşhisi

**Test Zorluk:** ⭐⭐⭐⭐⭐

### 2. Sensor Interface (Modül 2)
**Dosyalar:** `sensor_interface.h`, `sensor_interface.c`

20+ sensör yönetim fonksiyonu:
- RPM sensörü (4)
- Sıcaklık sensörü (4)
- Yakıt basıncı (4)
- Yakıt enjeksiyonu (3)
- Oksijen sensörü (3)
- Turbo kontrol (5)
- Tutuşturma (5)
- Soğutma sistemi (3)
- Hava alımı & throttle (3)
- Toplu işlemler (2)

**Test Zorluk:** ⭐⭐⭐

### 3. Vehicle Diagnostics (Modül 3)
**Dosyalar:** `vehicle_diagnostics.h`, `vehicle_diagnostics.c`

20+ tanı ve hata yönetim fonksiyonu:
- Hata raporlama (5)
- Sistem sağlığı (6)
- Performans analizi (6)
- Tarama & tetikleme (4)
- İstatistik & raporlama (5)
- Uyarı yönetimi (5)
- Kütüphane versiyonu (3)

**Test Zorluk:** ⭐⭐⭐⭐

## Dosya İstatistikleri

| Dosya | Satır | Fonksiyon | Type |
|-------|-------|-----------|------|
| engine_control.h | 400+ | 40 | Header |
| engine_control.c | 600+ | 40 | Implementation |
| sensor_interface.h | 350+ | 20+ | Header |
| sensor_interface.c | 400+ | 20+ | Implementation |
| vehicle_diagnostics.h | 350+ | 20+ | Header |
| vehicle_diagnostics.c | 500+ | 20+ | Implementation |

## Hata Kodu Kategorileri

```
0x1XXX - Engine Control hataları (başlatma, RPM, durum)
0x2XXX - Mod ve durum hataları
0x3XXX - RPM kontrol hataları
0x4XXX - Yakıt enjeksiyonu hataları
0x5XXX - Sıcaklık kontrol hataları
0x6XXX - Turbo sistem hataları
0x7XXX - Tutuşturma sistemi hataları
0x8XXX - Motor durumu ve sensör hataları
0x9XXX - Kritik hata yönetimi hataları
```

## Örnek Fonksiyon Kullanımı

### Motor Başlatma
```c
#include "engine_control.h"

// Motor başlat (initial RPM = 1000)
if (engine_initialize(1000)) {
    // Motor başarıyla başlatıldı
    
    // Hedef RPM ayarla
    engine_set_target_rpm(3000);
    
    // Motor durumunu al
    EngineStatus_t status;
    engine_get_status(&status);
    
    // Sonlandır
    engine_shutdown();
}
```

### Yakıt Kontrol
```c
// Yakıt enjeksiyonunu ayarla
engine_set_fuel_injection_mode(FUEL_INJECTION_NORMAL);
engine_set_fuel_injection_percentage(80);
engine_set_fuel_pressure(350);

// Tüketimi hesapla
float consumption = engine_calculate_fuel_consumption();
```

### Turbo Kontrolü
```c
// Turbo'yu etkinleştir
engine_set_turbo_enabled(true);
engine_set_turbo_boost_pressure(150);

// Durumu kontrol et
uint16_t boost = engine_get_turbo_boost_pressure();
bool health = engine_check_turbo_system_health();
```

## Test Stratejisi

Bu dataset, LLM test geliştirme framework'ü tarafından kullanılmak üzere tasarlanmıştır:

1. **Header Dosyaları Analizi**: Modülün tüm public API'sini öğrenir
2. **Mock Oluşturma**: CMock ile bağımlılıkları mock'lar
3. **Test Tasarımı**: Eşdeğerlik bölümlemesi, sınır değer analizi
4. **Test Kodlama**: Unity framework'ü ile test yazma
5. **Validasyon**: Tüm testler Ceedling ile çalıştırılır

## Diyalog Tabanlı Framework

`DIALOGUE_BASED_TESTING_FRAMEWORK.md` dosyasında **7 adımlı diyalog akışı** bulunur:

1. **Girizgah** - Rol ve görev tanımı
2. **Çevre Tanımlaması** - Header dosyaları ve teknik arayüz
3. **Fonksiyonel Davranış** - İş akışı ve mantık
4. **Test Tasarımı** - Kriterleri ve mock kuralları
5. **Statik Kontrol** - Tutarlılık kontrolü
6. **Bilgi Sıkıştırması** - Final Test Brifingi
7. **Üretim** - Test kodunu yazma

Her adımda **bağlam penceresi sınırları korunarak** bilgi aşama aşama inşa edilir.

## Bağımlılık Grafiği

```
┌─────────────────────────────────────────────┐
│     engine_control (ANA MODÜL)              │
│     40 fonksiyon - Tüm kontrol sistemleri   │
└─────────────────────────────────────────────┘
             ↓                    ↓
    ┌────────────────┐   ┌────────────────────┐
    │ sensor_interface│   │vehicle_diagnostics │
    │   20+ fonksiyon│   │    20+ fonksiyon   │
    └────────────────┘   └────────────────────┘
```

## Test Kapsamı

| Kategori | Sayı |
|----------|------|
| Başarılı Test | 15+ |
| Hata Test | 15+ |
| Sınır Değer Test | 10+ |
| Bağımlılık Mock Test | 15+ |
| **Toplam** | **55+** |

## Parametre Aralıkları

| Parametre | Min | Max | Birim |
|-----------|-----|-----|-------|
| RPM | 500 | 8000 | RPM |
| Sıcaklık | -40 | 150 | °C |
| Yakıt Basıncı | 200 | 600 | kPa |
| Oksijen | 0 | 100 | % |
| Turbo Boost | 0 | 300 | kPa |
| Tutuşturma | -15 | +15 | derece |
| Fan Hızı | 0 | 100 | % |

## Başlamak İçin

1. **Dokümantasyonu Oku**: `README.md`
2. **Modül Detaylarını Öğren**: Her modülün `README.md` dosyasını oku
3. **Header Dosyalarını İncele**: API'nin tamamını anla
4. **Framework'ü Kullan**: `DIALOGUE_BASED_TESTING_FRAMEWORK.md` ile LLM'e prompt ver
5. **Test Kodlarını Oluştur**: LLM tarafından üretilen test kodlarını çalıştır

## İletişim Noktaları

- **API Tanımlaması**: Header dosyaları (`.h`)
- **Implementasyon Detayları**: C dosyaları (`.c`)
- **Modül Özeti**: Her klasördeki `README.md`
- **Test Framework**: `DIALOGUE_BASED_TESTING_FRAMEWORK.md`
- **Genel İnformasyon**: Ana `README.md`

## Notlar

✅ **Dataset Hazır**: Tüm dosyalar oluşturulmuş ve test edilebilir  
✅ **Dokumentasyon Tam**: Tüm fonksiyonlar detaylı açıklandı  
✅ **Realističtir**: Gerçek motor kontrol sistemlerinden ilhamlanmıştır  
✅ **Test Hazır**: Ceedling/Unity + CMock ile test için hazırlandı  

---

**Versiyon:** 1.0  
**Durum:** ✅ TEST HAZIR  
**Oluşturma Tarihi:** 2025

