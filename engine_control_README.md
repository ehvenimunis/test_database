# Engine Control Module (Ana Modül)

## Genel Bilgi

Bu modül, otomotiv motor kontrol sisteminin **ana bileşenidir** ve motor RPM, yakıt enjeksiyonu, tutuşturma zamanlaması, turbo kontrol ve soğutma sistemi gibi tüm kritik fonksiyonları yönetir.

## Özellikler

- **40+ Public Fonksiyon** ile motor kontrol
- **Başlangıç/Kapanma**: Motor başlatma ve güvenli kapanma
- **RPM Kontrolü**: RPM limitlerini, hızlandırma/yavaşlatmayı ve stabiliteyi yönetir
- **Yakıt Enjeksiyonu**: Yakıt basıncı, enjeksiyonu modu ve zamanlaması
- **Sıcaklık Kontrolü**: Motor sıcaklığı, soğutma fanı, otomatik kontrol
- **Turbo Sistemi**: Turbo etkinleştirme, boost basıncı kontrolü
- **Tutuşturma (Ignition)**: Zamanlaması, coil gücü, spark plug kontrolü
- **Durum Yönetimi**: Statik durum değişkenleri ile gerçek zamanlı motor parametreleri
- **Hata Yönetimi**: Kapsamlı hata kodlama ve recovery mekanizması
- **Teşhis Verileri**: Toplam çalışma saati, maksimum/minimum sıcaklık vb.

## Bağımlılıklar

- `sensor_interface.h` - Sensör okuma/yazma işlemleri
- `vehicle_diagnostics.h` - Sistem sağlığı ve hata teşhisi

## Modül Mimarisi

### Veri Yapıları

```c
EngineState_t       - Motor durumu bilgileri
EngineCommand_t     - Motor komutları
EngineStatus_t      - Motor mevcut durumu
ErrorLog_t          - Hata günlüğü
DiagnosticData_t    - Tanı verileri
```

### Fonksiyon Kategorileri

1. **Başlangıç/Kapanma (5)**: `engine_initialize`, `engine_shutdown`, `engine_reset_state`, `engine_emergency_stop`, `engine_set_mode`

2. **RPM Kontrol (8)**: `engine_set_target_rpm`, `engine_get_current_rpm`, `engine_increase_rpm`, `engine_decrease_rpm`, `engine_set_rpm_limit`, `engine_set_rpm_acceleration_rate`, `engine_set_rpm_deceleration_rate`, `engine_check_rpm_stability`

3. **Yakıt Enjeksiyonu (7)**: `engine_set_fuel_injection_mode`, `engine_set_fuel_injection_percentage`, `engine_set_fuel_pressure`, `engine_get_fuel_pressure`, `engine_set_fuel_injection_timing`, `engine_calculate_fuel_consumption`, `engine_check_fuel_system_health`

4. **Sıcaklık Kontrol (6)**: `engine_get_temperature`, `engine_set_max_temperature_limit`, `engine_set_cooling_fan_speed`, `engine_auto_cooling_control`, `engine_calibrate_temperature_sensor`, `engine_check_temperature_warning`

5. **Turbo Kontrol (4)**: `engine_set_turbo_enabled`, `engine_set_turbo_boost_pressure`, `engine_get_turbo_boost_pressure`, `engine_check_turbo_system_health`

6. **Tutuşturma (Ignition) (5)**: `engine_set_ignition_mode`, `engine_set_ignition_timing`, `engine_set_ignition_coil_power`, `engine_check_spark_plug_health`, `engine_check_ignition_system_health`

7. **Motor Durumu & Sensörler (5)**: `engine_get_status`, `engine_apply_command`, `engine_set_air_intake_percentage`, `engine_set_throttle_position`, `engine_get_oxygen_level`

8. **Hata Yönetimi (4)**: `engine_get_last_error`, `engine_clear_error_code`, `engine_attempt_recovery`, `engine_get_diagnostic_data`

9. **Diğer (3)**: `engine_run_for_seconds`, `engine_get_system_version`

## Hata Kodları

Motor kontrolü, `0xXYZZ` formatında hata kodları kullanır:

- `0x1XXX` - Başlangıç/Kapanma hataları
- `0x2XXX` - Mod ve durum hataları
- `0x3XXX` - RPM kontrol hataları
- `0x4XXX` - Yakıt enjeksiyonu hataları
- `0x5XXX` - Sıcaklık kontrol hataları
- `0x6XXX` - Turbo sistem hataları
- `0x7XXX` - Tutuşturma sistemi hataları
- `0x8XXX` - Motor durumu ve sensör hataları
- `0x9XXX` - Kritik hata yönetimi hataları

## Örnek Kullanım

```c
#include "engine_control.h"

// Motor başlat
if (engine_initialize(1000)) {
    // Hedef RPM ayarla
    engine_set_target_rpm(3000);
    
    // Yakıt enjeksiyonunu ayarla
    engine_set_fuel_injection_percentage(80);
    
    // Motor durumunu al
    EngineStatus_t status;
    if (engine_get_status(&status)) {
        // Mevcut RPM: status.current_rpm
    }
    
    // Motor kapat
    engine_shutdown();
}
```

## Test Edilecek Fonksiyonlar

Bu modülün **tüm 40+ fonksiyonu** birim testlerinin konusu olacaktır. Her fonksiyon için:

- Geçerli girdiler ile başarı testleri
- Geçersiz girdiler ile hata testleri
- Sınır değer testleri
- Bağımlılık fonksiyonlarının mock'lanması

## Notlar

- Motor durumu, static değişkenler ile içsel olarak yönetilir
- Sensör sistemi başlatılmadıkça fonksiyonlar çalışmaz
- Tüm komutlar sensör arayüzü üzerinden yazılır
- Hata recovery mekanizması vardır (maksimum 3 deneme)

