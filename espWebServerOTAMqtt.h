#ifndef espWebServerOTAMqtt_h
#define espWebServerOTAMqtt_h

#include "Arduino.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson


class MyCommon {
  	ESP8266WebServer 	*_server;
	public:
	MyCommon();
	MyCommon(char* hostname);
	void   				setup();
	boolean				setup_wifi();
	boolean   			setup_wifi_sta();
	boolean				setup_wifi_soft_ap();
	void				set_hostname(char* hostname);
	void				loop();
	void				handle_index();
	void				handle_reboot();
	void				handle_reset();
	void				handle_onNotFound();
	String				get_uptime_as_str();
	void				set_main_page_html_body(String body);
	String 				generate_html_page(const char* body);
	String 				generate_html_table_row (String tag, String key, String value);
	boolean				mqtt_connect();
	boolean 			publish(const String &topic, const String &payload, bool retain);
	unsigned long  		get_elapsed_secs(unsigned long ms_start);

	private:
	WiFiClient 			_espClient;
	PubSubClient 		*_mqttClient;
	char 				_hostname[20] 	 	= "mortylabs";
	char				_ssid[20] 		 	= "XXX";
	char				_ssid_pass[20] 	 	= "XXX";
    char 				_mqtt_server[15] 	= "XXX";
    char				_mqtt_port[8] 		= "1883";
    char 				_mqtt_user[20] 		= "XXX";
    char 				_mqtt_pass[20] 		= "XXX";
	boolean 			_debug 				= true;	
	float				_deep_sleep_ms 		= 0;
	int					_menu_index 		= 0;
	unsigned long 		_ms_startup		;
	boolean				_battery_enabled;
	int					_battery_perc;
	int					_battery_volts;
	unsigned long		_battery_last_checked;
	String              _html_index_body;
	unsigned long		_ms_started_WIFI_AP;
	void 				handleForm();
	void 				handle_setup();
	boolean				read_SPIFFS();
	void				save_SPIFFS();
};

#endif
