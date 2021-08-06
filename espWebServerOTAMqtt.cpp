#include "Arduino.h"
#include "espWebServerOTAMqtt.h"
#include "index.h"
#include "settings.h"


MyCommon::MyCommon (char* hostname) {
  Serial.begin(115200);
  delay(100);
  while (!Serial) { delay(1); }
  strcpy(_hostname, hostname);
  _debug    			= true;
  _ms_startup 			= millis();
  _battery_last_checked = millis()- 300e3; 
  _battery_enabled		= false;
  _battery_perc 		= -999;
  _battery_volts 		= -999;
  _deep_sleep_ms		= 0;
}


MyCommon::MyCommon() : MyCommon ("mortylabs") {}


void MyCommon::set_hostname(char* hostname) {
	strcpy(_hostname, hostname);
}
	

unsigned long  MyCommon::get_elapsed_secs(unsigned long ms_start) {
  return (millis() - ms_start) / 1000;
}


String MyCommon::get_uptime_as_str()  {
  unsigned long secs_elapsed = get_elapsed_secs(_ms_startup);
  unsigned long hours = secs_elapsed / 60 / 60;
  unsigned long mins = secs_elapsed / 60.00 - hours * 60;
  unsigned long secs = secs_elapsed - hours * 60 * 60 - mins * 60;
  unsigned long days = hours / 24;
  hours = hours - days*24;
  String up_time = "uptime  : ";
  up_time += days;
  up_time += "d ";
  if (hours < 10) {
    up_time += "0";
  }
  up_time += hours;
  up_time += ":";
  if (mins < 10 ) {
    up_time += "0";
  }
  up_time += mins;
  up_time += ":";
  if (secs < 10 ) {
    up_time += "0";
  }
  up_time += secs;
  return up_time;
}


void MyCommon::loop() {
  if (WiFi.status() != WL_CONNECTED) {
  }
  if (!_mqttClient -> connected() ) { mqtt_connect(); }
  _server -> handleClient(); 
  if (WiFi.status() != WL_CONNECTED and WiFi.getMode() == WIFI_AP and millis() - _ms_started_WIFI_AP > 180E3) {
		Serial.println("3 mins elapsed, rebooting...");
		delay(200);
		ESP.restart();
  }
  if (_mqttClient -> connected()) { _mqttClient -> loop();     }
  ArduinoOTA.handle();

}	



void MyCommon::handle_reboot() {
  _menu_index = 0;
  _server -> sendHeader("Location","/");  
  _server -> send(303, "text/html", generate_html_page("restarting..."));
  delay(3000); 
  ESP.restart(); 

}	


void MyCommon::handle_reset() {
 _server -> sendHeader("Location","/");  
 _server -> send(303);   
  SPIFFS.format();
  delay(3000); 
  ESP.restart();

}	



void MyCommon::handle_setup() {
 _menu_index = 1;
 String s = SETUP_page; 
 s.replace("__HOSTNAME__", _hostname);
 s.replace("__SSID__", _ssid);
 s.replace("__SSID_PASS__", _ssid_pass);
 s.replace("__MQTT_SERVER__", _mqtt_server);
 s.replace("__MQTT_PORT__", _mqtt_port);
 s.replace("__MQTT_USER__", _mqtt_user);
 s.replace("__MQTT_PASS__", _mqtt_pass);
 _server -> send(200, "text/html", generate_html_page(s.c_str())); 

}

void MyCommon::handleForm() {
	String _action = _server -> arg("action").c_str();
	if (_action.equals("Reboot")) {
		_server -> sendHeader("Location","/");  
  _server -> send(303);  
  delay(4000);
  ESP.restart();
  }
  if (_action.equals("Save")) {
	strcpy(_hostname, 	_server -> arg("hostname").c_str()); 
	strcpy(_ssid, 		_server -> arg("ssid").c_str()); 
	strcpy(_ssid_pass, 	_server -> arg("ssid_pass").c_str()); 
	strcpy(_mqtt_server, _server -> arg("mqtt_server").c_str()); 
	strcpy(_mqtt_port, 	_server -> arg("mqtt_port").c_str()); 
	strcpy(_mqtt_user, 	_server -> arg("mqtt_user").c_str()); 
	strcpy(_mqtt_pass, 	_server -> arg("mqtt_pass").c_str()); 
	save_SPIFFS();
	delay(100);
	if (WiFi.getMode() == WIFI_AP) {
		ESP.restart();
	}
	_mqttClient -> disconnect();
	mqtt_connect();
  }
  _server -> sendHeader("Location","/");  
  _server -> send(303);   

}



void MyCommon::handle_onNotFound() {

 _server -> sendHeader("Location","/");  
 _server -> send(404, "text/html", generate_html_page("Ungaas baba, I 404 you...")); 

}	
	
	
String MyCommon::generate_html_table_row (String tag, String key, String value) {
  String s = "<tr><" + tag + " style='text-align:left'>";
  s += key;
  s += "</" + tag + "><" + tag + " style='text-align:left'>";
  s += value;
  s += "</" + tag + "></tr>";
  return s;
}


String MyCommon::generate_html_page(const char* body)  {
 String s= HEADER_page;
 if (_menu_index == 1) {
	 s.replace("__MENU1__", "buttonGreen buttonGrey");
	 s.replace("__MENU2__", "buttonGreen");
 } else {
	 s.replace("__MENU1__", "buttonGreen");
	 s.replace("__MENU2__", "buttonGreen buttonGrey");
 }

 s.replace("__HOSTNAME__", _hostname);
 s =s + body;
 s =s + "</body></html>";
 return s;
 
}
	


void MyCommon::handle_index() {
  
  _menu_index = 0;
  String s = "<BR><b>";
  s += _hostname;
  s += "</b><br><br>";

  s += "startup : ";
  s += ESP.getResetReason();
  
  //if (_battery_enabled) {
//	  s = s + "\nbattery : ";
//	  s = s + get_battery_perc();
//	  s = s + "%  ";
//	  s = s + get_battery_volts();	
//	  s = s + "v  (A0 ";
//	  s = s + _analog_value;
//	  s = s + "v)";
//  }
  s += "<BR>"+get_uptime_as_str()+"<BR><BR>";
  s += "mqtt: ";
  if   (_mqttClient ->connected() ) {
	    s += "<b><font color=\"green\">CONNECTED</font></b>"; } 
  else {s += "<b><font color=\"red\">DISCONNECTED</font></b>"; } 
  s= generate_html_page((s+ _html_index_body).c_str());
  s.replace("__HOSTNAME__", _hostname);
  _server -> send(200, "text/html", s); 

}




boolean MyCommon::read_SPIFFS() {

  if (SPIFFS.exists("/config.json")) {
		Serial.println("reading SPIFFS config file");
		File configFile = SPIFFS.open("/config.json", "r");
		if (configFile) {
			Serial.println("opened SPIFFS config file");
			size_t size = configFile.size();
			// Allocate buffer to store contents of config file
			std::unique_ptr<char[]> buf(new char[size]);

			configFile.readBytes(buf.get(), size);
			DynamicJsonBuffer jsonBuffer;
			JsonObject& json = jsonBuffer.parseObject(buf.get());
			json.printTo(Serial);
			if (json.success()) {
				strcpy(_ssid, json["ssid"]);
				strcpy(_ssid_pass, json["ssid_pass"]);
				strcpy(_mqtt_server, json["mqtt_server"]);
				strcpy(_hostname, json["hostname"]);
				strcpy(_mqtt_port, json["mqtt_port"]);
				strcpy(_mqtt_user, json["mqtt_user"]);
				strcpy(_mqtt_pass, json["mqtt_pass"]);
			} else {
				Serial.println("FAILED to load json config");
				return false;
			} 
		} else {
			Serial.println("FAILED  to open config file");
			return false;
		}
	} else {
			Serial.println("FAILED /config.json doesn't exist!"); 
			return false;
	}
  return true;

}



void MyCommon::save_SPIFFS() {

    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["ssid"] = _ssid;    
    json["ssid_pass"] = _ssid_pass;    
	json["hostname"] = _hostname;
    json["mqtt_server"] = _mqtt_server;
    json["mqtt_port"]   = _mqtt_port;
    json["mqtt_user"]   = _mqtt_user;
    json["mqtt_pass"]   = _mqtt_pass;
	Serial.println("writing to SPIFFS");
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
	Serial.println("SPIFFS save done");
}	



boolean MyCommon::mqtt_connect() {
  boolean _ret = _mqttClient -> connected();
  //if (WiFi.getMode() == WIFI_AP) {Serial.println("WIFI_AP");} 
  //if (WiFi.getMode() == WIFI_STA) {Serial.println("WIFI_STA");}
  //if (WiFi.getMode() == WIFI_AP_STA ) {Serial.println("WIFI_AP_STA "); }
  //if (WiFi.getMode() == WIFI_OFF ) {Serial.println("WIFI_OFF ");}
  
  if (!(WiFi.getMode() == WIFI_AP)  and !_ret ) {
	_mqttClient ->setServer(_mqtt_server, atoi(_mqtt_port));
	unsigned long millis_mqtt_started = millis();
	while (!_mqttClient -> connected() and (millis() - millis_mqtt_started <= 5E3)) {
		Serial.print("Attempting MQTT connection...");
		if (_mqttClient -> connect( _hostname, _mqtt_user, _mqtt_pass)) {
			Serial.println("CONNECTED!");
			_ret = true;
		} else {
			Serial.println("NOT connected!");
		}
	}
  } else {
	 // 
  }
  return _ret;

}




boolean MyCommon::setup_wifi_soft_ap() {

  _ms_started_WIFI_AP = millis();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP);
  WiFi.persistent(false);
  WiFi.setOutputPower(20.5);
  String _mac = String(WiFi.macAddress());
  _mac.toLowerCase();
  _mac.replace(":", "");
  _mac.replace("240ac4", "a");            // vendor = Espressif
  String _clientId = "mortylabs_" + _mac;
  WiFi.softAP(_clientId.c_str(), "");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP mode, IP = ");
  Serial.println(myIP);
  return true;
  
}



boolean MyCommon::setup_wifi() {

	boolean _ret = false;
	Serial.println("setup_wifi");
	if (_ssid != NULL and _ssid != "") {_ret = setup_wifi_sta(); }
  if (!_ret ) {
		_ret = setup_wifi_soft_ap();
  }
  return _ret;

}


boolean MyCommon::setup_wifi_sta() {

  Serial.println("wifi_setup_soft_sta");

  int led_status = 0;

  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setOutputPower(20.5);
  WiFi.hostname(_hostname);
  WiFi.setAutoReconnect(true);
  WiFi.begin(_ssid,_ssid_pass);
  Serial.println(_ssid);
  Serial.println(_ssid_pass);

  unsigned long ms_start_millis = millis();
  while (WiFi.status() != WL_CONNECTED and get_elapsed_secs(ms_start_millis) <= 15) {
    delay(500);
    if (_debug) { Serial.print("."); }
    if (led_status == 1) {
      led_status = 0;
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      led_status = 1;
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  
  int _wifi_status  = WiFi.status();
  Serial.print("WiFi Status: ");
  switch(_wifi_status) {
	  case WL_CONNECTED  : Serial.println("WL_CONNECTED"); break;
	  case WL_NO_SSID_AVAIL   : Serial.println("WL_NO_SSID_AVAIL "); break;
	  case WL_CONNECT_FAILED   : Serial.println("WL_CONNECT_FAILED "); break;
	  case 6   : Serial.println("WL_CONNECT_WRONG_PASSWORD "); break;
	  case WL_IDLE_STATUS   : Serial.println("WL_IDLE_STATUS "); break;
	  case WL_DISCONNECTED    : Serial.println("WL_DISCONNECTED"); break;
	  default:	Serial.println("UNKNOWN"); break;
  }
  return (WiFi.status() == WL_CONNECTED); 

}




void MyCommon::setup() {
  if (_debug) {Serial.println("setup -> debug is ENABLED");}
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  boolean SPIFFS_ok = false;
  if (SPIFFS.begin()) {
   //SPIFFS.format();
   //ESP.restart();
	Serial.println("SPIFFS begin OK");
	SPIFFS_ok = read_SPIFFS();
  } else { 
	Serial.println("SPIFFS begin FAIL");
  }
  
  _server = new ESP8266WebServer(80);
  if (!SPIFFS_ok)  {
	setup_wifi_soft_ap();
  } else {
	setup_wifi();
  }
  
  if (WiFi.status() != WL_CONNECTED and _deep_sleep_ms > 0) { 
			if (_debug ) {Serial.print("WiFi FAIL, deep sleep for "); Serial.println(_deep_sleep_ms /1000000.00);}
			delay(100);
			ESP.deepSleep(_deep_sleep_ms );
  }
  if (WiFi.getMode() == WIFI_AP) {
	_server->on("/", std::bind(&MyCommon::handle_setup, this));
  } else {
	_server->on("/", std::bind(&MyCommon::handle_index, this));
  }
  _server->on("/reboot", std::bind(&MyCommon::handle_reboot, this));
  _server->on("/reset", std::bind(&MyCommon::handle_reset, this));
  _server->on("/setup", std::bind(&MyCommon::handle_setup, this));
  _server->on("/action_page", std::bind(&MyCommon::handleForm, this));
  _server -> begin();  
  
  _mqttClient = new PubSubClient(_espClient);
  mqtt_connect();
  
 if (WiFi.getMode() == WIFI_STA	 or WiFi.getMode() == WIFI_AP_STA	) {
	 ArduinoOTA.setHostname(_hostname);
	ArduinoOTA.setPassword(_ssid_pass);
	ArduinoOTA.begin();
 }

}



void MyCommon::set_main_page_html_body(String body) {

	_html_index_body = body;
	Serial.println(_html_index_body);

}



bool MyCommon::publish(const String &topic, const String &payload, bool retain) {

  if(!_mqttClient -> connected ()) {
    return false;
  }
  bool success = _mqttClient -> publish(topic.c_str(), payload.c_str(), retain);
  if(success) {
      Serial.printf("MQTT << [%s] %s\n", topic.c_str(), payload.c_str());
  }
  else {
      Serial.println("MQTT! publish failed, is the message too long ? (see PubSubClient.h:setMaxPacketSize())"); 
  }
  return success;

}
