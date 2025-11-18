# Sensor Interface Module (Modül 2)

## Genel Bilgi

Bu modül, tüm sensörlerin **okuma ve yazma işlemlerinin arayüzü**ni sağlar. Motor kontrol sistemi bu modül üzerinden motor sensörlerine komut gönderir ve sensörlerden veri okur.

## Özellikler

- **20+ Public Fonksiyon** ile sensör yönetimi
- **RPM Sensörü**: Okuma, komut yazma, kalibrasyon
- **Sıcaklık Sensörü**: Okuma, kalibrasyon, limit ayarı
- **Yakıt Basıncı Sensörü**: Okuma, komut yazma, kalibrasyon
- **Yakıt Enjeksiyonu**: Enjeksiyonu yüzdesi, akış hızı ölçümü
- **Oksijen Sensörü**: Oksijen seviyesi ölçümü, kalibrasyon
- **Turbo Sistemi**: Turbo etkinleştirme, boost basıncı kontrolü
- **Tutuşturma (Ignition)**: Zamanlaması, coil gücü, spark plug
- **Soğutma Sistemi**: Fanı kontrol, sistem durumu
- **Hava Alımı & Throttle**: Hava alımı ve throttle konumu kontrolü
- **Toplu İşlemler**: Tüm sensörlerden tek seferde okuma

## Bağımlılıklar

Bağımsız modül olarak tasarlanmış. Engine Control modülü tarafından kullanılır.

## Modül Mimarisi

### Veri Yapıları

```c
SensorStatus_t      - Sensör durumu bilgileri
SensorReadings_t    - Tüm sensörlerden okunan değerler
```

### Fonksiyon Kategorileri

1. **Başlangıç/Kapanma (3)**: `sensor_initialize`, `sensor_shutdown`, `sensor_calibrate_all`

2. **RPM Sensörü (4)**: `sensor_read_rpm_sensor`, `sensor_write_rpm_command`, `sensor_check_rpm_sensor_status`, `sensor_calibrate_rpm_sensor`

3. **Sıcaklık Sensörü (4)**: `sensor_read_temperature_sensor`, `sensor_check_temperature_sensor_status`, `sensor_calibrate_temperature_sensor`, `sensor_set_temperature_limits`

4. **Yakıt Basıncı Sensörü (4)**: `sensor_read_fuel_pressure_sensor`, `sensor_write_fuel_pressure_command`, `sensor_check_fuel_pressure_status`, `sensor_calibrate_fuel_pressure_sensor`

5. **Yakıt Enjeksiyonu (3)**: `sensor_write_fuel_injection_command`, `sensor_check_fuel_sensor_status`, `sensor_measure_fuel_flow_rate`

6. **Oksijen Sensörü (3)**: `sensor_read_oxygen_sensor`, `sensor_check_oxygen_sensor_status`, `sensor_calibrate_oxygen_sensor`

7. **Turbo Kontrol (5)**: `sensor_enable_turbo`, `sensor_disable_turbo`, `sensor_write_turbo_boost_command`, `sensor_check_turbo_sensor_status`, `sensor_measure_boost_pressure`

8. **Tutuşturma (5)**: `sensor_write_ignition_timing_command`, `sensor_write_ignition_coil_command`, `sensor_check_spark_plug_status`, `sensor_check_ignition_system_status`, `sensor_calibrate_ignition_system`

9. **Soğutma Sistemi (3)**: `sensor_write_cooling_fan_command`, `sensor_check_cooling_system_status`, `sensor_measure_coolant_temperature`

10. **Hava Alımı & Throttle (3)**: `sensor_write_air_intake_command`, `sensor_write_throttle_command`, `sensor_measure_air_flow_rate`

11. **Toplu İşlemler (2)**: `sensor_read_all_sensors`, `sensor_check_all_sensor_status`

## Sensör Değer Aralıkları

| Sensör | Min | Max | Birim |
|--------|-----|-----|-------|
| RPM | 0 | 8000 | RPM |
| Sıcaklık | -40 | 150 | °C |
| Yakıt Basıncı | 200 | 600 | kPa |
| Oksijen | 0 | 100 | % |
| Turbo Boost | 0 | 300 | kPa |
| Tutuşturma | -15 | +15 | derece |
| Fanı | 0 | 100 | % |
| Hava Alımı | 10 | 100 | % |
| Throttle | 0 | 100 | % |

## Örnek Kullanım

```c
#include "sensor_interface.h"

// Sensörleri başlat
if (sensor_initialize()) {
    // RPM sensöründen oku
    uint16_t rpm = sensor_read_rpm_sensor();
    
    // Sıcaklık sensöründen oku
    int16_t temp = sensor_read_temperature_sensor();
    
    // Yakıt basıncı komutunu gönder
    sensor_write_fuel_pressure_command(350);
    
    // Tüm sensörlerden oku
    SensorReadings_t readings;
    if (sensor_read_all_sensors(&readings)) {
        // readings.rpm_value, readings.temperature_value vb.
    }
    
    sensor_shutdown();
}
```

## Durum Yönetimi

Modül, sensör değerlerini static değişkenlerde depolar ve bu değerleri:

- Yazma komutları ile günceller
- Okuma fonksiyonları ile sunar
- Kalibrasyon işlemleri ile ayarlar

Gerçek bir sistemde, bu değerler donanım sensörlerinden okunur.

## Hata Yönetimi

Hata durumları:

- Sensör başlatılmamış: Fonksiyonlar `false` döner
- Geçersiz parametre: Kalibrasyon değerleri kontrol edilir
- Sınır dışı değer: Yazma fonksiyonları `false` döner

## Notlar

- Tüm sensör operasyonları `sensor_initialize()` çağrısından sonra çalışır
- Kalibrasyon fonksiyonları, sensör değerlerini referans değerleri ile ayarlar
- Toplu okuma işlemleri, tüm sensörlerin anlık değerlerini sunar

