# 7 Adımlı Diyalog Tabanlı Test Geliştirme Çerçevesi

## Sistem Tanımı

Bu çerçeve, LLM ile **aşamalı, diyalog tabanlı** bir yaklaşım kullanarak Ceedling/Unity birim testleri üretmek için tasarlanmıştır. Bağlam penceresi limitlerini aşmamak için bilgi **adım adım inşa edilir**.

---

# ADIM 1: GİRİZGAH (Rol ve Görev Tanımı)

## Gönderilecek Mesaj

```
Merhaba. Sen, C dili için Ceedling test çatısını, özellikle de CMock kullanarak 
harici bağımlılıkları mock'layarak ve Unity test koşucusunu kullanarak birim 
testleri yazan uzman bir yazılım test mühendisisin. 

Seninle birlikte, otomotiv motor kontrol sistemi için kapsamlı birim testleri 
geliştireceğiz. Görevi tamamlamak için gerekli tüm bilgileri sana aşama aşama 
sunacağım:

1. Önce, genel bağlamı (header dosyalarını) vereceğim.
2. Sonra, test edilecek spesifik fonksiyonun arayüzünü sunacağım.
3. Ardından, fonksiyonun detaylı işlevsel davranışını ve mantığını aktaracağım.
4. Daha sonra, senden tüm bu bilgilere dayanarak test tasarım kriterlerini ve 
   mock davranış kurallarını türetmeni isteyeceğim.
5. Test kodunu üretmeden önce, senden bu analizine dair bir statik kontrol 
   yapmanı ve her şeyin eksiksiz olduğunu teyit etmeni isteyeceğim.
6. Tüm kontrollerden sonra, senden şimdiye kadarki tüm kritik bilgileri 
   tek bir 'Final Test Brifingi' belgesinde özetleyip sıkıştırmanı isteyeceğim.
7. Son olarak, bu nihai brifinge dayanarak Ceedling/Unity formatında tam bir 
   test kodu üretmeni isteyeceğim.

Her adımdan sonra bilgiyi işlediğini ve bir sonraki adıma hazır olduğunu teyit et. 
Bu göreve hazır mısın?
```

## LLM'den Beklenen Cevap

```
Evet, hazırım! Otomotiv motor kontrol sistemi için birim testleri geliştirmek 
üzere 7 adımlı bu diyalog akışını anladım. Her adımda sana verilecek bilgileri 
dikkatli bir şekilde işleyeceğim ve ilerlemeden önce hazır olduğunu teyit edeceğim. 
Devam et lütfen.
```

---

# ADIM 2: ÇEVREYI TANIMLAMA (Genel Bağlam - Header Dosyaları)

## Gönderilecek Mesaj

```
Harika. Şimdi sana projenin teknik bağlamını sunuyorum. Aşağıda, test edeceğimiz 
engine_control modülünün ve onun bağımlı olduğu sensor_interface ile 
vehicle_diagnostics modüllerinin header dosyaları bulunmaktadır.

Bu dosyalardaki tüm fonksiyon imzalarını ve veri yapılarını analiz et ve 
hafızanda tut. Bu bilgiler, CMock'un mock objeler oluşturması ve senin de 
bağımlılıkları doğru bir şekilde anlaman için temel oluşturacaktır.

Henüz test yazma, sadece bu yapıyı öğren ve içselleştir.

Ayrıca, engine_control.h içerisindeki ENGINE_INITIALIZE ve ENGINE_SET_TARGET_RPM 
fonksiyonlarına odaklanacağız.

=== HEADER DOSYALARI ===

[Aşağıya engine_control.h, sensor_interface.h ve vehicle_diagnostics.h'nin 
tamamı yapıştırılacak]

=== TEKNIK ARAYÜZ ÖZETI ===

Test edilecek modül: engine_control
Fonksiyonlar: 
  - engine_initialize(uint16_t initial_rpm)
  - engine_set_target_rpm(uint16_t target_rpm)

engine_initialize fonksiyonu:
  - Parametreler: initial_rpm (500-1000 arasında valid)
  - Dönüş Değeri: true (başarılı), false (başarısız)
  - Ön-koşullar: Sensör sistemi çalışmıyor olmalı
  - Yan etki: sensor_initialize(), diagnostic_initialize() çağrılır

engine_set_target_rpm fonksiyonu:
  - Parametreler: target_rpm (500-engine_state.max_rpm_limit arasında valid)
  - Dönüş Değeri: true (başarılı), false (başarısız)
  - Ön-koşullar: motor başlatılmış olmalı (engine_initialize)
  - Yan etki: sensor_write_rpm_command() çağrılır

Bu genel bağlamı ve fonksiyonların teknik arayüzünü anladıysan, henüz test yazma 
veya detaylı davranış analizi yapma, sadece bu yapıyı öğren ve içselleştir. 

Bir sonraki adıma hazır olduğunu teyit et.
```

## LLM'den Beklenen Cevap

```
Genel bağlamı ve engine_initialize ile engine_set_target_rpm fonksiyonlarının 
teknik arayüzünü başarıyla analiz ettim. 

Anladıklarım:
- engine_control modülü, sensor_interface ve vehicle_diagnostics modüllerine bağımlı
- engine_initialize: Motor sistemini başlatır, sensör ve tanı sistemlerini initialize eder
- engine_set_target_rpm: Hedef RPM değerini ayarlar ve sensor_write_rpm_command() çağrısı yapar
- Her fonksiyon başarı/başarısızlık durumunda true/false döner
- Parametre aralıkları: initial_rpm (500-1000), target_rpm (500-max_rpm_limit)

Bir sonraki adıma geçmeye hazırım.
```

---

# ADIM 3: FONKSİYONEL DAVRANIŞA HARITA ÇIKMA

## Gönderilecek Mesaj

```
Mükemmel. Şimdi, engine_control.c içerisindeki engine_initialize ve 
engine_set_target_rpm fonksiyonlarının detaylı fonksiyonel davranış ve 
mantık bölümü aşağıdadır.

Bu belgedeki bilgiyi, daha önce edindiğin genel bağlam ve fonksiyonların 
teknik arayüz bilgisiyle birleştirerek içselleştir.

=== FONKSIYONEL DAVRANIŞA HARITA ===

**engine_initialize(uint16_t initial_rpm) - Detaylı İş Akışı:**

1. ÖLÇÜŞ: "engine_state.is_initialized == true" ise hata kodu 0x1001 ile başarısız döner
2. DOĞRULAMA: "initial_rpm < 500 || initial_rpm > 1000" ise hata kodu 0x1002 ile başarısız döner
3. SENSÖR BAŞLATMA: sensor_initialize() çağrılır
   - Başarısız olursa hata kodu 0x1003 ile başarısız döner
4. TANI BAŞLATMA: diagnostic_initialize() çağrılır
   - Başarısız olursa hata kodu 0x1004 ile başarısız döner
5. DURUM GÜNCELLEMESI:
   - engine_state.is_initialized = true
   - engine_state.state = IDLE_STATE
   - engine_state.target_rpm = initial_rpm
   - engine_state.diagnostics.min_temperature_recorded = 150
   - engine_state.diagnostics.max_temperature_recorded = -40
6. SENSÖR OKUMASI: update_engine_state() çağrılır (içsel fonksiyon)
7. DÖNÜŞ: true (başarılı)

**engine_set_target_rpm(uint16_t target_rpm) - Detaylı İş Akışı:**

1. DOĞRULAMA: "target_rpm < 500 || target_rpm > engine_state.max_rpm_limit" 
   ise hata kodu 0x3001 ile başarısız döner
2. RPM AYARLAMA: set_rpm_internal(target_rpm) çağrılır
   - Bu fonksiyon, engine_state.target_rpm'yi ayarlar
   - sensor_write_rpm_command(target_rpm) çağrısı yapar
3. DÖNÜŞ: true (başarılı)

**Ön-koşullar (Pre-conditions):**
- engine_initialize() öncesinde engine_set_target_rpm() başarısız döner (engine başlatılmamış)
- Birden fazla engine_initialize() çağrısı başarısız döner

**Son-koşullar (Post-conditions):**
- engine_initialize() başarılıysa, engine_state.is_initialized true olur
- engine_set_target_rpm() başarılıysa, engine_state.target_rpm güncellenir

**Bağımlılık Çağrıları:**
- engine_initialize:
  - sensor_initialize() - başarılı true döner, başarısız false döner
  - diagnostic_initialize() - başarılı true döner, başarısız false döner
  - update_engine_state() (içsel) - sensor_read_rpm_sensor() vb. çağrılar
  
- engine_set_target_rpm:
  - set_rpm_internal() (içsel) - sensor_write_rpm_command() çağrısı yapar

**Hata Durumları:**
- 0x1001: Zaten başlatılmış
- 0x1002: Geçersiz initial_rpm (500 altı veya 1000 üstü)
- 0x1003: Sensör başlatma başarısız
- 0x1004: Tanı başlatma başarısız
- 0x3001: target_rpm sınırları dışında

Bu davranış bilgisini anladıysan ve bir sonraki adıma geçmek için hazırsan teyit et.
```

## LLM'den Beklenen Cevap

```
engine_initialize ve engine_set_target_rpm fonksiyonlarının detaylı fonksiyonel 
davranışını tam olarak anladım.

Özet:
- engine_initialize: Sistem başlatması, sensör ve tanı sistemlerini çağırır
- engine_set_target_rpm: RPM validasyonu ve sensor_write_rpm_command() çağrısı
- Ön/son koşullar: Başlatma durumuna bağlı
- Bağımlılıklar: sensor_interface ve vehicle_diagnostics modüllerine çağrılar
- Hata kodları: 0x1001, 0x1002, 0x1003, 0x1004, 0x3001

Bir sonraki adıma geçmeye hazırım.
```

---

# ADIM 4: TEST TASARIMI VE MOCK KURALLARINI TÜRETME

## Gönderilecek Mesaj

```
Mükemmel. engine_initialize ve engine_set_target_rpm fonksiyonlarının tüm 
gereksinimlerini ve fonksiyonel davranışını analiz ettim. Şimdi senden, bu 
bilgilere dayanarak aşağıdaki iki bölümü eksiksiz ve detaylı bir şekilde 
üretmeni istiyorum:

=== BÖLÜM 1: TEST TASARIMI KRITERLERI VE KAPSAM ALANLARI ===

**1.1 Test Tasarım Teknikleri:**
Bu fonksiyonları test ederken hangi teknikler kullanılmalı?
(Eşdeğerlik Bölümlemesi, Sınır Değer Analizi, Hata Tahmini vb.)

**1.2 Geçerli Girdi Kategorileri (Eşdeğerlik Sınıfları):**
engine_initialize için:
- initial_rpm parametresinin geçerli aralıkları (500-1000)
- Motor başlatılmamış durumunda (fresh state)

engine_set_target_rpm için:
- target_rpm parametresinin geçerli aralıkları
- Motor başlatılmış durumunda
- Farklı max_rpm_limit değerlerine karşı

**1.3 Geçersiz Girdi Kategorileri (Hata Durumları):**
engine_initialize için:
- initial_rpm < 500 (örn: 0, 100, 499)
- initial_rpm > 1000 (örn: 1001, 5000, 8000)
- Zaten başlatılmış motor

engine_set_target_rpm için:
- target_rpm < 500 (örn: 0, 100, 499)
- target_rpm > max_rpm_limit (örn: 10000)
- Motor başlatılmamış durumunda

**1.4 Sınır Değerleri:**
engine_initialize:
- initial_rpm = 500 (minimum geçerli)
- initial_rpm = 1000 (maksimum geçerli)
- initial_rpm = 499 (sınır altı)
- initial_rpm = 1001 (sınır üstü)

engine_set_target_rpm:
- target_rpm = 500 (minimum)
- target_rpm = 6500 (varsayılan max_rpm_limit)
- target_rpm = 499 (sınır altı)
- target_rpm = 6501 (sınır üstü)

=== BÖLÜM 2: BAĞIMLILIKLARI VE MOCK DAVRANIŞI KURALLAR ===

**2.1 Bağımlı Fonksiyonlar:**
engine_initialize için:
- sensor_initialize() (sensor_interface.h) - bool döner
- diagnostic_initialize() (vehicle_diagnostics.h) - bool döner
- update_engine_state() (içsel) - sensor_read_rpm_sensor() çağrısı yapabilir

engine_set_target_rpm için:
- sensor_write_rpm_command(uint16_t rpm_value) (sensor_interface.h) - bool döner

**2.2 Mock Davranış Mantığı (KESINLIK KURALII: Somut değerler!):**

**engine_initialize için mock kuralları:**

Mock: sensor_initialize()
- Senaryo 1 (Başarı): sensor_initialize() true döner
  → engine_initialize() başarılı olmalı ve true döner
  
- Senaryo 2 (Başarısızlık): sensor_initialize() false döner
  → engine_initialize() 0x1003 hata koduyla başarısız olmalı ve false döner

Mock: diagnostic_initialize()
- Senaryo 1 (Başarı): diagnostic_initialize() true döner
  → engine_initialize() başarılı olmalı ve true döner
  
- Senaryo 2 (Başarısızlık): diagnostic_initialize() false döner
  → engine_initialize() 0x1004 hata koduyla başarısız olmalı ve false döner

**engine_set_target_rpm için mock kuralları:**

Mock: sensor_write_rpm_command()
- Senaryo 1 (Başarı): sensor_write_rpm_command(3000) true döner
  → engine_set_target_rpm(3000) başarılı olmalı ve true döner
  
- Senaryo 2 (Başarısızlık): sensor_write_rpm_command(9000) false döner
  → engine_set_target_rpm(9000) başarısız olmalı ve false döner

Bu iki bölümü, yukarıdaki formatı takip ederek üret.
```

## LLM'den Beklenen Cevap

```
engine_initialize ve engine_set_target_rpm fonksiyonlarının tüm gereksinimlerini 
ve fonksiyonel davranışını analiz ettim. İşte istediğin Test Tasarım Kriterleri 
ve Kapsam Alanları ile Bağımlılıklar ve Mock Davranış Kuralları bölümleri:

=== BÖLÜM 1: TEST TASARIMI KRİTERLERİ VE KAPSAM ALANLARI ===

**1.1 Test Tasarım Teknikleri:**
- Eşdeğerlik Bölümlemesi: Geçerli ve geçersiz RPM aralıkları
- Sınır Değer Analizi: 500, 1000, 499, 1001 değerleri
- Durum Tabanlı Test: Motor başlatılmış/başlatılmamış durumları
- Hata Yolları: Bağımlılık başarısızlıkları (sensor, diagnostic)

**1.2 Geçerli Girdi Kategorileri:**
- engine_initialize: initial_rpm ∈ [500, 1000], fresh state
- engine_set_target_rpm: target_rpm ∈ [500, max_rpm_limit], motor başlatılmış

**1.3 Geçersiz Girdi Kategorileri:**
- engine_initialize: initial_rpm < 500, initial_rpm > 1000, zaten başlatılmış
- engine_set_target_rpm: target_rpm < 500, target_rpm > max_rpm_limit, motor başlatılmamış

**1.4 Sınır Değerleri:**
engine_initialize: 500, 1000, 499, 1001
engine_set_target_rpm: 500, 6500, 499, 6501

=== BÖLÜM 2: BAĞIMLILIKLARI VE MOCK DAVRANIŞI KURALLAR ===

**2.1 Bağımlı Fonksiyonlar:**
- sensor_initialize() → bool döner
- diagnostic_initialize() → bool döner
- sensor_write_rpm_command(uint16_t) → bool döner

**2.2 Mock Davranış Mantığı:**

**sensor_initialize() için:**
- Başarı: true döner → engine_initialize() == true
- Başarısızlık: false döner → engine_initialize() == false (0x1003 hatası)

**diagnostic_initialize() için:**
- Başarı: true döner → engine_initialize() == true
- Başarısızlık: false döner → engine_initialize() == false (0x1004 hatası)

**sensor_write_rpm_command() için:**
- Girdi 3000 kPa: true döner → engine_set_target_rpm(3000) == true
- Girdi 9000 kPa: false döner → engine_set_target_rpm(9000) == false

Test kodunu üretmek için hazırım.
```

---

# ADIM 5: STATIK KONTROL VE TEYIT

## Gönderilecek Mesaj

```
Harika, test tasarım kriterlerini ve mock kurallarını ürettin. Test kodunu 
üretmeden önce son bir statik kontrol yapmanı istiyorum.

Lütfen şimdiye kadar edindiğin tüm bilgileri (header dosyaları, fonksiyon 
arayüzü, iş akışı, test kriterleri ve mock davranış kuralları) karşılaştırarak 
aşağıdaki kontrol listesini yanıtla:

=== STATIK KONTROL LİSTESİ ===

**Soru 1: Bağımlılık Kapsamı**
Fonksiyonların iş akışında belirtilen tüm harici fonksiyon çağrıları 
'Bağımlı Fonksiyonlar' listene dahil edildi mi? Gözden kaçan bir çağrı var mı?
- engine_initialize: sensor_initialize(), diagnostic_initialize() → ✓/✗
- engine_set_target_rpm: sensor_write_rpm_command() → ✓/✗

**Soru 2: Mock Senaryo Derinliği**
'Mock Davranış Mantığı' bölümünde, her bir bağımlı fonksiyon için hem başarılı 
(success) hem de başarısız (failure) durumları kapsayan senaryolar tanımlandı mı?
- sensor_initialize: başarı (true) + başarısızlık (false) → ✓/✗
- diagnostic_initialize: başarı (true) + başarısızlık (false) → ✓/✗
- sensor_write_rpm_command: başarı (true) + başarısızlık (false) → ✓/✗

**Soru 3: Parametre Tutarlılığı**
Mock davranış kurallarında kullanılan parametreler ve dönüş değerlerinin 
tipleri, header dosyalarındaki fonksiyon imzalarıyla tam olarak uyumlu mu?
- sensor_initialize() → bool uyumlu mu? ✓/✗
- diagnostic_initialize() → bool uyumlu mu? ✓/✗
- sensor_write_rpm_command(uint16_t) → bool uyumlu mu? ✓/✗

**Soru 4: Hata Durumu İlişkisi**
Fonksiyonların 'Hata İşleme Mantığı'nda belirtilen her bir hata durumu, 
mock'lanmış bir bağımlılık hatasıyla (dependency failure) veya geçersiz bir 
girdiyle (invalid input) tetiklenecek şekilde test senaryolarında planlandı mı?
- 0x1001 (zaten başlatılmış) → test senaryosu var mı? ✓/✗
- 0x1002 (geçersiz initial_rpm) → test senaryosu var mı? ✓/✗
- 0x1003 (sensor_initialize başarısız) → mock test var mı? ✓/✗
- 0x1004 (diagnostic_initialize başarısız) → mock test var mı? ✓/✗
- 0x3001 (target_rpm sınırları dışında) → test senaryosu var mı? ✓/✗

**Soru 5: Değer Kesinliği**
Mock davranış kurallarındaki tüm dönüş değerleri, yan etkiler ve beklenen 
parametreler, genel ifadeler ('bir değer', 'bir struct') yerine somut ve 
test edilebilir değerler (örn: 5, NULL, true, false, "Error message") 
ile mi tanımlanmış?
- Mock dönüş değerleri somut mu? ✓/✗
- Beklenen parametreler somut mu? ✓/✗

Lütfen bu beş maddeyi kontrol ettiğini, her şeyin eksiksiz ve tutarlı olduğunu onayla.
```

## LLM'den Beklenen Cevap

```
Statik kontrol tamamlandı. Tüm maddeleri kontrol ettim:

**Soru 1: Bağımlılık Kapsamı** ✓
- engine_initialize: sensor_initialize(), diagnostic_initialize() → ✓
- engine_set_target_rpm: sensor_write_rpm_command() → ✓
Tüm çağrılar listelenmiş.

**Soru 2: Mock Senaryo Derinliği** ✓
- sensor_initialize: başarı (true) + başarısızlık (false) → ✓
- diagnostic_initialize: başarı (true) + başarısızlık (false) → ✓
- sensor_write_rpm_command: başarı (true) + başarısızlık (false) → ✓
Tüm senaryolar planlandı.

**Soru 3: Parametre Tutarlılığı** ✓
- sensor_initialize() → bool uyumlu ✓
- diagnostic_initialize() → bool uyumlu ✓
- sensor_write_rpm_command(uint16_t) → bool uyumlu ✓
Tüm imzalar eşleşiyor.

**Soru 4: Hata Durumu İlişkisi** ✓
- 0x1001 (zaten başlatılmış) → test senaryosu var ✓
- 0x1002 (geçersiz initial_rpm) → test senaryosu var ✓
- 0x1003 (sensor_initialize başarısız) → mock test var ✓
- 0x1004 (diagnostic_initialize başarısız) → mock test var ✓
- 0x3001 (target_rpm sınırları dışında) → test senaryosu var ✓
Tüm hatalar test planında.

**Soru 5: Değer Kesinliği** ✓
- Mock dönüş değerleri somut: true, false, 3000, 9000 vb. ✓
- Beklenen parametreler somut: 500, 1000, 499, 1001, 6500 vb. ✓
Tüm değerler spesifik.

Kontrol tamamlandı. Her şey eksiksiz ve tutarlı. Son adıma geçmeye hazırım.
```

---

# ADIM 6: BİLGİ SIKIŞTIRVMA (Final Test Brifingi)

## Gönderilecek Mesaj

```
Mükemmel, tüm kontroller tamamlandı. Şimdi son ve en kritik adıma geçiyoruz: 
Bilgi Sıkıştırma.

Şimdiye kadar konuştuğumuz her şeyi—header dosyaları, fonksiyon arayüzü, iş 
akışı, test kriterleri ve mock kuralları—aşağıdaki şablona uygun olarak, tek 
ve kapsamlı bir 'Final Test Brifingi' belgesinde birleştirmeni istiyorum.

Bu belge, sonraki ve son komutumda test kodunu yazarken başvuracağın tek ve 
nihai kaynak olacaktır.

Lütfen aşağıdaki brifingi eksiksiz doldur:

=== FINAL TEST BRİFİNGİ: engine_initialize ve engine_set_target_rpm ===

**Test Edilecek Fonksiyonlar:**
1. engine_initialize
2. engine_set_target_rpm

**Teknik Arayüz:**

**engine_initialize:**
İmza: bool engine_initialize(uint16_t initial_rpm)
Parametreler: initial_rpm (geçerli aralık: 500-1000)
Dönüş Değeri: true (başarılı), false (başarısız)

**engine_set_target_rpm:**
İmza: bool engine_set_target_rpm(uint16_t target_rpm)
Parametreler: target_rpm (geçerli aralık: 500-max_rpm_limit, default 6500)
Dönüş Değeri: true (başarılı), false (başarısız)

**Temel İş Akışı Özeti:**

**engine_initialize:**
1. Motor zaten başlatılmışsa 0x1001 hatasıyla başarısız döner
2. initial_rpm validasyonu (500-1000 aralığı)
3. sensor_initialize() çağrısı, başarısızsa 0x1003 hatası
4. diagnostic_initialize() çağrısı, başarısızsa 0x1004 hatası
5. Motor durumu başlatılır (is_initialized=true, state=IDLE_STATE)
6. Sensör durumu güncellenir ve true döner

**engine_set_target_rpm:**
1. target_rpm validasyonu (500-max_rpm_limit aralığı)
2. Sınırları dışındaysa 0x3001 hatasıyla başarısız döner
3. sensor_write_rpm_command(target_rpm) çağrısı
4. Başarılıysa true döner, başarısızsa false döner

**Bağımlılıklar ve Kesin Mock Kuralları:**

**sensor_initialize():**
- Başarı: true döner
  → engine_initialize() devam eder, diagnostic_initialize() çağrısına geçer
- Başarısızlık: false döner
  → engine_initialize() 0x1003 hatasıyla false döner

**diagnostic_initialize():**
- Başarı: true döner
  → engine_initialize() başarılı olur ve true döner
- Başarısızlık: false döner
  → engine_initialize() 0x1004 hatasıyla false döner

**sensor_write_rpm_command(uint16_t rpm_value):**
- rpm_value=3000: true döner
  → engine_set_target_rpm(3000) true döner
- rpm_value=9000: false döner (veya engine tarafından false yapılır)
  → engine_set_target_rpm(9000) false döner

**Temel Test Senaryoları (Kapsam):**

**engine_initialize testleri:**
1. Başarılı motor başlatması: initial_rpm=750, sensor_initialize=true, diagnostic_initialize=true
   → Beklenen sonuç: true
   
2. Zaten başlatılmış hata: engine_initialize() iki kez çağrılır
   → İkinci çağrı false döner (0x1001)
   
3. Geçersiz minimum initial_rpm: initial_rpm=499
   → Beklenen sonuç: false (0x1002)
   
4. Geçersiz maksimum initial_rpm: initial_rpm=1001
   → Beklenen sonuç: false (0x1002)
   
5. Sensör başlatma başarısız: sensor_initialize()=false
   → Beklenen sonuç: false (0x1003)
   
6. Tanı başlatma başarısız: diagnostic_initialize()=false
   → Beklenen sonuç: false (0x1004)
   
7. Sınır değer testleri: initial_rpm=500, initial_rpm=1000
   → Beklenen sonuç: true

**engine_set_target_rpm testleri:**
1. Başarılı RPM ayarlaması: target_rpm=3000, motor başlatılmış, sensor_write_rpm_command=true
   → Beklenen sonuç: true
   
2. Geçersiz minimum target_rpm: target_rpm=499
   → Beklenen sonuç: false (0x3001)
   
3. Geçersiz maksimum target_rpm: target_rpm=6501 (max_rpm_limit=6500)
   → Beklenen sonuç: false (0x3001)
   
4. Sensör yazma başarısız: sensor_write_rpm_command=false
   → Beklenen sonuç: false
   
5. Sınır değer testleri: target_rpm=500, target_rpm=6500
   → Beklenen sonuç: true

Bu özeti oluşturduktan sonra, başka bir şey yapmadan sadece onayı bekle.
```

## LLM'den Beklenen Cevap

```
Anlaşıldı. İşte engine_initialize ve engine_set_target_rpm için istenen 
'Final Test Brifingi':

=== FINAL TEST BRİFİNGİ: engine_initialize ve engine_set_target_rpm ===

[Yukarıdaki şablonun tamamen doldurulmuş versiyonu]

Özet tamamlanmıştır. Son komut bekleniyor.
```

---

# ADIM 7: ÜRETIM KOMUTU (Test Kodunu Yaz!)

## Gönderilecek Mesaj

```
Harika. Lütfen şimdiye kadar verdiğim tüm bilgileri (rol tanımı, header 
dosyaları, fonksiyon arayüzü, iş akışı, test tasarım kriterleri ve mock 
davranış kuralları) kullanarak, engine_initialize ve engine_set_target_rpm 
fonksiyonları için Ceedling/Unity formatında tüm test senaryolarını şimdi oluştur.

**Test Dosyası Adı:** test_engine_control_init_rpm.c

**Gereksinimler:**
1. Unity framework'ü kullanarak test fonksiyonları oluştur
2. CMock kullanarak sensor_interface ve vehicle_diagnostics fonksiyonlarını mock'la
3. setUp() ve tearDown() fonksiyonlarını ekle
4. Her test senaryosu için açıklayıcı isim ver
5. Beklenen sonuçları assert ile doğrula
6. Mock davranış kurallarını ExpectAndReturn() ile kur

**Test Kategorileri:**
- engine_initialize başarılı testleri
- engine_initialize hata testleri
- engine_initialize sınır değer testleri
- engine_set_target_rpm başarılı testleri
- engine_set_target_rpm hata testleri
- engine_set_target_rpm sınır değer testleri

Test kodunu şimdi oluştur.
```

## LLM'den Beklenen Cevap

```c
#include "unity.h"
#include "engine_control.h"
#include "mock_sensor_interface.h"
#include "mock_vehicle_diagnostics.h"

void setUp(void) {
    // Her test öncesinde yapılacak işlemler
}

void tearDown(void) {
    // Her test sonrasında yapılacak işlemler
}

// =====================================================================
// engine_initialize BAŞARILI TESTLER
// =====================================================================

void test_engine_initialize_with_valid_rpm_750(void) {
    // Arrange: Mock fonksiyonları setup et
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    
    // Act: motor başlat
    bool result = engine_initialize(750);
    
    // Assert: başarılı olmalı
    TEST_ASSERT_TRUE(result);
}

void test_engine_initialize_with_minimum_valid_rpm_500(void) {
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    
    bool result = engine_initialize(500);
    
    TEST_ASSERT_TRUE(result);
}

void test_engine_initialize_with_maximum_valid_rpm_1000(void) {
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    
    bool result = engine_initialize(1000);
    
    TEST_ASSERT_TRUE(result);
}

// =====================================================================
// engine_initialize HATA TESTLER
// =====================================================================

void test_engine_initialize_with_below_minimum_rpm_499(void) {
    // Arrange: Mock fonksiyonları setup et (çağrılmayacaklar)
    // sensor_initialize çağrılmaması lazım çünkü parametre validasyonu başarısız
    
    // Act: Geçersiz RPM ile başlat
    bool result = engine_initialize(499);
    
    // Assert: başarısız olmalı
    TEST_ASSERT_FALSE(result);
}

void test_engine_initialize_with_above_maximum_rpm_1001(void) {
    bool result = engine_initialize(1001);
    TEST_ASSERT_FALSE(result);
}

void test_engine_initialize_with_zero_rpm(void) {
    bool result = engine_initialize(0);
    TEST_ASSERT_FALSE(result);
}

void test_engine_initialize_when_sensor_initialization_fails(void) {
    // Arrange: sensor_initialize false döner
    sensor_initialize_ExpectAndReturn(false);
    
    // Act
    bool result = engine_initialize(750);
    
    // Assert: başarısız olmalı (0x1003)
    TEST_ASSERT_FALSE(result);
}

void test_engine_initialize_when_diagnostic_initialization_fails(void) {
    // Arrange: sensor_initialize başarılı, diagnostic başarısız
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(false);
    
    // Act
    bool result = engine_initialize(750);
    
    // Assert: başarısız olmalı (0x1004)
    TEST_ASSERT_FALSE(result);
}

void test_engine_initialize_called_twice_second_call_fails(void) {
    // Arrange: İlk çağrı
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    
    // Act: İlk başlatma
    bool first_result = engine_initialize(750);
    TEST_ASSERT_TRUE(first_result);
    
    // Act: İkinci başlatma (motor zaten başlatılmış, 0x1001 hatası)
    bool second_result = engine_initialize(750);
    
    // Assert: ikinci çağrı başarısız olmalı
    TEST_ASSERT_FALSE(second_result);
}

// =====================================================================
// engine_set_target_rpm BAŞARILI TESTLER
// =====================================================================

void test_engine_set_target_rpm_with_valid_3000(void) {
    // Arrange: Önce motor başlat
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act: Hedef RPM set et
    sensor_write_rpm_command_ExpectAndReturn(3000, true);
    bool result = engine_set_target_rpm(3000);
    
    // Assert: başarılı olmalı
    TEST_ASSERT_TRUE(result);
}

void test_engine_set_target_rpm_with_minimum_valid_500(void) {
    // Arrange: Motor başlat
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act
    sensor_write_rpm_command_ExpectAndReturn(500, true);
    bool result = engine_set_target_rpm(500);
    
    // Assert
    TEST_ASSERT_TRUE(result);
}

void test_engine_set_target_rpm_with_maximum_valid_6500(void) {
    // Arrange: Motor başlat (default max_rpm_limit = 6500)
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act
    sensor_write_rpm_command_ExpectAndReturn(6500, true);
    bool result = engine_set_target_rpm(6500);
    
    // Assert
    TEST_ASSERT_TRUE(result);
}

// =====================================================================
// engine_set_target_rpm HATA TESTLER
// =====================================================================

void test_engine_set_target_rpm_with_below_minimum_499(void) {
    // Arrange: Motor başlat
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act: Geçersiz RPM (499 < 500)
    bool result = engine_set_target_rpm(499);
    
    // Assert: başarısız olmalı (0x3001)
    TEST_ASSERT_FALSE(result);
}

void test_engine_set_target_rpm_with_above_maximum_6501(void) {
    // Arrange: Motor başlat
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act: Geçersiz RPM (6501 > max_rpm_limit=6500)
    bool result = engine_set_target_rpm(6501);
    
    // Assert: başarısız olmalı (0x3001)
    TEST_ASSERT_FALSE(result);
}

void test_engine_set_target_rpm_with_zero(void) {
    // Arrange: Motor başlat
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act
    bool result = engine_set_target_rpm(0);
    
    // Assert: başarısız olmalı
    TEST_ASSERT_FALSE(result);
}

void test_engine_set_target_rpm_when_sensor_write_fails(void) {
    // Arrange: Motor başlat
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act: sensor_write_rpm_command başarısız döner
    sensor_write_rpm_command_ExpectAndReturn(3000, false);
    bool result = engine_set_target_rpm(3000);
    
    // Assert: başarısız olmalı
    TEST_ASSERT_FALSE(result);
}

// =====================================================================
// engine_set_target_rpm YENİDEN ÇAĞRI TESTLER
// =====================================================================

void test_engine_set_target_rpm_multiple_calls_sequential(void) {
    // Arrange: Motor başlat
    sensor_initialize_ExpectAndReturn(true);
    diagnostic_initialize_ExpectAndReturn(true);
    engine_initialize(750);
    
    // Act & Assert: Ardışık çağrılar
    sensor_write_rpm_command_ExpectAndReturn(2000, true);
    TEST_ASSERT_TRUE(engine_set_target_rpm(2000));
    
    sensor_write_rpm_command_ExpectAndReturn(4000, true);
    TEST_ASSERT_TRUE(engine_set_target_rpm(4000));
    
    sensor_write_rpm_command_ExpectAndReturn(3000, true);
    TEST_ASSERT_TRUE(engine_set_target_rpm(3000));
}

void test_engine_set_target_rpm_after_initialization_failure(void) {
    // Arrange: Motor başlatma başarısız
    sensor_initialize_ExpectAndReturn(false);
    
    engine_initialize(750);  // Bu başarısız olacak
    
    // Act: engine_set_target_rpm çağrı
    bool result = engine_set_target_rpm(3000);
    
    // Assert: Motor başlatılmadığı için başarısız olmalı
    TEST_ASSERT_FALSE(result);
}
```

---

## Özet

Bu 7 adımlı diyalog tabanlı çerçeve şunları sağlar:

1. **Adım 1**: LLM'nin rol ve görevini tanıdığı
2. **Adım 2**: Genel teknik bağlamı öğrendiği
3. **Adım 3**: Fonksiyonel davranışı anladığı
4. **Adım 4**: Test tasarımı ve mock kurallarını türettiği
5. **Adım 5**: Tutarlılığı kontrol ettiği
6. **Adım 6**: Tüm bilgiyi sıkıştırdığı
7. **Adım 7**: Test kodunu ürettiği

Her adım, LLM'nin bağlam penceresi sınırları içinde kalarak, görevi adım adım tamamlamasını sağlar. **Progressif bilgi yüklemesi** (Progressive Information Loading) yaklaşımı kullanılır.

