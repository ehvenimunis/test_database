import os
# Python 3.14+ ile torch.compile uyumsuzluğunu önlemek için devre dışı bırak
os.environ["PYTORCH_DISABLE_COMPILE"] = "1"

import torch
# torch.compile'ı tamamen devre dışı bırak - Python 3.14+ uyumluluğu için
if hasattr(torch, '_dynamo'):
    torch._dynamo.config.suppress_errors = True
    torch._dynamo.config.disable = True

# torch.compile fonksiyonunu monkey-patch ile devre dışı bırak
if hasattr(torch, 'compile'):
    def _disabled_compile(*args, **kwargs):
        # torch.compile çağrıldığında sadece ilk argümanı döndür (hiçbir şey yapma)
        if args:
            return args[0]
        return None
    torch.compile = _disabled_compile

from transformers import AutoTokenizer, AutoModelForCausalLM
from peft import PeftModel
import sys

# ==============================================================================
# AYARLAR (Burayı Kendi Bilgisayarına Göre Düzenle)
# ==============================================================================
# Model klasörünün TAM yolunu buraya yapıştır:
adapter_path = r"C:\AI_Models\starcoder2-ceedling-expert-final" 

base_model_name = "bigcode/starcoder2-3b"

# ==============================================================================
# MODEL YÜKLEME
# ==============================================================================
print(f" Sistem: {torch.cuda.get_device_name(0)}")
print(" Temel model yükleniyor (12GB VRAM yeterli, FP16 kullanılıyor)...")

# Senin kartın güçlü olduğu için 4-bit sıkıştırma yapmadan FP16 (Yarım Hassasiyet) açıyoruz.
# Bu hem Windows'ta daha sorunsuz çalışır hem de daha kalitelidir.
base_model = AutoModelForCausalLM.from_pretrained(
    base_model_name,
    dtype=torch.bfloat16,
    device_map="auto"
)

tokenizer = AutoTokenizer.from_pretrained(base_model_name)
tokenizer.pad_token = tokenizer.eos_token

print(f" Eğittiğin LoRA adaptörü ({adapter_path}) bağlanıyor...")
try:
    model = PeftModel.from_pretrained(base_model, adapter_path)
except Exception as e:
    print(f"\n HATA: Model klasörü bulunamadı veya bozuk.\nDetay: {e}")
    sys.exit()

print("\n" + "="*50)
print(" MODEL BİLGİSAYARINDA ÇALIŞIYOR!")
print("="*50)

# ==============================================================================
# PROMPT ŞABLONLARI - 7 ADIMLI SÜREÇ
# ==============================================================================

def get_step1_prompt():
    """Adım 1: Girizgah (Rol ve Görev Tanımı)"""
    return """Merhaba. Sen, C dili için Ceedling test çatısını, özellikle de CMock kullanarak harici bağımlılıkları mock'layarak ve Unity test koşucusunu kullanarak birim testleri yazan uzman bir yazılım test mühendisisin. Seninle birlikte bir C modülü için kapsamlı birim testleri geliştireceğiz. Görevi tamamlaman için gerekli tüm bilgileri sana aşama aşama sunacağım:

Önce, genel bağlamı (header dosyalarını) vereceğim.
Sonra, test edilecek spesifik fonksiyonun arayüzünü sunacağım.
Ardından, fonksiyonun detaylı işlevsel davranışını ve mantığını aktaracağım.
Daha sonra, senden tüm bu bilgilere dayanarak test tasarım kriterlerini ve mock davranış kurallarını türetmeni isteyeceğim.
Test kodunu üretmeden önce, senden bu analizine dair bir statik kontrol yapmanı ve her şeyin eksiksiz olduğunu teyit etmeni isteyeceğim.
Tüm kontrollerden sonra, senden şimdiye kadarki tüm kritik bilgileri tek bir 'Final Test Brifingi' belgesinde özetleyip sıkıştırmanı isteyeceğim.
Son olarak, bu nihai brifinge dayanarak Ceedling/Unity formatında tam bir test kodu üretmeni isteyeceğim.

KRİTİK UYARI: Bu adımda SADECE ve SADECE şu cümleyi yaz: "Evet, hazırım. Lütfen ilk adıma geçelim."
Başka HİÇBİR şey yazma. Kod yazma, enum tanımlama, fonksiyon yazma, açıklama yapma. Sadece bu cümleyi yaz."""

def get_step2_prompt(header_files_content, function_name, function_interface_doc):
    """Adım 2: Çevreyi Tanımlama (Genel Bağlam - Header Dosyaları ve Fonksiyonun Genel Tanımı)"""
    return f"""Harika. Şimdi sana projenin teknik bağlamını sunuyorum. Aşağıda, test edeceğimiz modülün ve onun bağımlı olduğu modüllerin header dosyaları bulunmaktadır. Bu dosyalardaki tüm fonksiyon imzalarını ve veri yapılarını analiz et ve hafızanda tut. Bu bilgiler, CMock'un mock objeler oluşturması ve senin de bağımlılıkları doğru bir şekilde anlaman için temel oluşturacaktır.

{header_files_content}

Ayrıca, {function_name} fonksiyonuna odaklanacağız. Sadece bu fonksiyon için hazırlanmış detaylı tasarım belgesinin ilk kısmı aşağıdadır:

Kapsamlı Detay Tasarım Belgesi: {function_name} (Bölüm 1 ve Bölüm 2)

{function_interface_doc}

KRİTİK UYARI: Bu adımda SADECE ve SADECE şu cümleyi yaz: "Genel bağlamı ve {function_name} fonksiyonunun teknik arayüzünü başarıyla analiz ettim. Bir sonraki adıma geçmeye hazırım."
Başka HİÇBİR şey yazma. Kod yazma, enum tanımlama, typedef yazma, fonksiyon yazma, açıklama yapma. Sadece bu cümleyi yaz."""

def get_step3_prompt(function_name, functional_behavior_doc):
    """Adım 3: Fonksiyonel Davranış ve Mantık (Spesifik Fonksiyon Detay Tasarımı)"""
    return f"""Mükemmel. Şimdi, {function_name} fonksiyonunun detaylı fonksiyonel davranış ve mantık bölümü aşağıdadır. Bu belgedeki bilgiyi, daha önce edindiğin genel bağlam ve {function_name} fonksiyonunun teknik arayüz bilgisiyle birleştirerek içselleştir. Fonksiyon içindeki static veya internal durum değişkenlerine doğrudan erişilemeyeceği için, bu değişkenlerin etkileri yalnızca gözlemlenebilir sonuçlar (örneğin dönüş değerleri veya çağrı sıraları) üzerinden test edilmelidir.

Kapsamlı Detay Tasarım Belgesi: {function_name} (Bölüm 3: Fonksiyonel Davranış ve Mantık)

{functional_behavior_doc}

KRİTİK UYARI: Bu adımda SADECE ve SADECE şu cümleyi yaz: "{function_name} fonksiyonunun detaylı fonksiyonel davranışını ve içsel mantığını tam olarak anladım. Bir sonraki adıma geçmeye hazırım."
Başka HİÇBİR şey yazma. Kod yazma, enum tanımlama, typedef yazma, fonksiyon yazma, açıklama yapma. Sadece bu cümleyi yaz."""

def get_step4_prompt(function_name):
    """Adım 4: Test Tasarım Kriterleri ve Mock Davranış Kurallarını Türetme"""
    return f"""Mükemmel. {function_name} için tüm genel bağlam, arayüz ve detaylı fonksiyonel davranış bilgilerine sahipsin. Şimdi senden, bu bilgilere dayanarak, {function_name} için aşağıdaki iki bölümü eksiksiz ve detaylı bir şekilde üretmeni istiyorum:

Bölüm 3: Test Tasarım Kriterleri ve Kapsam Alanları

3.1. Test Tasarım Teknikleri: Bu fonksiyonu test ederken [buraya Eşdeğerlik Bölümlemesi, Sınır Değer Analizi, Hata Tahmini vb. uygun teknikleri belirle] tekniklerini kullanmalısın.

3.2. Geçerli Girdi Kategorileri (Eşdeğerlik Sınıfları): [{function_name} için geçerli girdi senaryolarını maddeler halinde listele. Örn: param_a parametresinin 1 ile 100 arasında bir değer aldığı durumlar.]

3.3. Geçersiz Girdi Kategorileri (Hata Durumları): [{function_name} için geçersiz girdi senaryolarını veya hata durumlarını maddeler halinde listele. Örn: param_b parametresine NULL gönderilmesi.]

3.4. Analiz Edilecek Sınır Değerleri: [{function_name} fonksiyonunun parametreleri için kritik sınır değerlerini maddeler halinde listele. Örn: param_a değişkenine 0, 1, 100 ve 101 değerlerini ver.]

Bölüm 4: Bağımlılıklar ve Mock Davranış Kuralları

4.1. Bağımlı Fonksiyonlar: [{function_name} fonksiyonunun çağırdığı tüm bağımlı fonksiyonları listele, mümkünse header dosyası/modül adını da belirt. Örn: example_func_1(int input_val) (example2.h)]

4.2. Mock Davranış Mantığı:

Bu bölümde, aşağıdaki 'Kesinlik Kuralı'na uymalısın: Her mock kuralı, genel bir tanım ('bir struct döndürür') yerine, spesifik ve somut değerler içermelidir. Her senaryo için girdi parametrelerinin tam değerlerini ve beklenen dönüş değerinin veya yan etkinin tam karşılığını belirtmelisin.

- example_func_1 fonksiyonu için mock oluştururken:

- Girdi olarak [belli bir değer, örn: 100] verildiğinde, [beklenen çıktı/davranışı, örn: xxxxx bilgilerini içeren bir example_struct döndürmelidir].

- Girdi olarak [başka bir değer/durum, örn: 100 verilmediğinde] [başka bir beklenen çıktı/davranışı, örn: NULL döndürmelidir].

- example_func_2 fonksiyonu için mock oluştururken:

- [Bu bağımlı fonksiyon için mock davranış kurallarını detaylıca listele. Örn: {function_name} fonksiyonuna geçersiz bir parametre (örn: NULL) gönderildiğinde bu fonksiyonun çağrıldığını doğrulamalısın.]

- [Gerekiyorsa diğer bağımlı fonksiyonlar için de aynı formatı kullan.]

Bu iki bölümü, yukarıdaki formatı takip ederek ve {function_name} fonksiyonuna özel detaylarla doldurarak oluşturmanı bekliyorum.

KRİTİK UYARI: Bu adımda SADECE test tasarım kriterlerini ve mock kurallarını METİN olarak üret. 
- Kod yazma
- Enum tanımlama  
- Typedef yazma
- Fonksiyon yazma
- C kodu üretme
Sadece analiz ve planlama METNİ üret. Yukarıdaki şablonu doldur."""

def get_step5_prompt():
    """Adım 5: Statik Kontrol ve Teyit"""
    return """Harika, test tasarım kriterlerini ve mock kurallarını ürettin. Test kodunu üretmeden önce son bir statik kontrol yapmanı istiyorum. Lütfen şimdiye kadar edindiğin tüm bilgileri (header dosyaları, fonksiyon arayüzü, iş akışı) ve kendi ürettiğin 'Bölüm 4: Bağımlılıklar ve Mock Davranış Kuralları'nı karşılaştırarak aşağıdaki kontrol listesini yanıtla:

Statik Kontrol Listesi:

Bağımlılık Kapsamı: Fonksiyonun iş akışında belirtilen tüm harici fonksiyon çağrıları 'Bağımlı Fonksiyonlar' listene dahil edildi mi? Gözden kaçan bir çağrı var mı?

Mock Senaryo Derinliği: 'Mock Davranış Mantığı' bölümünde, her bir bağımlı fonksiyon için hem başarılı (success) hem de başarısız (failure) durumları kapsayan senaryolar tanımlandı mı? (örn: example_func_1'in hem geçerli bir struct döndürdüğü hem de NULL döndürdüğü durumlar.)

Parametre Tutarlılığı: Mock davranış kurallarında kullanılan parametreler ve dönüş değerlerinin tipleri, Adım 2'de verilen header dosyalarındaki fonksiyon imzalarıyla tam olarak uyumlu mu?

Hata Durumu İlişkisi: Fonksiyonun 'Hata İşleme Mantığı'nda belirtilen her bir hata durumu, mock'lanmış bir bağımlılık hatasıyla (dependency failure) veya geçersiz bir girdiyle (invalid input) tetiklenecek şekilde test senaryolarında planlandı mı?

Değer Kesinliği (Value Precision): Mock davranış kurallarındaki tüm dönüş değerleri, yan etkiler ve beklenen parametreler, genel ifadeler ('bir değer', 'bir struct') yerine somut ve test edilebilir değerler (örn: 5, NULL, {'id': 123, 'status': true}, "Error message") ile mi tanımlanmış?

Lütfen bu beş maddeyi kontrol ettiğini, her şeyin eksiksiz ve tutarlı olduğunu onayla. Eğer bir eksiklik fark edersen, şimdi düzelt. Onayının ardından son üretim komutunu vereceğim.

KRİTİK UYARI: Bu adımda SADECE statik kontrol sonuçlarını METİN olarak raporla. Her madde için kısa bir açıklama yap (örn: "Bağımlılık Kapsamı: Evet, tüm fonksiyon çağrıları listelenmiştir.").
- Kod yazma
- Enum tanımlama
- Typedef yazma
- Fonksiyon yazma
Sadece kontrol sonuçlarını METİN olarak raporla."""

def get_step6_prompt(function_name):
    """Adım 6: Bilgi Sıkıştırma (Re-compress & Final Briefing)"""
    return f"""Mükemmel, tüm kontroller tamamlandı. Şimdi son ve en kritik adıma geçiyoruz: Bilgi Sıkıştırma.

Şimdiye kadar konuştuğumuz her şeyi—header dosyaları, fonksiyon arayüzü, iş akışı, test kriterleri ve mock kuralları—aşağıdaki şablona uygun olarak, tek ve kapsamlı bir 'Final Test Brifingi' belgesinde birleştirmeni istiyorum. Bu belge, bir sonraki ve son komutumda test kodunu yazarken başvuracağın tek ve nihai kaynak olacaktır.

Lütfen aşağıdaki brifingi eksiksiz doldur:

Final Test Brifingi: {function_name}

Test Edilecek Fonksiyon:

{function_name}

Teknik Arayüz:

İmza: [Fonksiyonun tam imzası, örn: int example_func(int param_a, ...)]

Parametreler: [Parametrelerin kısa açıklamaları ve kısıtlamaları]

Dönüş Değeri: [Dönüş değerlerinin anlamları, örn: 0=Başarı, -1=Hata]

Temel İş Akışı Özeti:

[İş akışının 3-4 maddelik özeti, örn: Parametreleri doğrular, example_func_1'i çağırır, dönen değere göre example_func_2'yi çağırır veya başarılı döner.]

Bağımlılıklar ve Kesin Mock Kuralları:

[Her bağımlı fonksiyon için başarı ve hata senaryolarını somut değerlerle belirt]

Temel Test Senaryoları (Kapsam):

[Geçerli girdilerle başarılı çalışma yolu testi, geçersiz girdi testleri, bağımlılık hata testleri, sınır değer testleri]

Bu özeti oluşturduktan sonra, başka bir şey yapmadan sadece onayı bekle.

KRİTİK UYARI: Bu adımda SADECE Final Test Brifingi'ni METİN olarak üret. Yukarıdaki şablonu doldur.
- Kod yazma
- Enum tanımlama
- Typedef yazma
- Fonksiyon yazma
- C kodu üretme
Sadece brifing METNİNİ üret."""

def get_step7_prompt(function_name):
    """Adım 7: Üretim Komutu (Testleri Yaz!)"""
    return f"""Harika. Lütfen şimdiye kadar verdiğim tüm bilgileri (rol tanımı, header dosyaları, {function_name} fonksiyonunun arayüzü, fonksiyonel davranışı ve senin türettiğin test tasarım kriterleri ile mock davranış kuralları) kullanarak, {function_name} fonksiyonu için Ceedling/Unity formatında tüm test senaryolarını şimdi oluştur. Her test senaryosu için uygun Unity ve CMock makrolarını kullan.

KRİTİK UYARI: Bu adımda SADECE C test kodu üret. Test dosyasını tam olarak yaz.
- #include direktifleri
- Unity test fonksiyonları (TEST_ASSERT_EQUAL, TEST_ASSERT_TRUE vb.)
- CMock mock tanımları (Mock_xxx_ExpectAndReturn vb.)
- Tüm test senaryoları
- Enum tanımları veya header dosyaları üretme
- Sadece test kodu üret"""

# ==============================================================================
# LLM İLE İLETİŞİM FONKSİYONU
# ==============================================================================

def send_to_llm(prompt, conversation_history=None, max_tokens=2000, step_number=None):
    """LLM'e mesaj gönder ve cevap al"""
    # Konuşma geçmişini akıllıca yönet - sadece son 2-3 mesajı tut
    if conversation_history:
        # Son 4-6 öğeyi al (her adım 2 öğe: prompt + response)
        recent_history = conversation_history[-6:] if len(conversation_history) > 6 else conversation_history
        full_prompt = "\n\n".join(recent_history) + "\n\n" + prompt
    else:
        full_prompt = prompt
    
    formatted_prompt = f"### Instruction:\n{full_prompt}\n\n### Response:\n"
    
    print("\n[LLM'e gönderiliyor...]")
    
    # Token limitini ayarla - input için daha fazla yer bırak
    max_input_length = 3072  # Model'in maksimum context window'una göre ayarla
    inputs = tokenizer(
        formatted_prompt, 
        return_tensors="pt", 
        truncation=True, 
        max_length=min(max_input_length, 2048)
    ).to("cuda")
    
    # Adım numarasına göre token limitini ayarla
    if step_number == 7:
        # Son adımda daha uzun çıktı bekleniyor
        token_limit = max_tokens
    elif step_number in [1, 2, 3]:
        # İlk adımlarda kısa onay bekleniyor
        token_limit = 300
    else:
        # Diğer adımlarda orta uzunlukta
        token_limit = 1500
    
    with torch.no_grad():
        outputs = model.generate(
            **inputs,
            max_new_tokens=token_limit,
            temperature=0.2,
            top_p=0.9,
            do_sample=True,
            eos_token_id=tokenizer.eos_token_id,
            pad_token_id=tokenizer.eos_token_id,
            repetition_penalty=1.1  # Tekrarları azalt
        )
    
    generated_text = tokenizer.decode(outputs[0][inputs['input_ids'].shape[1]:], skip_special_tokens=True)
    
    # Response kısmını temizle
    if "### Response:" in generated_text:
        generated_text = generated_text.split("### Response:")[-1].strip()
    
    # Çok uzun enum tanımları gibi tekrarlı içerikleri tespit et ve uyar
    if len(generated_text) > 2000 and ("typedef enum" in generated_text or "ENGINE_ERROR" in generated_text):
        print("\n[UYARI: LLM çok uzun enum tanımları üretiyor, ilk 500 karakter gösteriliyor]")
        return generated_text[:500] + "\n\n[... çıktı kesildi, muhtemelen tekrarlı enum tanımları ...]"
    
    return generated_text.strip()

# ==============================================================================
# ANA SÜREÇ - 7 ADIMLI YAKLAŞIM
# ==============================================================================

def run_7_step_process():
    """7 adımlı aşamalı bilgi yükleme sürecini yönet"""
    
    conversation_history = []
    step_data = {}
    
    print("\n" + "="*50)
    print(" 7 ADIMLI AŞAMALI BİLGİ YÜKLEME SÜRECİ")
    print("="*50)
    
    # Fonksiyon adını başta al
    print("\nÖnce test edilecek fonksiyonun adını girin:")
    function_name = input("Fonksiyon adı (örn: example_func): ").strip()
    if not function_name:
        print("Fonksiyon adı boş olamaz!")
        return
    
    # Adım 1: Girizgah
    print("\n[ADIM 1/7] Girizgah (Rol ve Görev Tanımı)")
    print("-" * 50)
    step1_prompt = get_step1_prompt()
    print("\nLLM'e gönderilen mesaj:")
    print(step1_prompt[:200] + "...")
    
    response1 = send_to_llm(step1_prompt, step_number=1)
    conversation_history.append(f"Kullanıcı: {step1_prompt}")
    conversation_history.append(f"Asistan: {response1}")
    
    print("\n[LLM Cevabı]:")
    print(response1)
    print("-" * 50)
    
    input("\nDevam etmek için Enter'a basın...")
    
    # Adım 2: Çevreyi Tanımlama
    print("\n[ADIM 2/7] Çevreyi Tanımlama (Header Dosyaları ve Fonksiyon Arayüzü)")
    print("-" * 50)
    print("\nLütfen header dosyalarının içeriğini girin (birden fazla satır, bitirmek için boş satır + 'END' yazın):")
    header_lines = []
    while True:
        line = input()
        if line.strip().upper() == "END":
            break
        header_lines.append(line)
    header_files_content = "\n".join(header_lines)
    
    print("\nLütfen fonksiyon arayüzü dokümantasyonunu girin (Bölüm 1 ve Bölüm 2 - bitirmek için boş satır + 'END' yazın):")
    print("(Bölüm 1: Genel Bakış ve Amaç, Bölüm 2: Teknik Arayüz Sözleşmesi)")
    interface_lines = []
    while True:
        line = input()
        if line.strip().upper() == "END":
            break
        interface_lines.append(line)
    function_interface_doc = "\n".join(interface_lines)
    
    step2_prompt = get_step2_prompt(header_files_content, function_name, function_interface_doc)
    response2 = send_to_llm(step2_prompt, conversation_history, step_number=2)
    conversation_history.append(f"Kullanıcı: {step2_prompt}")
    conversation_history.append(f"Asistan: {response2}")
    
    print("\n[LLM Cevabı]:")
    print(response2)
    print("-" * 50)
    
    input("\nDevam etmek için Enter'a basın...")
    
    # Adım 3: Fonksiyonel Davranış
    print("\n[ADIM 3/7] Fonksiyonel Davranış ve Mantık")
    print("-" * 50)
    print("\nLütfen fonksiyonel davranış dokümantasyonunu girin (bitirmek için boş satır + 'END' yazın):")
    behavior_lines = []
    while True:
        line = input()
        if line.strip().upper() == "END":
            break
        behavior_lines.append(line)
    functional_behavior_doc = "\n".join(behavior_lines)
    
    step3_prompt = get_step3_prompt(function_name, functional_behavior_doc)
    response3 = send_to_llm(step3_prompt, conversation_history, step_number=3)
    conversation_history.append(f"Kullanıcı: {step3_prompt}")
    conversation_history.append(f"Asistan: {response3}")
    
    print("\n[LLM Cevabı]:")
    print(response3)
    print("-" * 50)
    
    input("\nDevam etmek için Enter'a basın...")
    
    # Adım 4: Test Tasarım Kriterleri ve Mock Kuralları
    print("\n[ADIM 4/7] Test Tasarım Kriterleri ve Mock Davranış Kurallarını Türetme")
    print("-" * 50)
    step4_prompt = get_step4_prompt(function_name)
    response4 = send_to_llm(step4_prompt, conversation_history, step_number=4)
    conversation_history.append(f"Kullanıcı: {step4_prompt}")
    conversation_history.append(f"Asistan: {response4}")
    
    print("\n[LLM Cevabı]:")
    print(response4)
    print("-" * 50)
    
    input("\nDevam etmek için Enter'a basın...")
    
    # Adım 5: Statik Kontrol
    print("\n[ADIM 5/7] Statik Kontrol ve Teyit")
    print("-" * 50)
    step5_prompt = get_step5_prompt()
    response5 = send_to_llm(step5_prompt, conversation_history, step_number=5)
    conversation_history.append(f"Kullanıcı: {step5_prompt}")
    conversation_history.append(f"Asistan: {response5}")
    
    print("\n[LLM Cevabı]:")
    print(response5)
    print("-" * 50)
    
    input("\nDevam etmek için Enter'a basın...")
    
    # Adım 6: Bilgi Sıkıştırma
    print("\n[ADIM 6/7] Bilgi Sıkıştırma (Final Briefing)")
    print("-" * 50)
    step6_prompt = get_step6_prompt(function_name)
    response6 = send_to_llm(step6_prompt, conversation_history, step_number=6)
    conversation_history.append(f"Kullanıcı: {step6_prompt}")
    conversation_history.append(f"Asistan: {response6}")
    
    print("\n[LLM Cevabı]:")
    print(response6)
    print("-" * 50)
    
    input("\nDevam etmek için Enter'a basın...")
    
    # Adım 7: Test Kodunu Üret
    print("\n[ADIM 7/7] Test Kodunu Üretme")
    print("-" * 50)
    step7_prompt = get_step7_prompt(function_name)
    response7 = send_to_llm(step7_prompt, conversation_history, step_number=7, max_tokens=2500)
    
    print("\n" + "="*50)
    print(" ÜRETİLEN TEST KODU")
    print("="*50)
    print(response7)
    print("="*50)
    
    # Sonuçları dosyaya kaydet
    save_results = input("\nSonuçları dosyaya kaydetmek ister misiniz? (e/h): ")
    if save_results.lower() == 'e':
        filename = input("Dosya adı (örn: test_output.txt): ").strip()
        if not filename:
            filename = "test_output.txt"
        
        with open(filename, 'w', encoding='utf-8') as f:
            f.write("="*50 + "\n")
            f.write("7 ADIMLI TEST ÜRETİM SÜRECİ - SONUÇLAR\n")
            f.write("="*50 + "\n\n")
            
            steps_data = [
                ("Adım 1: Girizgah", step1_prompt, response1),
                ("Adım 2: Çevreyi Tanımlama", step2_prompt, response2),
                ("Adım 3: Fonksiyonel Davranış", step3_prompt, response3),
                ("Adım 4: Test Tasarım Kriterleri", step4_prompt, response4),
                ("Adım 5: Statik Kontrol", step5_prompt, response5),
                ("Adım 6: Final Briefing", step6_prompt, response6),
                ("Adım 7: Test Kodu", step7_prompt, response7),
            ]
            
            for i, (title, prompt, response) in enumerate(steps_data, 1):
                f.write(f"\n{'='*50}\n")
                f.write(f"{i}. {title}\n")
                f.write(f"{'='*50}\n\n")
                f.write(f"[Prompt]:\n{prompt[:500]}...\n\n")
                f.write(f"[LLM Cevabı]:\n{response}\n\n")
            
            f.write("\n" + "="*50 + "\n")
            f.write("ÜRETİLEN TEST KODU (FİNAL)\n")
            f.write("="*50 + "\n\n")
            f.write(response7)
        
        print(f"\nSonuçlar '{filename}' dosyasına kaydedildi.")

# ==============================================================================
# ANA PROGRAM
# ==============================================================================

if __name__ == "__main__":
    try:
        print("\n" + "="*50)
        print(" TEST ÜRETİM SİSTEMİ")
        print("="*50)
        print("\nSeçenekler:")
        print("1. 7 Adımlı Aşamalı Bilgi Yükleme Süreci")
        print("2. Basit Tek Seferlik Talimat (Eski Yöntem)")
        print("3. Çıkış")
        
        choice = input("\nSeçiminiz (1/2/3): ").strip()
        
        if choice == "1":
            run_7_step_process()
        elif choice == "2":
            # Eski basit yöntem
            while True:
                try:
                    instruction = input("\n Talimat (Çıkış için 'exit'): ")
                    if instruction.lower() in ["exit", "quit"]:
                        break
                    
                    print(" Kod yazılıyor...")
                    prompt = f"### Instruction:\n{instruction}\n\n### Response:\n"
                    inputs = tokenizer(prompt, return_tensors="pt").to("cuda")
                    
                    with torch.no_grad():
                        outputs = model.generate(
                            **inputs,
                            max_new_tokens=600,
                            temperature=0.2,
                            top_p=0.9,
                            do_sample=True,
                            eos_token_id=tokenizer.eos_token_id,
                            pad_token_id=tokenizer.eos_token_id
                        )
                    
                    generated_code = tokenizer.decode(outputs[0][inputs['input_ids'].shape[1]:], skip_special_tokens=True)
                    
                    print("\n" + "-"*20 + " SONUÇ " + "-"*20)
                    if "### Response:" in generated_code:
                        print(generated_code.split("### Response:\n")[1])
                    else:
                        print(generated_code)
                    print("-" * 50)
                    
                except KeyboardInterrupt:
                    print("\nÇıkılıyor...")
                    break
        elif choice == "3":
            print("Çıkılıyor...")
        else:
            print("Geçersiz seçim!")
            
    except KeyboardInterrupt:
        print("\n\nProgram sonlandırıldı.")
    except Exception as e:
        print(f"\nHATA: {e}")
        import traceback
        traceback.print_exc()
