ESP32_DeVICE_Module for SCDE (Smart Connedcted Device Engine) V2

- ESP32_ -> intended for use on ESP32 SoC hardware only ! 

- DeVICE -> DeVICE Feature. It is used to control the device that runs the SCDE. (E.G. Wifi settings, reboot cmd, ...)

- _Module -> intended for use with SCDE (Smart Connected Device Engine) only !

Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany, for EcSUHA.de

MSchulze780@GMAIL.COM

Copyright by EcSUHA

This is part of

- ESP 8266EX & ESP32 SoC Activities ...

- HoME CoNTROL & Smart Connected Device Engine Activities ...
 
EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE


To use it: Define an definition with an custom name 'ThisDeVICE' of the module 'ESP32_DeVICE'
 and KEY&VALUES to assign initial settings.
define ThisDeVICE ESP32_DeVICE Name=maik&WSAP_Password=pw&WSAP_RF_Channel=1&WSAP_Maximal_Connections=3&WSAP_Authentication_Method=WPA2_PSK
de
Initial settings (KEY&VALUES) will be used if not overridden by setstate cmd from boot process.

Block 1
Name=[a-zA-Z0-9_.] -> Setze Device-Name = WSAP-Name (char) (0-sizeof (ap_wifi_config.ap.ssid)-1)
Name=[a-zA-Z0-9_.] -> Set Device-Name = WSAP-Name (char) (0-sizeof (ap_wifi_config.ap.ssid)-1)

WSAP_Password=[a-zA-Z0-9_.] -> Setze das Wireless Service Access Point Passwort (char) (0-sizeof (ap_wifi_config.ap.password)-1)
WSAP_Password=[a-zA-Z0-9_.] -> Set the Wireless Service Access Point Passwort (char) (0-sizeof (ap_wifi_config.ap.password)-1)

WSAP_RF_Channel=[0-9] -> Setze den Wireless Service Access Point Kanal (1-13 aber Station Kanal hat Priorität!)
WSAP_RF_Channel=[0-9] -> Set the Wireless Service Access Point channel (1-13 but Station Channel has priority)

WSAP_Maximal_Connections=[0-9] -> Setze die maximale Anzahl der Verbindungen zum WSAP (0-4)
WSAP_Maximal_Connections=[0-9] -> Set maximum number of connections to the WSAP (0-4)

WSAP_Authentication_Method=[OPEN|WEP|WPA_PSK|WPA2_PSK|WPA_WPA2_PSK|WPA2_ENTERPRISE|MAX] -> Setze die Authentifizierungsmethode für den Wireless Service Access Point
WSAP_Authentication_Method=[OPEN|WEP|WPA_PSK|WPA2_PSK|WPA_WPA2_PSK|WPA2_ENTERPRISE|MAX] -> Set the authentication method for the Wireless Service Access Point


WSAP_Password
WSAP_RF_Channel
WSAP_Maximal_Connections
WSAP_Authentication_Method
WSAP_SSID
WSAP_Beacon_Interval


