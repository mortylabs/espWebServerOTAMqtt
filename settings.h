const char SETUP_page[] PROGMEM = R"=====(

<h3>Settings</h3>

<form action="/action_page">
<b> WiFi Settings</b><BR>
  SSID:<br>
  <input type="text" name="ssid" value="__SSID__">
  <br>
  password:<br>
  <input type="password" name="ssid_pass" value="__SSID_PASS__">
  <br>
  hostname:<br>
  <input type="text" name="hostname" value="__HOSTNAME__">
  <br>
  <br>
  <b> MQTT Settings </b><BR>
  server:<br>
  <input type="text" name="mqtt_server" value="__MQTT_SERVER__">
  <br>
  port:<br>
  <input type="text" name="mqtt_port" value="__MQTT_PORT__">
  <br>
  user:<br>
  <input type="text" name="mqtt_user" value="__MQTT_USER__">
  <br>
  password:<br>
  <input type="password" name="mqtt_pass" value="__MQTT_PASS__">
  <br>
  <br><br>
  <input type="submit" class="buttonGreenSmall buttonGrey" name="action" value="Cancel">
  <input type="submit" class="buttonGreenSmall" name="action" value="Save">
  <input type="submit" class="buttonGreenSmall buttonRed" name="action" value="Reboot">
 
</form> 

</body>
</html>
)=====";

