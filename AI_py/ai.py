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
import re
import os
from pathlib import Path

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
    return f"""Sen bir C test kodu uzmanısın. {function_name} fonksiyonu için Ceedling/Unity formatında TAM ve EKSİKSİZ bir test dosyası yaz.

GEREKLİLİKLER:
1. #include "unity.h" ve gerekli header dosyalarını ekle
2. #include "cmock.h" ekle
3. Mock edilecek bağımlı fonksiyonlar için CMock mock tanımları ekle
4. setUp() ve tearDown() fonksiyonlarını yaz
5. Her test senaryosu için ayrı test fonksiyonu yaz (test_ ile başlamalı)
6. Unity makrolarını kullan (TEST_ASSERT_EQUAL, TEST_ASSERT_TRUE, TEST_ASSERT_FALSE vb.)
7. CMock makrolarını kullan (Mock_xxx_ExpectAndReturn, Mock_xxx_Expect vb.)
8. Geçerli girdi testleri, geçersiz girdi testleri, sınır değer testleri, bağımlılık hata testleri ekle

KRİTİK: SADECE C test kodu üret. Enum tanımları, typedef'ler, header dosyaları ÜRETME. Sadece test_*.c dosyası içeriği üret."""

# ==============================================================================
# OTOMATİK BİLGİ TOPLAMA FONKSİYONLARI
# ==============================================================================

def get_project_root():
    """Proje kök dizinini bul (AI_py klasörünün bir üstü)"""
    current_dir = Path(__file__).parent.absolute()
    # AI_py klasöründeysek bir üst dizine çık
    if current_dir.name == "AI_py":
        return current_dir.parent
    return current_dir

def find_function_module(function_name):
    """Fonksiyon adından hangi modülde olduğunu bul"""
    if function_name.startswith("engine_"):
        return "engine_control"
    elif function_name.startswith("sensor_"):
        return "sensor_interface"
    elif function_name.startswith("diagnostic_"):
        return "vehicle_diagnostics"
    else:
        # Varsayılan olarak engine_control'ü dene
        return "engine_control"

def read_header_file(module_name):
    """Header dosyasını oku"""
    project_root = get_project_root()
    header_file = project_root / f"{module_name}.h"
    if header_file.exists():
        with open(header_file, 'r', encoding='utf-8') as f:
            return f.read()
    return None

def extract_function_signature(header_content, function_name):
    """Header dosyasından fonksiyon imzasını ve dokümantasyonunu çıkar"""
    if not header_content:
        return None, None
    
    # Fonksiyon imzasını bul
    pattern = rf'/\*\*.*?\*/\s*(\w+\s+{re.escape(function_name)}\s*\([^)]*\)\s*;)'
    match = re.search(pattern, header_content, re.DOTALL)
    
    if not match:
        # Daha basit pattern dene
        pattern = rf'(\w+\s+{re.escape(function_name)}\s*\([^)]*\)\s*;)'
        match = re.search(pattern, header_content)
    
    signature = match.group(1) if match else None
    
    # Dokümantasyonu bul (imzadan önceki @brief, @param, @return içeren yorum bloğu)
    if match:
        start_pos = match.start()
        # Geriye doğru yorum bloğunu bul
        doc_pattern = r'/\*\*.*?\*/'
        doc_matches = list(re.finditer(doc_pattern, header_content[:start_pos], re.DOTALL))
        if doc_matches:
            doc = doc_matches[-1].group(0)
            return signature, doc
    
    return signature, None

def extract_all_dependencies(header_content):
    """Header dosyasından tüm include'ları ve bağımlılıkları bul"""
    includes = re.findall(r'#include\s+["<]([^">]+)[">]', header_content)
    return includes

def read_dependency_headers(dependencies):
    """Bağımlılık header dosyalarını oku"""
    project_root = get_project_root()
    headers = {}
    for dep in dependencies:
        if dep.endswith('.h'):
            dep_file = project_root / dep
        else:
            dep_file = project_root / f"{dep}.h"
        
        if dep_file.exists():
            with open(dep_file, 'r', encoding='utf-8') as f:
                headers[dep] = f.read()
    return headers

def extract_function_implementation(c_file, function_name):
    """C dosyasından fonksiyon implementasyonunu çıkar"""
    project_root = get_project_root()
    c_file_path = project_root / c_file
    if not c_file_path.exists():
        return None
    
    with open(c_file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Fonksiyon implementasyonunu bul
    # Pattern: return_type function_name(...) { ... }
    pattern = rf'(\w+\s+{re.escape(function_name)}\s*\([^)]*\)\s*{{[^}}]*(?:{{[^}}]*}}[^}}]*)*}})'
    match = re.search(pattern, content, re.DOTALL)
    
    if match:
        return match.group(1)
    
    # Daha basit pattern - sadece fonksiyon başlangıcından sonraki 50 satır
    pattern = rf'(\w+\s+{re.escape(function_name)}\s*\([^)]*\)\s*{{)'
    match = re.search(pattern, content)
    if match:
        start = match.end()
        # Sonraki 100 satırı al veya } bulana kadar
        lines = content[start:].split('\n')
        implementation_lines = []
        brace_count = 1
        for line in lines[:200]:  # Maksimum 200 satır
            implementation_lines.append(line)
            brace_count += line.count('{') - line.count('}')
            if brace_count == 0:
                break
        return match.group(1) + '\n' + '\n'.join(implementation_lines)
    
    return None

def read_module_readme(module_name):
    """Modül README dosyasını oku"""
    project_root = get_project_root()
    readme_file = project_root / f"{module_name}_README.md"
    if readme_file.exists():
        with open(readme_file, 'r', encoding='utf-8') as f:
            return f.read()
    return None

def build_function_interface_doc(function_name, signature, doc_comment):
    """Fonksiyon arayüz dokümantasyonunu oluştur"""
    doc = f"Kapsamlı Detay Tasarım Belgesi: {function_name} (Bölüm 1 ve Bölüm 2)\n\n"
    doc += "Bölüm 1: Genel Bakış ve Amaç:\n\n"
    
    if doc_comment:
        # @brief'den genel bakışı çıkar
        brief_match = re.search(r'@brief\s+(.+?)(?:\n|@)', doc_comment, re.DOTALL)
        if brief_match:
            doc += brief_match.group(1).strip() + "\n\n"
        else:
            doc += f"{function_name} fonksiyonu motor kontrol sisteminin bir parçasıdır.\n\n"
    else:
        doc += f"{function_name} fonksiyonu motor kontrol sisteminin bir parçasıdır.\n\n"
    
    doc += "Bölüm 2: Teknik Arayüz Sözleşmesi:\n\n"
    doc += f"Fonksiyon İmzası: {signature}\n\n" if signature else f"Fonksiyon İmzası: Bulunamadı\n\n"
    
    if doc_comment:
        # @param'ları çıkar
        params = re.findall(r'@param\[?(\w+)?\]?\s+(\w+)\s+(.+)', doc_comment)
        if params:
            doc += "Parametreler:\n"
            for param_type, param_name, param_desc in params:
                doc += f"{param_name}: {param_desc.strip()}\n"
            doc += "\n"
        
        # @return'ü çıkar
        return_match = re.search(r'@return\s+(.+)', doc_comment, re.DOTALL)
        if return_match:
            doc += f"Dönüş Değeri: {return_match.group(1).strip()}\n\n"
    
    return doc

def build_functional_behavior_doc(function_name, implementation, readme_content):
    """Fonksiyonel davranış dokümantasyonunu oluştur"""
    doc = f"Kapsamlı Detay Tasarım Belgesi: {function_name} (Bölüm 3: Fonksiyonel Davranış ve Mantık)\n\n"
    
    doc += "Fonksiyon İş Akışı:\n"
    if implementation:
        # Implementasyondan iş akışını çıkar
        doc += "Fonksiyon implementasyonu aşağıdadır:\n\n"
        doc += "```c\n"
        doc += implementation[:1000]  # İlk 1000 karakter
        doc += "\n```\n\n"
    else:
        doc += "Implementasyon bulunamadı. README dosyasından genel bilgiler kullanılacak.\n\n"
    
    if readme_content:
        # README'den ilgili bilgileri çıkar
        doc += "Modül Genel Bilgileri:\n"
        doc += readme_content[:500] + "\n\n"
    
    return doc

# ==============================================================================
# LLM İLE İLETİŞİM FONKSİYONU
# ==============================================================================

def send_to_llm(prompt, conversation_history=None, max_tokens=2000, step_number=None, use_minimal_history=False):
    """LLM'e mesaj gönder ve cevap al"""
    # Son adımda conversation history'yi sıfırla veya minimal tut
    if step_number == 7 or use_minimal_history:
        # Son adımda sadece final briefing'i gönder, önceki adımları gönderme
        full_prompt = prompt
    elif conversation_history:
        # Diğer adımlarda sadece son 2 mesajı tut (1 adım: prompt + response)
        recent_history = conversation_history[-2:] if len(conversation_history) > 2 else conversation_history
        full_prompt = "\n\n".join(recent_history) + "\n\n" + prompt
    else:
        full_prompt = prompt
    
    formatted_prompt = f"### Instruction:\n{full_prompt}\n\n### Response:\n"
    
    print("\n[LLM'e gönderiliyor...]")
    
    # Token limitini ayarla - input için daha fazla yer bırak
    # Son adımda daha fazla bilgi gönderebilmek için daha yüksek limit
    if step_number == 7:
        max_input_length = 2048  # Son adımda daha fazla bilgi göndermek için
    else:
        max_input_length = 2048  # Diğer adımlar için standart limit
    
    inputs = tokenizer(
        formatted_prompt, 
        return_tensors="pt", 
        truncation=True, 
        max_length=max_input_length
    ).to("cuda")
    
    # Adım numarasına göre token limitini ayarla
    if step_number == 7:
        # Son adımda çok daha uzun çıktı bekleniyor (tam test kodu)
        token_limit = 3000  # Test kodu için daha fazla token
    elif step_number in [1, 2, 3]:
        # İlk adımlarda kısa onay bekleniyor
        token_limit = 200  # Daha kısa limit - sadece onay
    elif step_number == 4:
        # Test tasarım kriterleri için orta uzunluk
        token_limit = 2000
    elif step_number == 5:
        # Statik kontrol için kısa
        token_limit = 800
    elif step_number == 6:
        # Final briefing için orta uzunluk
        token_limit = 2000
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
    
    # Yanlış format kontrolü - eğer kod/enum üretiyorsa uyar
    if step_number in [1, 2, 3] and ("typedef" in generated_text or "enum" in generated_text or "void " in generated_text[:100]):
        print("\n[UYARI: LLM beklenen formatta cevap vermedi - kod/enum üretiyor]")
        print("[İlk 200 karakter:]", generated_text[:200])
    
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
    print("\nTest edilecek fonksiyonun adını girin:")
    function_name = input("Fonksiyon adı (örn: engine_initialize): ").strip()
    if not function_name:
        print("Fonksiyon adı boş olamaz!")
        return
    
    # Otomatik bilgi toplama
    print(f"\n[Otomatik bilgi toplanıyor: {function_name}...]")
    module_name = find_function_module(function_name)
    print(f"Modül bulundu: {module_name}")
    
    # Header dosyasını oku
    header_content = read_header_file(module_name)
    if not header_content:
        print(f"HATA: {module_name}.h dosyası bulunamadı!")
        return
    
    # Fonksiyon imzasını ve dokümantasyonunu çıkar
    signature, doc_comment = extract_function_signature(header_content, function_name)
    if not signature:
        print(f"UYARI: {function_name} fonksiyon imzası bulunamadı, devam ediliyor...")
    
    # Bağımlılıkları bul
    dependencies = extract_all_dependencies(header_content)
    dependency_headers = read_dependency_headers(dependencies)
    
    # Tüm header dosyalarını birleştir
    all_headers = header_content
    for dep_name, dep_content in dependency_headers.items():
        all_headers += f"\n\n/* ========== {dep_name} ========== */\n\n"
        all_headers += dep_content
    
    # Implementasyonu bul
    c_file = f"{module_name}.c"
    implementation = extract_function_implementation(c_file, function_name)
    
    # README'yi oku
    readme_content = read_module_readme(module_name)
    
    # Fonksiyon arayüz dokümantasyonunu oluştur
    function_interface_doc = build_function_interface_doc(function_name, signature, doc_comment)
    
    # Fonksiyonel davranış dokümantasyonunu oluştur
    functional_behavior_doc = build_functional_behavior_doc(function_name, implementation, readme_content)
    
    print(f"[✓] Header dosyası okundu")
    print(f"[✓] Fonksiyon imzası bulundu: {signature[:50] if signature else 'Bulunamadı'}...")
    print(f"[✓] {len(dependencies)} bağımlılık bulundu")
    print(f"[✓] Implementasyon {'bulundu' if implementation else 'bulunamadı'}")
    print(f"[✓] README {'okundu' if readme_content else 'bulunamadı'}")
    
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
    
    # Adım 2: Çevreyi Tanımlama (Otomatik)
    print("\n[ADIM 2/7] Çevreyi Tanımlama (Header Dosyaları ve Fonksiyon Arayüzü)")
    print("-" * 50)
    print("[Otomatik olarak header dosyaları ve fonksiyon arayüzü hazırlanıyor...]")
    
    # Otomatik olarak toplanan bilgileri kullan
    header_files_content = all_headers
    # function_interface_doc zaten oluşturuldu
    
    step2_prompt = get_step2_prompt(header_files_content, function_name, function_interface_doc)
    response2 = send_to_llm(step2_prompt, conversation_history, step_number=2)
    conversation_history.append(f"Kullanıcı: {step2_prompt}")
    conversation_history.append(f"Asistan: {response2}")
    
    print("\n[LLM Cevabı]:")
    print(response2)
    print("-" * 50)
    
    input("\nDevam etmek için Enter'a basın...")
    
    # Adım 3: Fonksiyonel Davranış (Otomatik)
    print("\n[ADIM 3/7] Fonksiyonel Davranış ve Mantık")
    print("-" * 50)
    print("[Otomatik olarak fonksiyonel davranış dokümantasyonu hazırlanıyor...]")
    
    # functional_behavior_doc zaten oluşturuldu
    
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
    
    # Final briefing'i hazırla - tüm önemli bilgileri özetle
    # Adım 6'dan gelen final briefing'i kullan, yoksa kendimiz oluştur
    if response6 and "Final Test Brifingi" in response6 and len(response6) > 300:
        final_briefing_summary = response6
    else:
        # Kendimiz oluştur
        final_briefing_summary = f"""Final Test Brifingi: {function_name}

Test Edilecek Fonksiyon: {function_name}

Teknik Arayüz:
İmza: {signature if signature else 'Bulunamadı'}
{doc_comment[:300] if doc_comment else ''}

Bağımlılıklar: {', '.join(dependencies) if dependencies else 'Bulunamadı'}

Fonksiyon İş Akışı (Özet):
{implementation[:500] if implementation else 'Implementasyon bulunamadı'}...
"""
    
    # Test tasarım kriterlerini de ekle (Adım 4'ten)
    test_design_summary = ""
    if response4 and len(response4) > 200 and "Bölüm 3" in response4:
        # Adım 4'ten test tasarım kriterlerini çıkar
        test_design_summary = response4[:800] + "..."
    
    # Son adımda conversation history'yi sıfırla ve sadece özet bilgiler + prompt gönder
    step7_prompt = get_step7_prompt(function_name)
    
    # Kapsamlı prompt oluştur
    enhanced_step7_prompt = f"""Sen bir C test kodu uzmanısın. Aşağıda {function_name} fonksiyonu için tüm gerekli bilgiler bulunmaktadır:

{final_briefing_summary}

{test_design_summary if test_design_summary else ''}

Şimdi bu bilgilere dayanarak {function_name} fonksiyonu için Ceedling/Unity formatında TAM ve EKSİKSİZ bir test dosyası yaz.

{step7_prompt}"""
    
    print("[Final briefing hazırlandı, test kodu üretiliyor...]")
    print(f"[Final briefing uzunluğu: {len(enhanced_step7_prompt)} karakter]")
    response7 = send_to_llm(enhanced_step7_prompt, conversation_history=None, step_number=7, max_tokens=3000, use_minimal_history=True)
    
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
        print("\nBu sistem, projedeki C fonksiyonları için otomatik test kodu üretir.")
        print("Sadece test edilecek fonksiyonun adını girin (örn: engine_initialize)")
        print("\nSeçenekler:")
        print("1. Otomatik Test Üretimi (7 Adımlı - Önerilen)")
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
