#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <stdio.h>
#include <SHTSensor.h>
#include "wifi_info.h"

#include <IRsend.h>
#include <ir_Coolix.h>
//#include <ir_Fujitsu.h>

#define LOG_PRINT(fmt, args...) printf(("%s,%s,LINE%d: " fmt "\n"), __FILE__, __func__, __LINE__, ##args)
#define SENSOR_TEMP_OFFSET 0.0
#define SENSOR_HUM_OFFSET 0.0

void my_homekit_setup();
void my_homekit_loop();
void update_status();
void th_sensor_sample();

// IR settings
// https://github.com/crankyoldgit/IRremoteESP8266/blob/master/src/IRremoteESP8266.h line 1011
// https://github.com/crankyoldgit/IRremoteESP8266/blob/d8aee22117b826945724dfb4f1b94d82d68f31f0/src/IRremoteESP8266.h#L1011
IRCoolixAC ac(14); // Set IR_TX be used to sending the IR_LED message
// IRFujitsuAC ac(14);
// IRac ac(14);

// Globals
bool queueCommand = false;
void flipQueueCommand(bool newState)
{
	// LOG_PRINT("Flipping queueCommand to %d\n", newState);
	queueCommand = newState;
}

SHTSensor sht;

void Led_int()
{
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);
}


void setup()
{
	Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
	// Serial.begin(115200);
	while (!Serial) // Wait for the serial connection to be establised.
		delay(50);
	Led_int();

	wifi_connect();
	
	Wire.begin(4, 5);
	sht.init();	
	sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
	
	ac.begin();
	
	my_homekit_setup();
	WiFi.setSleepMode(WIFI_LIGHT_SLEEP); // WIFI_NONE_SLEEP、WIFI_LIGHT_SLEEP、WIFI_MODEM_SLEEP
}

void loop()
{
	my_homekit_loop();
	delay(10);

	if (queueCommand)
	{
		Serial.printf("Homekit App send ir...\n");
		ac.send();
		flipQueueCommand(false);
	}
}


extern "C" homekit_server_config_t config;

extern "C" homekit_characteristic_t ac_name;
extern "C" homekit_characteristic_t optional_name;
extern "C" homekit_characteristic_t serial_number;
extern "C" homekit_characteristic_t ac_active;
extern "C" homekit_characteristic_t current_temperature;
extern "C" homekit_characteristic_t current_relative_humidity;
extern "C" homekit_characteristic_t current_heater_cooler_state;
extern "C" homekit_characteristic_t target_heater_cooler_state;
extern "C" homekit_characteristic_t cooling_threshold_temperature;
extern "C" homekit_characteristic_t rotation_speed;
extern "C" homekit_characteristic_t swing_mode;

extern "C" void accessory_init();

// get temperature and humidity from sensor
void th_sensor_sample()
{
	if (sht.readSample())
	{
		float temperature_c = sht.getTemperature() + SENSOR_TEMP_OFFSET;
		float humidity = sht.getHumidity() + SENSOR_HUM_OFFSET;

		// temperature_f=temperature_c*1.8+32;
		// temperature_f=celsiusToFahrenheit(temperature_c);

		if (temperature_c != current_temperature.value.float_value || humidity != current_relative_humidity.value.float_value)
		{
			current_temperature.value = HOMEKIT_FLOAT(temperature_c);
			current_relative_humidity.value = HOMEKIT_FLOAT(humidity);
			homekit_characteristic_notify(&current_temperature, current_temperature.value);
		}
	}
	else
	{
		Serial.print("Error in readSample()\n");
	}
}

void ac_active_setter(const homekit_value_t value)
{
	bool on = value.bool_value;
	ac_active.value.bool_value = on; // sync the value
	on ? ac.on() : ac.off();
	update_status();
}

void target_heater_cooler_state_setter(const homekit_value_t value)
{
	uint8 state = value.uint8_value;
	target_heater_cooler_state.value = HOMEKIT_UINT8(state);
	update_status();
}

void cooling_threshold_temperature_setter(const homekit_value_t value)
{
	float ctemp = value.float_value;
	cooling_threshold_temperature.value = HOMEKIT_FLOAT(ctemp);
	ac.setTemp(ctemp);
	flipQueueCommand(true);
}

void rotation_speed_setter(const homekit_value_t value)
{
	uint8 sp = value.float_value;
	uint8 newsp = (uint8)(sp / 20);
	rotation_speed.value = HOMEKIT_FLOAT(((float)newsp) * 20);
	// coolix Fan: 7 (Fixed),Fan: 4 (Min),Fan: 2 (Med),Fan: 1 (Max),Fan: 5 (Auto)
	switch (newsp)
	{
	case 5:
		ac.setFan(ac.convertFan(stdAc::fanspeed_t::kAuto));
		break;
	case 4:
		ac.setFan(ac.convertFan(stdAc::fanspeed_t::kMax));
		break;
	case 3:
		ac.setFan(ac.convertFan(stdAc::fanspeed_t::kMedium));
		break;
	case 2:
		ac.setFan(4);
		break;
	case 1:
		ac.setFan(7);
		break;
	case 0:
		ac.setFan(7);
		break;
	}
	flipQueueCommand(true);
}

void swing_mode_setter(const homekit_value_t value)
{
	uint8 state = value.uint8_value;
	swing_mode.value = HOMEKIT_UINT8(state);

	state ? ac.setSwing() : ac.setSwing();

	if (ac_active.value.bool_value)
	{
		flipQueueCommand(true);
	}
}

void update_status()
{
	homekit_value_t new_current_state;
	if (ac_active.value.bool_value == false)
	{
		new_current_state = HOMEKIT_UINT8(0);
	}
	else
	{
		new_current_state = HOMEKIT_UINT8(3);
		// kMideaACCool
		ac.setMode(ac.convertMode(stdAc::opmode_t::kCool));
		ac.setTemp(cooling_threshold_temperature.value.float_value);
	}
	if (!homekit_value_equal(&new_current_state, &current_heater_cooler_state.value))
	{
		current_heater_cooler_state.value = new_current_state;
		homekit_characteristic_notify(&current_heater_cooler_state, current_heater_cooler_state.value);
		flipQueueCommand(true);
	}
}

static uint32_t next_heap_millis = 0;
static uint32_t next_th_sensor_sample_millis = 0;

void my_homekit_setup()
{
	ac_active.setter = ac_active_setter;
	target_heater_cooler_state.setter = target_heater_cooler_state_setter;
	cooling_threshold_temperature.setter = cooling_threshold_temperature_setter;
	rotation_speed.setter = rotation_speed_setter;
	swing_mode.setter = swing_mode_setter;
	
	uint8_t mac[6];
	WiFi.macAddress(mac);
	sprintf(ac_name.value.string_value, "HAC-%02X%02X%02X", mac[3], mac[4], mac[5]);
	sprintf(serial_number.value.string_value,"%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
	accessory_init();
	arduino_homekit_setup(&config);
}

void my_homekit_loop()
{
	arduino_homekit_loop();
	const uint32_t t = millis();
	if (t > next_th_sensor_sample_millis)
	{
		// sensor sample every 30 seconds
		next_th_sensor_sample_millis = t + 30 * 1000;
		th_sensor_sample();
	}
	if (t > next_heap_millis)
	{
		// show heap info every 5 seconds
		next_heap_millis = t + 15 * 1000;
		INFO("Free heap: %d, HomeKit clients: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
	}
}
