/**
 * @file engine_control.c
 * @brief Otomotiv Motor Kontrol Sistemi - Ana Modül İmplementasyonu
 * @details Motor kontrol sisteminin 40+ fonksiyonunun implementasyonu
 */

#include "engine_control.h"
#include "../sensor_interface/sensor_interface.h"
#include "../vehicle_diagnostics/vehicle_diagnostics.h"
#include <string.h>

/* ============================================================================
   STATIC DEĞIŞKENLER VE DURUM YÖNETİMİ
   ============================================================================ */

static struct {
    EngineState_t state;
    uint16_t current_rpm;
    uint16_t target_rpm;
    uint16_t max_rpm_limit;
    int16_t current_temperature;
    uint8_t fuel_injection_percentage;
    int8_t ignition_advance;
    uint16_t fuel_pressure;
    uint16_t boost_pressure;
    bool turbo_enabled;
    uint8_t cooling_fan_speed;
    FuelInjectionMode_t fuel_mode;
    IgnitionTimingMode_t ignition_mode;
    uint8_t throttle_position;
    uint8_t air_intake_percentage;
    ErrorLog_t last_error;
    DiagnosticData_t diagnostics;
    bool is_initialized;
    uint32_t runtime_seconds;
    uint8_t rpm_acceleration_rate;
    uint8_t rpm_deceleration_rate;
    EngineCallback_t callbacks;
    EngineConfig_t config;
    EngineState_t previous_state;
} engine_state = {
    .state = IDLE_STATE,
    .current_rpm = 0,
    .target_rpm = 1000,
    .max_rpm_limit = 6500,
    .current_temperature = 20,
    .fuel_injection_percentage = 0,
    .ignition_advance = 0,
    .fuel_pressure = 0,
    .boost_pressure = 0,
    .turbo_enabled = false,
    .cooling_fan_speed = 0,
    .fuel_mode = FUEL_INJECTION_NORMAL,
    .ignition_mode = IGNITION_TIMING_OPTIMAL,
    .throttle_position = 0,
    .air_intake_percentage = 20,
    .is_initialized = false,
    .runtime_seconds = 0,
    .rpm_acceleration_rate = 50,
    .rpm_deceleration_rate = 50,
    .callbacks = {.on_state_change = NULL, .on_error = NULL},
    .config = {.max_rpm = 6500, .max_temp = 130, .max_fuel_pressure = 600, .max_boost = 250, .validator = NULL},
    .previous_state = IDLE_STATE
};

/* ============================================================================
   İÇ FONKSIYONLAR (STATIC)
   ============================================================================ */

/**
 * @brief İç durumu günceller ve kontrol eder
 */
static void update_engine_state(void) {
    /* Sensörlerden veri okuma */
    uint16_t rpm_value = sensor_read_rpm_sensor();
    int16_t temp_value = sensor_read_temperature_sensor();
    uint16_t fuel_press = sensor_read_fuel_pressure_sensor();
    
    engine_state.current_rpm = rpm_value;
    engine_state.current_temperature = temp_value;
    engine_state.fuel_pressure = fuel_press;
    
    /* Tanı verilerini güncelle */
    if (rpm_value > engine_state.diagnostics.max_rpm_recorded) {
        engine_state.diagnostics.max_rpm_recorded = rpm_value;
    }
    if (temp_value < engine_state.diagnostics.min_temperature_recorded) {
        engine_state.diagnostics.min_temperature_recorded = temp_value;
    }
    if (temp_value > engine_state.diagnostics.max_temperature_recorded) {
        engine_state.diagnostics.max_temperature_recorded = temp_value;
    }
    
    engine_state.runtime_seconds++;
    engine_state.diagnostics.total_runtime_seconds++;
}

/**
 * @brief Motor durumunu güncelleyen iç fonksiyon
 */
static void update_engine_status(void) {
    if (engine_state.current_rpm > 500) {
        engine_state.state = RUNNING_STATE;
    } else if (engine_state.state == RUNNING_STATE && engine_state.current_rpm <= 500) {
        engine_state.state = IDLE_STATE;
    }
}

/**
 * @brief Hata kaydını tutması gereken iç fonksiyon
 */
static void log_error(uint16_t error_code, const char *message, bool is_critical) {
    engine_state.last_error.error_code = error_code;
    strncpy(engine_state.last_error.error_message, message, 127);
    engine_state.last_error.error_message[127] = '\0';
    engine_state.last_error.is_critical = is_critical;
    engine_state.last_error.recovery_attempts = 0;
    
    engine_state.diagnostics.total_errors++;
    if (is_critical) {
        engine_state.diagnostics.critical_errors++;
    }
    engine_state.diagnostics.last_error_code = error_code;
    
    /* Harici hata raporlaması */
    diagnostic_report_error(error_code, message);
}

/**
 * @brief Temel RPM ayarlaması yapan iç fonksiyon
 */
static uint16_t set_rpm_internal(uint16_t target) {
    if (target > engine_state.max_rpm_limit) {
        log_error(0x0101, "RPM limit exceeded", false);
        return engine_state.current_rpm;
    }
    
    engine_state.target_rpm = target;
    
    /* Fiziksel RPM ayarlaması sensör aracılığıyla */
    sensor_write_rpm_command(target);
    
    return target;
}

/* ============================================================================
   BAŞLANGIC VE KAPANMA IMPLEMENTASYONLARI
   ============================================================================ */

bool engine_initialize(uint16_t initial_rpm) {
    if (engine_state.is_initialized) {
        log_error(0x1001, "Engine already initialized", false);
        return false;
    }
    
    if (initial_rpm < 500 || initial_rpm > 1000) {
        log_error(0x1002, "Invalid initial RPM value", true);
        return false;
    }
    
    /* Sensör sistemini başlat */
    if (!sensor_initialize()) {
        log_error(0x1003, "Sensor initialization failed", true);
        return false;
    }
    
    /* Diagnostik sistemi başlat */
    if (!diagnostic_initialize()) {
        log_error(0x1004, "Diagnostic system initialization failed", true);
        return false;
    }
    
    engine_state.is_initialized = true;
    engine_state.state = IDLE_STATE;
    engine_state.target_rpm = initial_rpm;
    engine_state.diagnostics.min_temperature_recorded = 150;
    engine_state.diagnostics.max_temperature_recorded = -40;
    
    update_engine_state();
    
    return true;
}

bool engine_shutdown(void) {
    if (!engine_state.is_initialized) {
        return false;
    }
    
    /* Sıfır RPM'e düşür */
    engine_state.current_rpm = 0;
    engine_state.target_rpm = 0;
    engine_state.fuel_injection_percentage = 0;
    engine_state.state = SHUTDOWN_STATE;
    
    /* Sensör sistemini kapat */
    sensor_shutdown();
    
    /* Diagnostik sistemi kapat */
    diagnostic_shutdown();
    
    engine_state.is_initialized = false;
    
    return true;
}

bool engine_reset_state(void) {
    if (!engine_state.is_initialized) {
        return false;
    }
    
    engine_state.state = IDLE_STATE;
    engine_state.current_rpm = 0;
    engine_state.fuel_injection_percentage = 0;
    engine_state.ignition_advance = 0;
    engine_state.cooling_fan_speed = 0;
    engine_state.throttle_position = 0;
    
    return true;
}

bool engine_emergency_stop(void) {
    engine_state.state = SHUTDOWN_STATE;
    engine_state.current_rpm = 0;
    engine_state.fuel_injection_percentage = 0;
    engine_state.turbo_enabled = false;
    engine_state.cooling_fan_speed = 100;
    
    log_error(0x2001, "Emergency stop activated", true);
    diagnostic_log_emergency_event();
    
    return true;
}

bool engine_set_mode(uint8_t mode) {
    if (mode > 3) {
        log_error(0x2002, "Invalid engine mode", false);
        return false;
    }
    
    switch (mode) {
        case 0: /* Normal mod */
            engine_state.rpm_acceleration_rate = 50;
            engine_state.rpm_deceleration_rate = 50;
            break;
        case 1: /* Ekonomi modu */
            engine_state.rpm_acceleration_rate = 30;
            engine_state.rpm_deceleration_rate = 30;
            break;
        case 2: /* Sport modu */
            engine_state.rpm_acceleration_rate = 80;
            engine_state.rpm_deceleration_rate = 80;
            break;
        case 3: /* Track modu */
            engine_state.rpm_acceleration_rate = 100;
            engine_state.rpm_deceleration_rate = 100;
            break;
        default:
            return false;
    }
    
    return true;
}

/* ============================================================================
   RPM KONTROL IMPLEMENTASYONLARI
   ============================================================================ */

bool engine_set_target_rpm(uint16_t target_rpm) {
    if (target_rpm < 500 || target_rpm > engine_state.max_rpm_limit) {
        log_error(0x3001, "Target RPM out of range", false);
        return false;
    }
    
    set_rpm_internal(target_rpm);
    return true;
}

uint16_t engine_get_current_rpm(void) {
    update_engine_state();
    return engine_state.current_rpm;
}

uint16_t engine_increase_rpm(uint8_t increment_percentage) {
    if (increment_percentage == 0 || increment_percentage > 100) {
        log_error(0x3002, "Invalid increment percentage", false);
        return engine_state.current_rpm;
    }
    
    uint16_t increment = (engine_state.max_rpm_limit * increment_percentage) / 100;
    uint16_t new_rpm = engine_state.current_rpm + increment;
    
    if (new_rpm > engine_state.max_rpm_limit) {
        new_rpm = engine_state.max_rpm_limit;
    }
    
    set_rpm_internal(new_rpm);
    return new_rpm;
}

uint16_t engine_decrease_rpm(uint8_t decrement_percentage) {
    if (decrement_percentage == 0 || decrement_percentage > 100) {
        log_error(0x3003, "Invalid decrement percentage", false);
        return engine_state.current_rpm;
    }
    
    uint16_t decrement = (engine_state.current_rpm * decrement_percentage) / 100;
    uint16_t new_rpm = engine_state.current_rpm > decrement ? 
                       engine_state.current_rpm - decrement : 0;
    
    set_rpm_internal(new_rpm);
    return new_rpm;
}

bool engine_set_rpm_limit(uint16_t max_rpm) {
    if (max_rpm < 5000 || max_rpm > 8000) {
        log_error(0x3004, "RPM limit out of valid range", false);
        return false;
    }
    
    engine_state.max_rpm_limit = max_rpm;
    return true;
}

bool engine_set_rpm_acceleration_rate(uint8_t acceleration_rate) {
    if (acceleration_rate == 0 || acceleration_rate > 100) {
        log_error(0x3005, "Invalid acceleration rate", false);
        return false;
    }
    
    engine_state.rpm_acceleration_rate = acceleration_rate;
    return true;
}

bool engine_set_rpm_deceleration_rate(uint8_t deceleration_rate) {
    if (deceleration_rate == 0 || deceleration_rate > 100) {
        log_error(0x3006, "Invalid deceleration rate", false);
        return false;
    }
    
    engine_state.rpm_deceleration_rate = deceleration_rate;
    return true;
}

bool engine_check_rpm_stability(void) {
    update_engine_state();
    
    /* RPM stabilité kontrolü */
    if (engine_state.current_rpm >= engine_state.target_rpm - 100 &&
        engine_state.current_rpm <= engine_state.target_rpm + 100) {
        return true;
    }
    
    return false;
}

/* ============================================================================
   YAKIT ENJEKSİYONU KONTROL IMPLEMENTASYONLARI
   ============================================================================ */

bool engine_set_fuel_injection_mode(uint8_t mode) {
    if (mode > 3) {
        log_error(0x4001, "Invalid fuel injection mode", false);
        return false;
    }
    
    engine_state.fuel_mode = (FuelInjectionMode_t)mode;
    return true;
}

bool engine_set_fuel_injection_percentage(uint8_t percentage) {
    if (percentage < 10 || percentage > 100) {
        log_error(0x4002, "Fuel injection percentage out of range", false);
        return false;
    }
    
    engine_state.fuel_injection_percentage = percentage;
    sensor_write_fuel_injection_command(percentage);
    
    return true;
}

bool engine_set_fuel_pressure(uint16_t pressure_kpa) {
    if (pressure_kpa < 200 || pressure_kpa > 600) {
        log_error(0x4003, "Fuel pressure out of range", false);
        return false;
    }
    
    engine_state.fuel_pressure = pressure_kpa;
    sensor_write_fuel_pressure_command(pressure_kpa);
    
    return true;
}

uint16_t engine_get_fuel_pressure(void) {
    update_engine_state();
    return engine_state.fuel_pressure;
}

bool engine_set_fuel_injection_timing(int8_t timing_degrees) {
    if (timing_degrees < -10 || timing_degrees > 10) {
        log_error(0x4004, "Fuel timing out of range", false);
        return false;
    }
    
    engine_state.ignition_advance = timing_degrees;
    return true;
}

float engine_calculate_fuel_consumption(void) {
    /* Basit bir yakıt tüketim hesaplaması */
    float consumption = (float)engine_state.current_rpm / 1000.0f;
    consumption *= (float)engine_state.fuel_injection_percentage / 100.0f;
    consumption *= 6.0f; /* Saatlik hesaplama */
    
    return consumption;
}

bool engine_check_fuel_system_health(void) {
    /* Sensörlerden veri okuma */
    bool fuel_sensor_ok = sensor_check_fuel_sensor_status();
    bool fuel_pressure_ok = sensor_check_fuel_pressure_status();
    
    if (fuel_sensor_ok && fuel_pressure_ok) {
        return true;
    }
    
    log_error(0x4005, "Fuel system health check failed", true);
    return false;
}

/* ============================================================================
   MOTOR SICAKLIĞI KONTROL IMPLEMENTASYONLARI
   ============================================================================ */

int16_t engine_get_temperature(void) {
    update_engine_state();
    return engine_state.current_temperature;
}

bool engine_set_max_temperature_limit(int16_t max_temp_celsius) {
    if (max_temp_celsius < 80 || max_temp_celsius > 150) {
        log_error(0x5001, "Max temperature limit out of range", false);
        return false;
    }
    
    return true;
}

bool engine_set_cooling_fan_speed(uint8_t fan_speed) {
    if (fan_speed > 100) {
        log_error(0x5002, "Fan speed out of range", false);
        return false;
    }
    
    engine_state.cooling_fan_speed = fan_speed;
    sensor_write_cooling_fan_command(fan_speed);
    
    return true;
}

bool engine_auto_cooling_control(void) {
    update_engine_state();
    
    int16_t temp = engine_state.current_temperature;
    
    if (temp > 100) {
        engine_set_cooling_fan_speed(100);
    } else if (temp > 90) {
        engine_set_cooling_fan_speed(70);
    } else if (temp > 80) {
        engine_set_cooling_fan_speed(40);
    } else {
        engine_set_cooling_fan_speed(0);
    }
    
    return true;
}

bool engine_calibrate_temperature_sensor(int16_t reference_temp) {
    if (reference_temp < -40 || reference_temp > 150) {
        log_error(0x5003, "Reference temperature out of range", false);
        return false;
    }
    
    sensor_calibrate_temperature_sensor(reference_temp);
    return true;
}

bool engine_check_temperature_warning(void) {
    update_engine_state();
    
    if (engine_state.current_temperature > 120) {
        log_error(0x5004, "Engine temperature critical", true);
        return false;
    }
    
    if (engine_state.current_temperature < 0) {
        log_error(0x5005, "Engine temperature too low", false);
        return false;
    }
    
    return true;
}

/* ============================================================================
   TURBO KONTROL IMPLEMENTASYONLARI
   ============================================================================ */

bool engine_set_turbo_enabled(bool enable) {
    engine_state.turbo_enabled = enable;
    
    if (enable) {
        sensor_enable_turbo();
    } else {
        sensor_disable_turbo();
    }
    
    return true;
}

bool engine_set_turbo_boost_pressure(uint16_t boost_pressure_kpa) {
    if (boost_pressure_kpa < 0 || boost_pressure_kpa > 300) {
        log_error(0x6001, "Turbo boost pressure out of range", false);
        return false;
    }
    
    if (!engine_state.turbo_enabled && boost_pressure_kpa > 0) {
        log_error(0x6002, "Turbo not enabled", false);
        return false;
    }
    
    engine_state.boost_pressure = boost_pressure_kpa;
    sensor_write_turbo_boost_command(boost_pressure_kpa);
    
    return true;
}

uint16_t engine_get_turbo_boost_pressure(void) {
    return engine_state.boost_pressure;
}

bool engine_check_turbo_system_health(void) {
    bool turbo_sensor_ok = sensor_check_turbo_sensor_status();
    
    if (!turbo_sensor_ok) {
        log_error(0x6003, "Turbo system health check failed", true);
        return false;
    }
    
    return true;
}

/* ============================================================================
   TUTUŞTURMAİGNİTION) KONTROL IMPLEMENTASYONLARI
   ============================================================================ */

bool engine_set_ignition_mode(uint8_t mode) {
    if (mode > 2) {
        log_error(0x7001, "Invalid ignition mode", false);
        return false;
    }
    
    engine_state.ignition_mode = (IgnitionTimingMode_t)mode;
    return true;
}

bool engine_set_ignition_timing(int8_t advance_degrees) {
    if (advance_degrees < -15 || advance_degrees > 15) {
        log_error(0x7002, "Ignition timing out of range", false);
        return false;
    }
    
    engine_state.ignition_advance = advance_degrees;
    sensor_write_ignition_timing_command(advance_degrees);
    
    return true;
}

bool engine_set_ignition_coil_power(uint8_t coil_power) {
    if (coil_power < 50 || coil_power > 100) {
        log_error(0x7003, "Ignition coil power out of range", false);
        return false;
    }
    
    sensor_write_ignition_coil_command(coil_power);
    return true;
}

bool engine_check_spark_plug_health(void) {
    bool spark_plug_ok = sensor_check_spark_plug_status();
    
    if (!spark_plug_ok) {
        log_error(0x7004, "Spark plug health check failed", false);
        return false;
    }
    
    return true;
}

bool engine_check_ignition_system_health(void) {
    bool ignition_ok = sensor_check_ignition_system_status();
    
    if (!ignition_ok) {
        log_error(0x7005, "Ignition system health check failed", true);
        return false;
    }
    
    return true;
}

/* ============================================================================
   MOTOR DURUMU VE SENSÖR IMPLEMENTASYONLARI
   ============================================================================ */

bool engine_get_status(EngineStatus_t *status) {
    if (status == NULL) {
        log_error(0x8001, "Status pointer is NULL", false);
        return false;
    }
    
    update_engine_state();
    update_engine_status();
    
    status->current_rpm = engine_state.current_rpm;
    status->temperature_celsius = engine_state.current_temperature;
    status->throttle_position = engine_state.throttle_position;
    status->fuel_pressure = engine_state.fuel_pressure;
    status->oxygen_level = sensor_read_oxygen_sensor();
    status->air_intake_percentage = engine_state.air_intake_percentage;
    status->is_running = (engine_state.state != IDLE_STATE && 
                          engine_state.state != SHUTDOWN_STATE);
    status->state = engine_state.state;
    
    return true;
}

bool engine_apply_command(const EngineCommand_t *command) {
    if (command == NULL) {
        log_error(0x8002, "Command pointer is NULL", false);
        return false;
    }
    
    /* Hedef RPM ayarla */
    if (!engine_set_target_rpm(command->target_rpm)) {
        return false;
    }
    
    /* Yakıt enjeksiyonu ayarla */
    if (!engine_set_fuel_injection_percentage(command->fuel_injection_percentage)) {
        return false;
    }
    
    /* Tutuşturma zamanlaması ayarla */
    if (!engine_set_ignition_timing(command->ignition_advance_degrees)) {
        return false;
    }
    
    /* Turbo boost ayarla */
    if (command->turbo_enabled) {
        if (!engine_set_turbo_enabled(true)) {
            return false;
        }
        if (!engine_set_turbo_boost_pressure(command->boost_pressure)) {
            return false;
        }
    }
    
    /* Soğutma fanı ayarla */
    if (!engine_set_cooling_fan_speed(command->cooling_fan_speed)) {
        return false;
    }
    
    return true;
}

bool engine_set_air_intake_percentage(uint8_t air_intake_percentage) {
    if (air_intake_percentage < 10 || air_intake_percentage > 100) {
        log_error(0x8003, "Air intake percentage out of range", false);
        return false;
    }
    
    engine_state.air_intake_percentage = air_intake_percentage;
    sensor_write_air_intake_command(air_intake_percentage);
    
    return true;
}

bool engine_set_throttle_position(uint16_t throttle_position) {
    if (throttle_position > 100) {
        log_error(0x8004, "Throttle position out of range", false);
        return false;
    }
    
    engine_state.throttle_position = (uint8_t)throttle_position;
    sensor_write_throttle_command((uint8_t)throttle_position);
    
    return true;
}

uint8_t engine_get_oxygen_level(void) {
    return sensor_read_oxygen_sensor();
}

/* ============================================================================
   HATA YÖNETİMİ VE RECOVERY IMPLEMENTASYONLARI
   ============================================================================ */

bool engine_get_last_error(ErrorLog_t *error_log) {
    if (error_log == NULL) {
        return false;
    }
    
    memcpy(error_log, &engine_state.last_error, sizeof(ErrorLog_t));
    return true;
}

bool engine_clear_error_code(uint16_t error_code) {
    if (engine_state.last_error.error_code == error_code) {
        memset(&engine_state.last_error, 0, sizeof(ErrorLog_t));
        return true;
    }
    
    return false;
}

bool engine_attempt_recovery(void) {
    if (engine_state.last_error.recovery_attempts >= 3) {
        log_error(0x9002, "Maximum recovery attempts exceeded", true);
        return false;
    }
    
    engine_state.last_error.recovery_attempts++;
    
    /* Sensör sistemini tekrar başlat */
    sensor_shutdown();
    if (!sensor_initialize()) {
        return false;
    }
    
    engine_reset_state();
    return true;
}

bool engine_get_diagnostic_data(DiagnosticData_t *diag_data) {
    if (diag_data == NULL) {
        log_error(0x9001, "Diagnostic data pointer is NULL", false);
        return false;
    }
    
    memcpy(diag_data, &engine_state.diagnostics, sizeof(DiagnosticData_t));
    return true;
}

/* ============================================================================
   UYUMLULUK VE STANDART IMPLEMENTASYONLARI
   ============================================================================ */

uint32_t engine_run_for_seconds(uint32_t seconds) {
    if (seconds == 0 || seconds > 3600) {
        return 0;
    }
    
    uint32_t elapsed = 0;
    for (uint32_t i = 0; i < seconds; i++) {
        update_engine_state();
        elapsed++;
    }
    
    return elapsed;
}

uint32_t engine_get_system_version(void) {
    return 0x00010000; /* v1.0.0 */
}

/* ============================================================================
   İLERİ SEVİYE IMPLEMENTASYONLARI
   ============================================================================ */

EngineResult_t engine_register_callbacks(const EngineCallback_t *callbacks) {
    if (callbacks == NULL) {
        log_error(0x8005, "Callbacks pointer is NULL", false);
        return RESULT_INVALID_PARAM;
    }
    
    memcpy(&engine_state.callbacks, callbacks, sizeof(EngineCallback_t));
    return RESULT_OK;
}

bool engine_set_configuration(const EngineConfig_t *config) {
    if (config == NULL) {
        log_error(0x8006, "Configuration pointer is NULL", false);
        return false;
    }
    
    if (config->max_rpm < 5000 || config->max_rpm > 8000) {
        return false;
    }
    
    if (config->max_temp < 80 || config->max_temp > 150) {
        return false;
    }
    
    memcpy(&engine_state.config, config, sizeof(EngineConfig_t));
    engine_state.max_rpm_limit = config->max_rpm;
    
    return true;
}

EngineResult_t engine_process_raw_sensor_data(const RawSensorData_t *raw_data, 
                                               PerformanceMetrics_t *output) {
    if (raw_data == NULL || output == NULL) {
        return RESULT_INVALID_PARAM;
    }
    
    engine_state.current_rpm = raw_data->rpm;
    engine_state.current_temperature = raw_data->temperature;
    engine_state.fuel_pressure = raw_data->fuel_pressure;
    engine_state.throttle_position = raw_data->throttle;
    engine_state.boost_pressure = raw_data->boost_pressure;
    
    /* Performans metriklerini hesapla */
    output->efficiency = (float)engine_state.current_rpm / 1000.0f;
    output->performance_score = (uint8_t)(100 - (engine_state.diagnostics.total_errors * 5));
    output->health_status = engine_state.diagnostics.total_errors == 0 ? 100 : 50;
    output->runtime_hours = engine_state.diagnostics.total_runtime_seconds / 3600;
    output->remaining_fuel_percent = 100 - (engine_state.throttle_position / 2);
    
    return RESULT_OK;
}

bool engine_calculate_performance_metrics(PerformanceMetrics_t *out_metrics) {
    if (out_metrics == NULL) {
        return false;
    }
    
    out_metrics->efficiency = engine_calculate_fuel_consumption();
    out_metrics->performance_score = 100 - (engine_state.diagnostics.total_errors * 5);
    out_metrics->health_status = engine_state.diagnostics.total_errors == 0 ? 100 : 50;
    out_metrics->runtime_hours = engine_state.diagnostics.total_runtime_seconds / 3600;
    out_metrics->remaining_fuel_percent = 100 - (engine_state.throttle_position / 2);
    
    return true;
}

uint16_t engine_batch_set_rpm_values(const uint16_t *rpm_values, 
                                     uint16_t count,
                                     EngineFilterFunction_t filter) {
    if (rpm_values == NULL || count == 0) {
        return 0;
    }
    
    uint16_t processed = 0;
    
    for (uint16_t i = 0; i < count; i++) {
        /* Filter uygulanıyorsa kontrol et */
        if (filter != NULL) {
            if (!filter((const void*)&rpm_values[i], NULL)) {
                continue;
            }
        }
        
        /* RPM validasyonu ve ayarlaması */
        if (rpm_values[i] >= 500 && rpm_values[i] <= engine_state.max_rpm_limit) {
            engine_set_target_rpm(rpm_values[i]);
            processed++;
        }
    }
    
    return processed;
}

bool engine_trigger_error_callback(uint16_t error_code, const char *message) {
    if (engine_state.callbacks.on_error != NULL && message != NULL) {
        engine_state.callbacks.on_error(error_code, message);
        return true;
    }
    
    return false;
}

bool engine_trigger_state_change_callback(EngineState_t new_state) {
    if (engine_state.callbacks.on_state_change != NULL) {
        if (engine_state.state != new_state) {
            engine_state.callbacks.on_state_change(engine_state.state, new_state);
            engine_state.previous_state = engine_state.state;
            engine_state.state = new_state;
            return true;
        }
    }
    
    return false;
}

bool engine_run_custom_validation(uint16_t rpm, int16_t temp, uint16_t fuel_pressure) {
    if (engine_state.config.validator != NULL) {
        return engine_state.config.validator(rpm, temp, fuel_pressure);
    }
    
    /* Default validasyon */
    return (rpm <= engine_state.config.max_rpm &&
            temp <= engine_state.config.max_temp &&
            fuel_pressure <= engine_state.config.max_fuel_pressure);
}

EngineControl_t* engine_get_control_structure(void) {
    if (!engine_state.is_initialized) {
        return NULL;
    }
    
    return (EngineControl_t*)&engine_state;
}

uint8_t engine_process_sensor_array(RawSensorData_t *sensors, 
                                    uint8_t size,
                                    EngineFilterFunction_t callback) {
    if (sensors == NULL || size == 0) {
        return 0;
    }
    
    uint8_t processed = 0;
    
    for (uint8_t i = 0; i < size; i++) {
        if (callback != NULL) {
            if (!callback((const void*)&sensors[i], NULL)) {
                continue;
            }
        }
        
        PerformanceMetrics_t metrics;
        if (engine_process_raw_sensor_data(&sensors[i], &metrics) == RESULT_OK) {
            processed++;
        }
    }
    
    return processed;
}

uint16_t engine_copy_configuration(void *dest, const void *src, uint16_t size) {
    if (dest == NULL || src == NULL || size == 0) {
        return 0;
    }
    
    if (size > sizeof(EngineConfig_t)) {
        size = sizeof(EngineConfig_t);
    }
    
    memcpy(dest, src, size);
    return size;
}

const ErrorLog_t* engine_get_error_log_pointer(void) {
    return &engine_state.last_error;
}

uint8_t engine_clear_multiple_error_codes(const uint16_t *error_codes, uint8_t count) {
    if (error_codes == NULL || count == 0) {
        return 0;
    }
    
    uint8_t cleared = 0;
    
    for (uint8_t i = 0; i < count; i++) {
        if (engine_clear_error_code(error_codes[i])) {
            cleared++;
        }
    }
    
    return cleared;
}

bool engine_get_current_configuration(EngineConfig_t *out_config) {
    if (out_config == NULL) {
        return false;
    }
    
    memcpy(out_config, &engine_state.config, sizeof(EngineConfig_t));
    return true;
}

 bool engine_get_system_info(struct EngineSystemInfo *out_info) {
     if (out_info == NULL) {
         return false;
     }
     
     return true;
 }

/* ============================================================================
   İLERİ STATE MACHINE VE KONTROL AKIŞI IMPLEMENTASYONLARI
   ============================================================================ */

/**
 * @brief SWITCH-CASE: Sıcaklık seviyelerine göre kontrol (7 case!)
 */
uint8_t engine_handle_temperature_levels(int16_t temp) {
    uint8_t action = 0;
    
    switch (temp) {
        case -40 ... -10:  /* Çok soğuk */
            action = 1;
            engine_state.cooling_fan_speed = 0;
            engine_state.fuel_injection_percentage = 90;
            break;
            
        case -9 ... 0:     /* Soğuk */
            action = 2;
            engine_state.cooling_fan_speed = 10;
            engine_state.fuel_injection_percentage = 85;
            break;
            
        case 1 ... 50:     /* Normal-düşük */
            action = 3;
            engine_state.cooling_fan_speed = 20;
            engine_state.fuel_injection_percentage = 75;
            break;
            
        case 51 ... 80:    /* Optimal */
            action = 4;
            engine_state.cooling_fan_speed = 30;
            engine_state.fuel_injection_percentage = 70;
            break;
            
        case 81 ... 100:   /* Sıcak */
            action = 5;
            engine_state.cooling_fan_speed = 70;
            engine_state.fuel_injection_percentage = 65;
            break;
            
        case 101 ... 120:  /* Çok sıcak */
            action = 6;
            engine_state.cooling_fan_speed = 100;
            engine_state.fuel_injection_percentage = 60;
            log_error(0x5006, "Engine temperature high", false);
            break;
            
        case 121 ... 150:  /* Kritik sıcak */
            action = 7;
            engine_state.cooling_fan_speed = 100;
            engine_state.fuel_injection_percentage = 50;
            engine_state.state = ERROR_STATE;
            log_error(0x5007, "Engine critical temperature", true);
            break;
            
        default:           /* Geçersiz */
            action = 0;
            break;
    }
    
    return action;
}

/**
 * @brief NESTED-LOOP: Tüm sensörleri döngüsel olarak işleme
 */
uint8_t engine_process_all_sensors_loop(void) {
    if (!engine_state.is_initialized) {
        return 0;
    }
    
    uint8_t processed_count = 0;
    
    /* Dış döngü: 3 sensör grubu */
    for (uint8_t sensor_group = 0; sensor_group < 3; sensor_group++) {
        /* İç döngü: Her gruptaki sensörler */
        for (uint8_t sensor_index = 0; sensor_index < 5; sensor_index++) {
            bool success = false;
            
            /* Sensör grubuna göre farklı işlemler */
            if (sensor_group == 0) {  /* RPM sensörleri */
                uint16_t rpm = sensor_read_rpm_sensor();
                if (rpm > 0 && rpm <= engine_state.max_rpm_limit) {
                    engine_state.current_rpm = rpm;
                    success = true;
                }
            } else if (sensor_group == 1) {  /* Sıcaklık sensörleri */
                int16_t temp = sensor_read_temperature_sensor();
                if (temp >= -40 && temp <= 150) {
                    engine_state.current_temperature = temp;
                    success = true;
                }
            } else if (sensor_group == 2) {  /* Basınç sensörleri */
                uint16_t pressure = sensor_read_fuel_pressure_sensor();
                if (pressure >= 200 && pressure <= 600) {
                    engine_state.fuel_pressure = pressure;
                    success = true;
                }
            }
            
            if (success) {
                processed_count++;
            }
        }
    }
    
    return processed_count;
}

/**
 * @brief RPM seviyesine göre kapsamlı ayarlamalar (IF-ELSE CHAIN)
 */
bool engine_adjust_by_rpm_level(uint8_t rpm_level) {
    if (rpm_level > 100) {
        return false;
    }
    
    uint16_t target_rpm = (engine_state.max_rpm_limit * rpm_level) / 100;
    
    /* RPM seviyesine göre farklı ayarlamalar */
    if (rpm_level < 20) {
        /* Idle seviyesi */
        engine_state.fuel_injection_percentage = 40;
        engine_state.ignition_advance = -5;
        engine_state.cooling_fan_speed = 10;
    } else if (rpm_level < 40) {
        /* Düşük çalışma */
        engine_state.fuel_injection_percentage = 50;
        engine_state.ignition_advance = -2;
        engine_state.cooling_fan_speed = 20;
    } else if (rpm_level < 60) {
        /* Normal çalışma */
        engine_state.fuel_injection_percentage = 65;
        engine_state.ignition_advance = 0;
        engine_state.cooling_fan_speed = 35;
    } else if (rpm_level < 80) {
        /* Yüksek çalışma */
        engine_state.fuel_injection_percentage = 78;
        engine_state.ignition_advance = 5;
        engine_state.cooling_fan_speed = 65;
    } else {
        /* Maksimum çalışma */
        engine_state.fuel_injection_percentage = 90;
        engine_state.ignition_advance = 10;
        engine_state.cooling_fan_speed = 100;
        
        /* Turbo etkinleştir */
        if (engine_state.current_rpm > 5000) {
            engine_state.turbo_enabled = true;
            engine_state.boost_pressure = 200;
        }
    }
    
    engine_set_target_rpm(target_rpm);
    return true;
}

/**
 * @brief Motor durumuna bağlı durum otomatik ayarlama (STATE MACHINE)
 */
EngineState_t engine_auto_adjust_by_state(void) {
    if (!engine_state.is_initialized) {
        return ERROR_STATE;
    }
    
    EngineState_t new_state = engine_state.state;
    
    /* Durum makinesinde geçişler */
    switch (engine_state.state) {
        case IDLE_STATE:
            if (engine_state.target_rpm > 1500) {
                new_state = RUNNING_STATE;
                engine_state.fuel_injection_percentage = 60;
            }
            break;
            
        case RUNNING_STATE:
            if (engine_state.target_rpm > engine_state.current_rpm + 500) {
                new_state = ACCELERATING_STATE;
            } else if (engine_state.target_rpm < engine_state.current_rpm - 500) {
                new_state = DECELERATING_STATE;
            } else if (engine_state.target_rpm < 1000) {
                new_state = IDLE_STATE;
            }
            break;
            
        case ACCELERATING_STATE:
            if (engine_state.current_temperature > 120) {
                new_state = ERROR_STATE;
                log_error(0x2003, "Over temperature during acceleration", true);
            } else if (engine_state.current_rpm >= engine_state.target_rpm) {
                new_state = RUNNING_STATE;
            }
            break;
            
        case DECELERATING_STATE:
            if (engine_state.current_rpm <= engine_state.target_rpm) {
                new_state = RUNNING_STATE;
            }
            break;
            
        case ERROR_STATE:
            /* Hata durumundan çıkış denemesi */
            if (engine_state.last_error.recovery_attempts < 3) {
                if (engine_state.current_temperature < 100) {
                    new_state = IDLE_STATE;
                    engine_state.last_error.recovery_attempts++;
                }
            }
            break;
            
        case SHUTDOWN_STATE:
            /* Kapalı durumda kalır */
            break;
            
        default:
            new_state = IDLE_STATE;
            break;
    }
    
    /* Durum değişimi tetikleme */
    if (new_state != engine_state.state) {
        engine_trigger_state_change_callback(new_state);
    }
    
    return new_state;
}

/**
 * @brief Hata recovery state machine (KOMPLEKS MANTIK)
 */
EngineState_t engine_handle_error_recovery_state(void) {
    if (!engine_state.is_initialized) {
        return ERROR_STATE;
    }
    
    if (engine_state.state != ERROR_STATE) {
        return engine_state.state;
    }
    
    /* Hata türüne göre recovery stratejisi */
    uint16_t error_code = engine_state.last_error.error_code;
    
    if (error_code >= 0x5000 && error_code <= 0x5FFF) {
        /* Sıcaklık hatası: Soğutma fanını artır */
        if (engine_state.cooling_fan_speed < 100) {
            engine_state.cooling_fan_speed = 100;
        } else if (engine_state.current_temperature < 80) {
            return IDLE_STATE;  /* Recovery başarılı */
        }
    } else if (error_code >= 0x3000 && error_code <= 0x3FFF) {
        /* RPM hatası: RPM'i normalize et */
        engine_state.target_rpm = 1000;
        engine_state.rpm_acceleration_rate = 30;
        if (engine_state.current_rpm <= 1500) {
            return IDLE_STATE;  /* Recovery başarılı */
        }
    } else if (error_code >= 0x4000 && error_code <= 0x4FFF) {
        /* Yakıt hatası: Enjeksiyonu sıfırla */
        engine_state.fuel_injection_percentage = 50;
        sensor_calibrate_fuel_pressure_sensor(350);
        if (engine_state.fuel_pressure >= 300 && engine_state.fuel_pressure <= 400) {
            return IDLE_STATE;  /* Recovery başarılı */
        }
    }
    
    return ERROR_STATE;  /* Recovery devam ediyor */
}

/**
 * @brief Nested loop performans optimizasyonu
 */
uint16_t engine_sequential_performance_optimization(uint8_t iterations) {
    if (!engine_state.is_initialized || iterations == 0) {
        return 0;
    }
    
    uint16_t total_score = 0;
    
    /* Dış döngü: Her iterasyonda */
    for (uint8_t iteration = 0; iteration < iterations; iteration++) {
        uint16_t iteration_score = 0;
        
        /* İç döngü 1: RPM optimizasyonu */
        for (uint8_t rpm_idx = 0; rpm_idx < 5; rpm_idx++) {
            uint16_t test_rpm = 1000 + (rpm_idx * 1000);
            if (test_rpm <= engine_state.max_rpm_limit) {
                engine_set_target_rpm(test_rpm);
                if (engine_check_rpm_stability()) {
                    iteration_score += 20;
                }
            }
        }
        
        /* İç döngü 2: Yakıt optimizasyonu */
        for (uint8_t fuel_idx = 0; fuel_idx < 4; fuel_idx++) {
            uint8_t fuel_level = 60 + (fuel_idx * 10);
            if (fuel_level <= 100) {
                engine_set_fuel_injection_percentage(fuel_level);
                float consumption = engine_calculate_fuel_consumption();
                if (consumption < 10.0f) {
                    iteration_score += 25;
                }
            }
        }
        
        /* İç döngü 3: Soğutma optimizasyonu */
        for (uint8_t cool_idx = 0; cool_idx < 3; cool_idx++) {
            uint8_t fan_speed = cool_idx * 50;
            engine_set_cooling_fan_speed(fan_speed);
            if (engine_check_temperature_warning()) {
                iteration_score += 15;
            }
        }
        
        total_score += iteration_score;
    }
    
    return total_score / iterations;
}

/**
 * @brief Dinamik throttle kontrolü (RPM ve sıcaklığa göre)
 */
uint16_t engine_dynamic_throttle_control(void) {
    uint16_t new_throttle = engine_state.throttle_position;
    
    /* RPM'e göre dinamik throttle */
    if (engine_state.current_rpm < 1000) {
        new_throttle = 10;
    } else if (engine_state.current_rpm < 2000) {
        new_throttle = 25;
    } else if (engine_state.current_rpm < 4000) {
        new_throttle = 50;
    } else if (engine_state.current_rpm < 6000) {
        new_throttle = 75;
    } else {
        new_throttle = 100;
    }
    
    /* Sıcaklık ile oynama */
    if (engine_state.current_temperature > 100) {
        /* Sıcaksa throttle'ı azalt */
        new_throttle = (new_throttle * 80) / 100;
    } else if (engine_state.current_temperature < 20) {
        /* Soğuksa throttle'ı artır */
        new_throttle = (new_throttle * 110) / 100;
        if (new_throttle > 100) new_throttle = 100;
    }
    
    engine_set_throttle_position(new_throttle);
    return new_throttle;
}

/**
 * @brief Turbo boost adaptif kontrolü (state-based)
 */
uint16_t engine_adaptive_turbo_control(void) {
    uint16_t new_boost = engine_state.boost_pressure;
    
    if (!engine_state.turbo_enabled) {
        return 0;
    }
    
    /* Durum tabanlı boost kontrolü */
    if (engine_state.state == ACCELERATING_STATE) {
        /* Hızlanırken boost'u artır */
        if (engine_state.current_rpm > 5000) {
            new_boost = 220;
        } else if (engine_state.current_rpm > 4000) {
            new_boost = 180;
        } else {
            new_boost = 140;
        }
    } else if (engine_state.state == DECELERATING_STATE) {
        /* Yavaşlarken boost'u azalt */
        new_boost = (new_boost * 60) / 100;
    } else if (engine_state.state == RUNNING_STATE) {
        /* Normal çalışmada moderate boost */
        new_boost = 100;
    } else {
        /* Diğer durumlarda boost'u kapat */
        new_boost = 0;
    }
    
    /* Sıcaklık kontrol */
    if (engine_state.current_temperature > 110) {
        new_boost = (new_boost * 50) / 100;
    }
    
    engine_set_turbo_boost_pressure(new_boost);
    return new_boost;
}

/**
 * @brief Yakıt enjeksiyonu state machine
 */
bool engine_fuel_injection_state_machine(void) {
    bool valid = true;
    
    /* Mevcut moddan sonraki moda geçiş */
    switch (engine_state.fuel_mode) {
        case FUEL_INJECTION_NORMAL:
            if (engine_state.current_rpm > 5000 || engine_state.current_temperature > 100) {
                engine_state.fuel_mode = FUEL_INJECTION_RICH;
            }
            break;
            
        case FUEL_INJECTION_RICH:
            if (engine_state.current_rpm < 3000 && engine_state.current_temperature < 80) {
                engine_state.fuel_mode = FUEL_INJECTION_NORMAL;
            } else if (engine_state.current_rpm > 6500) {
                engine_state.fuel_mode = FUEL_INJECTION_LEAN;
            }
            break;
            
        case FUEL_INJECTION_LEAN:
            if (engine_state.current_rpm < 4000) {
                engine_state.fuel_mode = FUEL_INJECTION_NORMAL;
            } else if (engine_state.current_temperature > 120) {
                engine_state.fuel_mode = FUEL_INJECTION_CUTOFF;
                valid = false;
            }
            break;
            
        case FUEL_INJECTION_CUTOFF:
            if (engine_state.current_temperature < 90 && engine_state.current_rpm < 2000) {
                engine_state.fuel_mode = FUEL_INJECTION_NORMAL;
            } else {
                valid = false;
            }
            break;
            
        default:
            engine_state.fuel_mode = FUEL_INJECTION_NORMAL;
            break;
    }
    
    return valid;
}

/**
 * @brief Tüm limitleri kontrol eden güvenlik fonksiyonu
 */
bool engine_check_all_safety_limits(void) {
    bool safe = true;
    
    /* RPM limitleri */
    if (engine_state.current_rpm > engine_state.max_rpm_limit) {
        safe = false;
        log_error(0x3007, "RPM limit exceeded", true);
    }
    
    /* Sıcaklık limitleri */
    if (engine_state.current_temperature > 140 || engine_state.current_temperature < -30) {
        safe = false;
        log_error(0x5008, "Temperature out of safe range", true);
    }
    
    /* Yakıt basıncı limitleri */
    if (engine_state.fuel_pressure > 600 || engine_state.fuel_pressure < 150) {
        safe = false;
        log_error(0x4006, "Fuel pressure out of safe range", true);
    }
    
    /* Turbo boost limitleri */
    if (engine_state.turbo_enabled && engine_state.boost_pressure > 280) {
        safe = false;
        log_error(0x6004, "Turbo boost limit exceeded", true);
    }
    
    /* Throttle limitleri */
    if (engine_state.throttle_position > 100) {
        safe = false;
        log_error(0x8007, "Throttle position invalid", true);
    }
    
    return safe;
}
 
