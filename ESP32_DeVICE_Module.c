/* #################################################################################################
 *
 *  Function: ESP32_DeVICE Module for SCDE (Smart Connected Device Engine)
 *
 *  DeVICE -> DeVICE Feature
 *
 *  It is used to control an IoT Device (ESP32 - SoC and OS) that runs the SCDE 
 *  (Smart Connected Device Engine)
 *
 *  ESP 8266EX & ESP32 SOC Activities ...
 *  Copyright by EcSUHA
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  Copyright by EcSUHA
 *
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 */

/*
define xyz ESP32_DeVICE Name=maik&WSAP_Password=pw&WSAP_RF_Channel=1&WSAP_Maximal_Connections=3&WSAP_Authentication_Method=WPA2_PSK
define xyz 

*/

// Daten:
// 1. define (commmon/specific)
// 2. Attribute (Name/Content)
// 3. Readings (Time/data/Content)

// define myname Module Key&Values die direkt ins define gehen 
//    nicht sinvoll -> + Readings mit intialen Werten + Attribute mit initialen Werten 

// set myname + Readings mit Werten

// attr myname + Attribute mit Werten

#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>



// the Smart-Connected-Device-Engine structures & types ...
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module.h"

// this Modules structures & types ...
#include "ESP32_DeVICE_Module.h"







#include "freertos/event_groups.h"



// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef ESP32_DeVICE_Module_DBG  
#define ESP32_DeVICE_Module_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------




// FreeRTOS event group to signal when we are connected & ready to make a request
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;

// -------------------------------------------------------------------------------------------------



#include <WebIf_EspFSStdFileTX.h>
#include <WebIf_EspFSAdvFileTX.h>

#include "HTools_cgi.h"
#include <ServAPCfg_tpl.h>
#include <CGI_Redirect.h>





// -------------------------------------------------------------------------------------------------



/*
 * Implemented Values for Keys
 */

// ESP32_DeVICE_SET_SIG_OUT_EN -> 'SIG_OUT_EN' -> Disabled|Enabled
SelectAData ESP32_DeVICE_DisEn[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Disabled"},
  {1,"Enabled"}, 
  {0, NULL}
};

// ESP32_DeVICE_SET_MAX_CONN -> 'MAX_CONN' -> 1|2|3|4|5
SelectAData ESP32_DeVICE_MAX_CONN[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"1"},
  {1,"2"},
  {2,"3"},
  {3,"4"},
  {4,"5"},
  {0, NULL}
};

// ESP32_DeVICE_SET_CHANNEL -> 'CHANNEL' -> CH_1|CH_2|CH_3|CH_4|CH_5|CH_6|CH_7|CH_8|CH_9|CH_10|CH_11|CH_12|CH_13|CH_14
SelectAData ESP32_DeVICE_CHANNEL[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"1_N"},
  {1,"CH_1"},
  {2,"CH_2"},
  {3,"CH_3"},
  {4,"CH_4"},
  {5,"CH_5"},
  {6,"CH_6"},
  {7,"CH_7"},
  {8,"CH_8"},
  {9,"CH_9"},
  {10,"CH_10"},
  {11,"CH_11"},
  {12,"CH_12"},
  {13,"CH_13"},
  {14,"CH_14"},
  {0, NULL}
};

// ESP32_DeVICE_SET_CHANNEL -> 'SSID' -> Visible|Hidden
SelectAData ESP32_DeVICE_SSID[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Visible"},
  {1,"Hidden"},
  {0, NULL}
};

// ESP32_DeVICE_SET_AP -> 'AP' -> 1|2|3|4|5|6
SelectAData ESP32_DeVICE_AP[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"1"},
  {1,"2"},
  {2,"3"},
  {3,"4"},
  {4,"5"},
  {5,"6"},
  {0, NULL}
};

// ESP32_DeVICE_SET_PH_MODE -> 'PH_MODE' -> 802.11b|802.11g|802.11n
SelectAData ESP32_DeVICE_PH_MODE[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {1,"802.11b"},
  {2,"802.11g"},
  {3,"802.11n"},
  {0, NULL}
};

// ESP32_DeVICE_SET_WIFI_COUNTRY -> 'WIFI_COUNTRY' -> CN|JP|US|EU
SelectAData ESP32_DeVICE_WIFI_COUNTRY[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"CN"},
  {1,"JP"},
  {2,"US"},
  {3,"EU"},
  {0, NULL}
};

// ESP32_DeVICE_SET_AUTH_MODE -> 'AUTH_MODE' -> OPEN|WEP|WPA_PSK|WPA2_PSK|WPA_WPA2_PSK|WPA2_ENTERPRISE|MAX
SelectAData ESP32_DeVICE_AUTH_MODE[] = {  //authMode[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"OPEN"},
  {1,"WEP"},
  {2,"WPA_PSK"},
  {3,"WPA2_PSK"},
  {4,"WPA_WPA2_PSK"},
  {5,"WPA2_ENTERPRISE"},
  {6,"MAX"},
  {0, NULL}
};

// ESP32_DeVICE_SET_WIFI_BANDWIDTH -> 'WIFI_BANDWIDTH' -> HT20|HT40|HT??
SelectAData ESP32_DeVICE_WIFI_BANDWIDTH[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"HT20"},
  {1,"HT40"},
  {2,"HT??"},
  {0, NULL}
};



//define xyz ESP32_DeVICE Name=maik&WSAP_Password=pw&WSAP_RF_Channel=1&WSAP_Maximal_Connections=3&WSAP_Authentication_Method=WPA2_PSK


/**
 * For Type: ESP32_DeVICE
 * Implemented-Keys (Args input via Key=value) for SetFn input (MAX 64!)
 * KEYs-Field-Set for Feature specific query. Will be generated by SCDEH_ParseStrToparsedKVInput()
 * parsed result is stored in ESP32_DeVICE_parsedKVInput_t
 */
enum ESP32_DeVICE_Set_IK {				// Bit #XX for debugging

   // Block 1 - func
    ESP32_DeVICE_Set_IK_Name		= 0		// Bit #00 'Name'			-> 
  , ESP32_DeVICE_Set_IK_WSAP_Password			// Bit #01 'WSAP_Password'		-> 
  , ESP32_DeVICE_Set_IK_WSAP_RF_Channel		        // Bit #02 'WSAP_RF_Channel' 		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections	// Bit #03 'WSAP_Maximal_Connections'  	-> 
  , ESP32_DeVICE_Set_IK_WSAP_Authentication_Method	// Bit #04 'WSAP_Authentication_Method' -> 
  , ESP32_DeVICE_Set_IK_WSAP_SSID			// Bit #05 'WSAP_SSID'  		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval		// Bit #06 'WSAP_Beacon_Interval'  	-> 

   // Block 2 - func
  , ESP32_DeVICE_Set_IK_WSAP_IP_Adress			// Bit #07 'WSAP_IP_Adress'  		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Netmask			// Bit #08 'WSAP_Netmask'  		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Gateway_Adress		// Bit #09 'WSAP_Gateway_Adress'  	-> 

   // Block 3 - func
  , ESP32_DeVICE_Set_IK_WSAP_MAC_Adress		        // Bit #10 'WSAP_MAC_Adress'  		-> 

   // Block 4 - func
  , ESP32_DeVICE_Set_IK_WSAP_WiFi_Bandwidth		// Bit #11 'WSAP_WiFi_Bandwidth'	-> 

   // Block 5 - func
  , ESP32_DeVICE_Set_IK_WiFi_Country			// Bit #12 'WiFi_Country'		-> 

   // Block 6 - func
  , ESP32_DeVICE_Set_IK_Station_IP_Adress		// Bit #13 'Station_IP_Adress'  	-> 
  , ESP32_DeVICE_Set_IK_Station_Netmask		        // Bit #14 'Station_Netmask'  		-> 
  , ESP32_DeVICE_Set_IK_Station_Gateway_Adress		// Bit #15 'Station_Gateway_Adress'  	-> 

   // Block 7 - func
  , ESP32_DeVICE_Set_IK_Station_MAC_Adress		// Bit #16 'Station_MAC_Adress'		-> 

   // Block 8 - func
  , ESP32_DeVICE_Set_IK_Station_Auto_Connect		// Bit #17 'Station_Auto_Connect'	-> 

   // Block 9 - func
  , ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth		// Bit #18 'Station_WiFi_Bandwidth'	-> 






   // Block 3 - func
  , ESP32_DeVICE_Set_IK_WSAP_DHCPS_Lease_Start_IP	// Bit #10 'WSAP_DHCPS_Lease_Start_IP'  -> 
  , ESP32_DeVICE_Set_IK_WSAP_DHCPS_Lease_End_IP	        // Bit #11 'WSAP_DHCPS_Lease_End_IP'  	-> 

   // Block 4 - func
  , ESP32_DeVICE_Set_IK_WSAP_DHCPS			// Bit #12 'WSAP_DHCPS'  		-> 


   // Block 6 - func
  , ESP32_DeVICE_Set_IK_WSAP				// Bit #14 'WSAP'  			-> 


   // Block 8 - func
  , ESP32_DeVICE_Set_IK_Station			        // Bit #18 'Station'  			-> 

   // Block 9 - func
  , ESP32_DeVICE_Set_IK_Station_DHCP			// Bit #19 'Station_DHCP'		-> 


   // Block 11 - func
  , ESP32_DeVICE_Set_IK_Station_Physical_Mode		// Bit #21 'Station_Physical_Mode'	-> 

   // Block 12 - func
  , ESP32_DeVICE_Set_IK_Station_Get_RSSI		// Bit #22 'Station_Get_RSSI'		-> 




   // Block 14 - func
  , ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Main		// Bit #24 'TiSt_NTP_Server_Main'	-> 

   // Block 15 - func
  , ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_1	// Bit #25 'TiSt_NTP_Server_Backup_1'	-> 

   // Block 16 - func
  , ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_2	// Bit #26 'TiSt_NTP_Server_Backup_2'	-> 

   // Block 17 - func
  , ESP32_DeVICE_Set_IK_TiSt_Time_Zone			// Bit #27 'TiSt_Time_Zone'		-> 

   // Block 18 - func
  , ESP32_DeVICE_Set_IK_TiSt_Get_Time			// Bit #28 'TiSt_Get_Time'		-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Restart			// Bit #29 'SoCHW_Restart'		-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset_SCDE	// Bit #30 'SoCHW_Factory_Reset_SCDE'	-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset_CAPI	// Bit #31 'SoCHW_Factory_Reset_CAPI'	-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset		// Bit #32 'SoCHW_Factory_Reset'	-> 

   // helper to get number of implemented keys
  , ESP32_DeVICE_Set_NUMBER_OF_IK			// Bit #34 MAX 64 IMPLEMENTED !
};



/* For Type: ESP32_DeVICE
 * Implemented readings (MAX 32!)
 * Can be assigned to Implemented Keys, if affected
 */
enum ESP32_DeVICE_Readings {				 // Bit #XX for debugging
  // Internals - TiSt-Sharing - Block 1 - func
   ESP32_DeVICE_R_Name				= (1<<0) // Bit #01 'Name'			-> 
  ,ESP32_DeVICE_R_WSAP_Password			= (1<<1) // Bit #01 'WSAP_Password'		-> 
  ,ESP32_DeVICE_R_WSAP_RF_Channel		= (1<<2) // Bit #01 'WSAP_RF_Channel'		-> 
  ,ESP32_DeVICE_R_WSAP_Maximal_Connections	= (1<<3) // Bit #01 'WSAP_Maximal_Connections'	-> 
  ,ESP32_DeVICE_R_WSAP_Authentication_Method	= (1<<4) // Bit #01 'WSAP_Authentication_Method'-> 
  ,ESP32_DeVICE_R_WSAP_SSID			= (1<<5) // Bit #01 'WSAP_SSID'			->
  ,ESP32_DeVICE_R_WSAP_Beacon_Interval		= (1<<6) // Bit #01 'WSAP_Beacon_Interval'	-> 

  // Internals - TiSt-Sharing - Block 2 - func
  ,ESP32_DeVICE_R_WSAP_IP_Adress		= (1<<7) // Bit #02 'WSAP_IP_Adress'		-> 
  ,ESP32_DeVICE_R_WSAP_Netmask			= (1<<8) // Bit #02 'WSAP_Netmask'		-> 
  ,ESP32_DeVICE_R_WSAP_Gateway_Adress		= (1<<9) // Bit #02 'WSAP_Gateway_Adress'	-> 

  // Internals - TiSt-Sharing - Block 3 - func
  ,ESP32_DeVICE_R_WSAP_MAC_Adress		= (1<<10) // Bit #04 'WSAP_MAC_Adress'		-> 

  // Internals - TiSt-Sharing - Block 4 - func
  ,ESP32_DeVICE_R_WSAP_WiFi_Bandwidth		= (1<<11) // Bit #07 'WSAP_WiFi_Bandwidth'	-> 

  // Internals - TiSt-Sharing - Block 5 - func
  ,ESP32_DeVICE_R_WiFi_Country			= (1<<12) // Bit #07 'WiFi_Country'		-> 

  // Internals - TiSt-Sharing - Block 6 - func
  ,ESP32_DeVICE_R_Station_IP_Adress		= (1<<13) // Bit #06 'Station_IP_Adress'	-> 
  ,ESP32_DeVICE_R_Station_Netmask		= (1<<14) // Bit #06 'Station_Netmask'		-> 
  ,ESP32_DeVICE_R_Station_Gateway_Adress	= (1<<15) // Bit #06 'Station_Gateway_Adress'	-> 

  // Internals - TiSt-Sharing - Block 7 - func
  ,ESP32_DeVICE_R_Station_MAC_Adress		= (1<<16) // Bit #11 'Station_MAC_Adress'	-> 

  // Internals - TiSt-Sharing - Block 8 - func
  ,ESP32_DeVICE_R_Station_Auto_Connect		= (1<<16) // Bit #09 'Station_Auto_Connect'	-> 

  // Internals - TiSt-Sharing - Block 9 - func
  ,ESP32_DeVICE_R_Station_WiFi_Bandwidth	= (1<<16) // Bit #07 'Station_WiFi_Bandwidth'	-> 






  // Internals - TiSt-Sharing - Block 3 - func
  ,ESP32_DeVICE_R_WSAP_DHCPS_Lease_Start_IP	= (1<<2) // Bit #03 'WSAP_DHCPS_Lease_Start_IP'	-> 
  ,ESP32_DeVICE_R_WSAP_DHCPS_Lease_End_IP	= (1<<2) // Bit #03 'WSAP_DHCPS_Lease_End_IP'	-> 
  ,ESP32_DeVICE_R_WSAP_DHCPS			= (1<<2) // Bit #03 'WSAP_DHCPS'		-> 


  // Internals - TiSt-Sharing - Block 5 - func
  ,ESP32_DeVICE_R_WSAP				= (1<<4) // Bit #05 'WSAP'			-> 


  // Internals - TiSt-Sharing - Block 7 - func
  ,ESP32_DeVICE_R_Station			= (1<<6) // Bit #07 'Station'			-> 

  // Internals - TiSt-Sharing - Block 8 - func
  ,ESP32_DeVICE_R_Station_DHCP			= (1<<7) // Bit #08 'Station_DHCP'		-> 


  // Internals - TiSt-Sharing - Block 10 - func
  ,ESP32_DeVICE_R_Station_Physical_Mode		= (1<<9) // Bit #10 'Station_Physical_Mode'	-> 


  // Internals - TiSt-Sharing - Block 12 - func
  ,ESP32_DeVICE_R_TiSt_NTP_Server_Main		= (1<<11) // Bit #12 'TiSt_NTP_Server_Main'	-> 
  ,ESP32_DeVICE_R_TiSt_NTP_Server_Backup_1	= (1<<11) // Bit #12 'TiSt_NTP_Server_Backup_1'	-> 
  ,ESP32_DeVICE_R_TiSt_NTP_Server_Backup_2	= (1<<11) // Bit #12 'TiSt_NTP_Server_Backup_2'	-> 
  ,ESP32_DeVICE_R_TiSt_Time_Zone		= (1<<11) // Bit #12 'TiSt_Time_Zone'		-> 


  // Readings - TiSt-Sharing - Block 1 - func
  ,ESP32_DeVICE_R_Station_RSSI			= (1<<12) // Bit #13 'Station_RSSI'		-> 
  ,ESP32_DeVICE_R_TiSt_Time			= (1<<12) // Bit #13 'TiSt_Time'		-> 

  ,ESP32_DeVICE_R_Namex				= (1<<13) // Bit #02 'name' + caps + ufid -> 
};























/* For: SCDE_cgi
 * Implemented KEYS and assigned readings for this query - analyzed by by http_parser_parse_url()
 * Num should meet enum XX_QueryFields
 * Result is stored in struct SCDE_XX_parsedKVInput
 */
kvParseImplementedKeys_t ESP32_DeVICE_Set_ImplementedKeys[] =
  {//affected_reading | affected_reading			|  CMD
  // Block 1 - func
   { ESP32_DeVICE_R_Name					, "Name" }			// #00
  ,{ ESP32_DeVICE_R_WSAP_Password				, "WSAP_Password" }		// #01
  ,{ ESP32_DeVICE_R_WSAP_RF_Channel				, "WSAP_RF_Channel" }		// #02
  ,{ ESP32_DeVICE_R_WSAP_Maximal_Connections			, "WSAP_Maximal_Connections" }	// #03
  ,{ ESP32_DeVICE_R_WSAP_Authentication_Method			, "WSAP_Authentication_Method" }// #04
  ,{ ESP32_DeVICE_R_WSAP_SSID					, "WSAP_SSID" }			// #05
  ,{ ESP32_DeVICE_R_WSAP_Beacon_Interval			, "WSAP_Beacon_Interval" }	// #06

  // Block 2 - func
  ,{ ESP32_DeVICE_R_WSAP_IP_Adress				, "WSAP_IP_Adress" }		// #07
  ,{ ESP32_DeVICE_R_WSAP_Netmask				, "WSAP_Netmask" }		// #08
  ,{ ESP32_DeVICE_R_WSAP_Gateway_Adress				, "WSAP_Gateway_Adress" }	// #09

  // Block 3 - func
  ,{ ESP32_DeVICE_R_WSAP_MAC_Adress				, "WSAP_MAC_Adress" }		// #10

  // Block 4 - func
  ,{ ESP32_DeVICE_R_WSAP_WiFi_Bandwidth				, "WSAP_WiFi_Bandwidth" }	// #11

  // Block 5 - func
  ,{ ESP32_DeVICE_R_WiFi_Country				, "WiFi-Country" }		// #12

  // Block 6 - func
  ,{ ESP32_DeVICE_R_Station_IP_Adress				, "Station_IP_Adress" }		// #13
  ,{ ESP32_DeVICE_R_Station_Netmask				, "Station_Netmask" }		// #14
  ,{ ESP32_DeVICE_R_Station_Gateway_Adress			, "Station_Gateway_Adress" }	// #15

  // Block 7 - func
  ,{ ESP32_DeVICE_R_Station_MAC_Adress				, "Station_MAC_Adress" }	// #16

  // Block 8 - func
  ,{ ESP32_DeVICE_R_Station_Auto_Connect			, "Station_Auto_Connect" }	// #17

  // Block 9 - func
  ,{ ESP32_DeVICE_R_Station_WiFi_Bandwidth			, "Station_WiFi_Bandwidth" }	// #18









  // Block 3 - func
  ,{ ESP32_DeVICE_R_WSAP_DHCPS_Lease_Start_IP			, "WSAP_DHCPS_Lease_Start_IP" }	// #10
  ,{ ESP32_DeVICE_R_WSAP_DHCPS_Lease_End_IP			, "WSAP_DHCPS_Lease_End_IP" }	// #11

  // Block 4 - func
  ,{ ESP32_DeVICE_R_WSAP_DHCPS					, "WSAP_DHCPS" }		// #12

  // Block 6 - func
  ,{ ESP32_DeVICE_R_WSAP					, "WSAP" }			// #14

  // Block 8 - func
  ,{ ESP32_DeVICE_R_Station					, "Station" }			// #18

  // Block 9 - func
  ,{ ESP32_DeVICE_R_Station_DHCP				, "Station_DHCP" }		// #19

  // Block 11 - func
  ,{ ESP32_DeVICE_R_Station_Physical_Mode			, "Station_Physical_Mode" }	// #21

  // Block 12 - func
  ,{ ESP32_DeVICE_R_Station_RSSI				, "Station_Get_RSSI" }		// #22

   // Block 14 - func
  ,{ ESP32_DeVICE_R_TiSt_NTP_Server_Main			, "TiSt_NTP_Server_Main" }	// #24

  // Block 15 - func
  ,{ ESP32_DeVICE_R_TiSt_NTP_Server_Backup_1			, "TiSt_NTP_Server_Backup_1" }	// #25

  // Block 16 - func
  ,{ ESP32_DeVICE_R_TiSt_NTP_Server_Backup_2			, "TiSt_NTP_Server_Backup_2" }	// #26

  // Block 17 - func
  ,{ ESP32_DeVICE_R_TiSt_Time_Zone				, "TiSt_Time_Zone" }		// #27

  // Block 18 - func
  ,{ ESP32_DeVICE_R_TiSt_Time					, "TiSt_Get_Time" }		// #28

  // Block 18 - func
  ,{ 0								, "SoCHW_Restart" }		// #29

  // Block 18 - func
  ,{ 0								, "SoCHW_Factory_Reset_SCDE" }	// #30

  // Block 18 - func
  ,{ 0								, "SoCHW_Factory_Reset_CAPI" }	// #31

 // Block 18 - func
  ,{ 0								, "SoCHW_Factory_Reset" }	// #32

}; // !!! number of implemented keys should be equal with XX_IK_Number_of_keys, LIMIT IS 64 ELEMENTS !!!











/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_DeVICE_ActiveResourcesDataA_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART A (Resource-Data-Row)
 *  Data: WebIf_ActiveResourcesDataA_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// Content:   AllowedMethodBF          | AllowedDocMimeBF  | AllowedSchemeBF |free|CgiNo| EnaByBit | Url
const WebIf_ActiveResourcesDataA_t ESP32_DeVICE_ActiveResourcesDataA_forWebIf[] = {  //ICACHE_RODATA_ATTR = 

  // SOC Hardware Cfg
  { 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  0, 0b00000000, "/SoCHWCfg"}

  // WiFi Stations Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  1, 0b00000000, "/WiFi/StationCfg"}

  // WIFI Q-Connect PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  2, 0b00000000, "/WiFi/QConnect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000001000, 0b0000000000000001,  0,  3, 0b00000000, "/WiFi/WiFiScan"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  4, 0b00000000, "/WiFi/Connect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  5, 0b00000000, "/WiFi/Setmode"}

  // Service Access Point Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  6, 0b00000000, "/WiFi/ServAPCfg"}

  // TimeStamp Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  7, 0b00000000, "/TiStCfg"}

  // Firmware Update
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  8, 0b00000000, "/Firmware"}

  // Redirects
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0,  9, 0b00000000, "/WiFi"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0,  9, 0b00000000, "/WiFi/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 10, 0b00000000, "/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 10, 0b00000000, "/index"}	// .htm

  // Secret services
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 11, 0b00000000, "/32MBitFlash"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 12, 0b00000000, "/wfs"}

 ,{0,0,0,0,0,0,"*"}

  };



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_DeVICE_ActiveResourcesDataB_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART B (Procedure-Call-Data-Row) 
 *  Data: WebIf_ActiveResourcesDataB_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// CgiFucID=(No.<<16)+AllowedSchemeBF |      cgi            |     cgi_data
const WebIf_ActiveResourcesDataB_t ESP32_DeVICE_ActiveResourcesDataB_forWebIf[] =  {  //ICACHE_RODATA_ATTR =

  // ### SOC Hardware Cfg ###
  {( 0<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SoCHWCfg_tpl	}
 ,{( 0<<16) +	0b0000000000000100,	NULL,NULL}//	SoCHWCfg_cgi,		NULL		}
 ,{( 0<<16) +	0b0000000000001000,	NULL,NULL}//	SoCHWCfg_jso,		NULL		}

  // ### WiFi Station Cfg ###
 ,{( 1<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	StationCfg_tpl	}
 ,{( 1<<16) +	0b0000000000000100,	NULL,NULL}//	StationCfg_cgi,		NULL		}
 ,{( 1<<16) +	0b0000000000001000,	NULL,NULL}//	StationCfg_jso,		NULL		}

  // ### WIFI Q-Connect PAGES ###
 ,{( 2<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	WifiQConnect_tpl}
 ,{( 3<<16) +	0b0000000000001000,	NULL,NULL}//	WiFiScan_jso,		NULL		}
 ,{( 4<<16) +	0b0000000000000100,	NULL,NULL}//	WiFiConnect_cgi,	NULL		}
 ,{( 5<<16) +	0b0000000000000100,	NULL,NULL}//	WifiSetMode_cgi,	NULL		}	

  // ### Service Access Point Cfg ###
 ,{( 6<<16) +	0b0000000000000010,	WebIf_EspFSAdvFileTX,	ServAPCfg_tpl	}
 ,{( 6<<16) +	0b0000000000000100,	NULL,NULL}//	ServAPCfg_cgi,		NULL		}
 ,{( 6<<16) +	0b0000000000001000,	NULL,NULL}//	ServAPCfg_jso,		NULL		}

  // ### TimeStamp Cfg ###
 ,{( 7<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	TiStCfg_tpl	}
 ,{( 7<<16) +	0b0000000000000100,	NULL,NULL}//	TiStCfg_cgi,		NULL		}
 ,{( 7<<16) +	0b0000000000001000,	NULL,NULL}//	TiStCfg_jso,		NULL		}

  // ### Firmware Update ###
 ,{( 8<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	FirmwareUd_tpl	}
 ,{( 8<<16) +	0b0000000000000100,	NULL,NULL}//	FirmwareUd_cgi,		NULL		}
 ,{( 8<<16) +	0b0000000000001000,	NULL,NULL}//	FirmwareUd_jso,		NULL		}

  // ### Redirects ###
 ,{( 9<<16) +	0b0000000000000001,	cgiRedirect,	"/WiFi/QConnect.htm"		}
 ,{(10<<16) +	0b0000000000000001,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}
 ,{(10<<16) +	0b0000000000000010,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}

  // ### Secret services ###
 ,{(11<<16) +	0b0000000000100000,	ReadFullFlash_cgi,	NULL		}		//.bin
 ,{(12<<16) +	0b0000000000100000,	NULL,NULL}//	WriteFileSystem_cgi,	NULL		}		//.bin

  };



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_DeVICE_Module
 *  Desc: Data 'Provided By Module' for the ESP32Control module (functions + infos this module provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */
ProvidedByModule_t
ESP32_DeVICE_ProvidedByModule = { 	// A-Z order
  "ESP32_DeVICE"			// Type-Name of module -> on Linux libfilename.so !
  ,12					// size of Type-Name

  ,NULL					// Add
  ,ESP32_DeVICE_Attribute		// Attribute
  ,ESP32_DeVICE_Define			// Define
  ,NULL					// Delete
  ,NULL					// DirectRead
  ,NULL					// DirectWrite
  ,NULL					// Except
  ,NULL					// Get
  ,NULL					// IdleCb
  ,ESP32_DeVICE_Initialize		// Initialize
  ,NULL					// Notify
  ,NULL					// Parse
  ,NULL					// Read
  ,NULL					// Ready
  ,ESP32_DeVICE_Rename			// Rename
  ,ESP32_DeVICE_Set			// Set
  ,ESP32_DeVICE_Shutdown		// Shutdown
  ,ESP32_DeVICE_State			// State
  ,NULL					// Sub
  ,ESP32_DeVICE_Undefine		// Undefine
  ,NULL					// Write
  ,NULL					// FnProvided
  ,sizeof(ESP32_DeVICE_Definition_t)	// Modul specific Size (Common_Definition_t + X)
};



/* -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Attribute
 *  Desc: Informs an Definition of this Type for attribute activities (set/del)
 *  Info: 'attrCmd' is the command text for the activity: set, del
 *        'attrName' is the attribute name text
 *        'attrVal' is the attribute value text
 *        THE attrVal CAN BE MANIPULATED / CORRECTED
 *  Para: Common_Definition_t* Common_Definition -> the belonging definition for the attribute-activitie
 *	  const uint8_t *attrCmdText -> ptr to attribute-command text "attrCmd"
 *	  const size_t attrCmdTextLen -> length of attribute-command text "attrCmd"
 *	  const uint8_t *attrNameText -> ptr to attribute-name text "attrName"
 *	  const size_t attrNameTextLen -> length of attribute-name text "attrName"
 *	  uint8_t **attrValTextPtr -> ptr to ptr holding attribute-value text "attrValue" - manipulation possible
 *	  size_t *attrValLenTextPtr -> ptr to length of attribute-value text "attrValue" - manipulation possible
 *  Rets: strTextMultiple_t* -> veto = error-text-string in allocated mem, or NULL = OK - no veto
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_DeVICE_Attribute(Common_Definition_t* Common_Definition
		,const uint8_t *attrCmdText
		,const size_t attrCmdTextLen
		,const uint8_t *attrNameText
		,const size_t attrNameTextLen
		,uint8_t **attrValTextPtr
		,size_t *attrValTextLenPtr)
{

  // preparations?
  uint8_t *attrValText = *attrValTextPtr;
  size_t attrValTextLen = *attrValTextLenPtr;

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
		  (ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,5
	,"Executing AttributeFn of Module '%.*s' for Definition '%.*s' with attrCmd '%.*s' attrName '%.*s' attrVal '%.*s'."
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName
	,ESP32_DeVICE_Definition->common.nameLen
	,ESP32_DeVICE_Definition->common.name
	,attrCmdTextLen
	,attrCmdText
  	,attrNameTextLen
	,attrNameText
	,(int) attrValTextLen
	,attrValText);
  #endif

// ------------------------------------------------------------------------------------------------

/*
  // set start of possible def-Name
  const uint8_t *defName = args;

  // set start of possible attr-Name
  const uint8_t *attrName = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*attrName != ' ') ) {i++;attrName++;}

  // length of def-Name
  size_t defNameLen = i;

  // seek to start position of attr-Name '\32'
  while( (i < argsLen) && (*attrName == ' ') ) {i++;attrName++;}

  // set start of possible attr-Val
  const uint8_t *attrVal = attrName;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*attrVal != ' ') ) {i++,j++;attrVal++;}

  // length of attr-Name
  size_t attrNameLen = j;

  // start position of attr-Val
  while( (i < argsLen) && (*attrVal == ' ') ) {i++;attrVal++;}

  // length of attr-Val
  size_t attrValLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (defNameLen == 0) || (attrNameLen == 0) )

		{

		// response with error text
		asprintf(&retMsg
				,"Could not interpret command ''! Usage: Attr <defname> <attrname> [<attrval>]");

		return retMsg;

	//	}

// -------------------------------------------------------------------------------------------------
*/

  return retMsg;
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Define
 *  Desc: Finalizes the definition of a new "device" of 'ESP32Control' type.
 *        Contains devicespecific init code.
 *  Info: 
 *  Para: Common_Definition_t *Common_Definition -> prefilled ESP32Control Definition
 *        char *Definition -> the last part of the CommandDefine arg* 
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_DeVICE_Define(Common_Definition_t *Common_Definition)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
		  (ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,5
	,"Executing DefineFn of Module '%.*s' to continue creation of Definition '%.*s' "
	 "with args '%.*s'."
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName
	,ESP32_DeVICE_Definition->common.nameLen
	,ESP32_DeVICE_Definition->common.name
	,ESP32_DeVICE_Definition->common.definitionLen
	,ESP32_DeVICE_Definition->common.definition);
  #endif

// ------------------------------------------------------------------------------------------------

  // new conversation
  uint8_t *defArgsText = Common_Definition->definition;
  size_t defArgsTextLen = Common_Definition->definitionLen;

  // Check for args. This type requires args...
  if (!defArgsTextLen) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! Expected Args!");

	return retMsg;
  }

// -------------------------------------------------------------------------------------------


  // s1.1: The main task calls tcpip_adapter_init() to create an LwIP core task and 
  //       initialize LwIP-related work.
  tcpip_adapter_init();


  // s1.2: The main task calls esp_event_loop_init() to create a system Event task and 
  //       initialize an application event’s callback function. In the scenario above,
  //       the application event’s callback function does nothing but relaying the event
  //       to the application task.
  wifi_event_group = xEventGroupCreate();

  // install wifi event handler
  ESP_ERROR_CHECK(esp_event_loop_init(ESP32_DeVICE_WiFiEventHandler, NULL) );


  // s1.3: The main task calls esp_wifi_init() to create the Wi-Fi driver task
  // and initialize the Wi-Fi driver.
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

   ESP_ERROR_CHECK( esp_wifi_init(&cfg) );






  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

  wifi_config_t sta_config = {
        .sta = {
            .ssid = "SF4 AP",
            .password = "pcmcia91",
            .bssid_set = false
        }
  };

  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );


  // s3.1: Call esp_wifi_start to start the Wi-Fi driver.
  ESP_ERROR_CHECK( esp_wifi_start() );

  ESP_ERROR_CHECK( esp_wifi_connect() );






 // wait till wifi is connected
  xEventGroupWaitBits(wifi_event_group
		  , CONNECTED_BIT
		  ,false
		  ,true
		  ,portMAX_DELAY);


  // sync time via SNTP
  Obtain_Time();




// -------------------------------------------------------------------------------------------

  // store FD to Definition. Will than be processed in global loop ... -> THIS MODULE USES NO FD
  ESP32_DeVICE_Definition->common.fd = -1;

// -------------------------------------------------------------------------------------------

  // init WebIf_Provided offset
//  ESP32_DeVICE_Definition->common.link =
//	&ESP32_DeVICE_Definition->WebIf_Provided;

  // check for loaded Module 'WebIf' -> get provided Fn
  ESP32_DeVICE_Definition->WebIf_Provided.WebIf_FnProvided =
	NULL;//(WebIf_FnProvided_t *) SCDEFn_at_ESP32_DeVICE_M->GetFnProvidedByModule("WebIf");

 // Providing data for WebIf? Initialise data provided for WebIf
  if (ESP32_DeVICE_Definition->WebIf_Provided.WebIf_FnProvided) {

	ESP32_DeVICE_Definition->WebIf_Provided.ActiveResourcesDataA =
		(WebIf_ActiveResourcesDataA_t *) &ESP32_DeVICE_ActiveResourcesDataA_forWebIf;

	ESP32_DeVICE_Definition->WebIf_Provided.ActiveResourcesDataB =
		(WebIf_ActiveResourcesDataB_t *) &ESP32_DeVICE_ActiveResourcesDataB_forWebIf;

	}

  else	{

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,1
		,"Could not enable WebIf support for '%.*s'. "
		 "Type '%.*s' detects Type 'WebIf' is NOT loaded!"
		,ESP32_DeVICE_Definition->common.nameLen
		,ESP32_DeVICE_Definition->common.name
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName);
	}

// -------------------------------------------------------------------------------------------

  // Parse define-args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t *parsedKVInput = 
	SCDEFn_at_ESP32_DeVICE_M->ParseKVInputArgsFn(ESP32_DeVICE_Set_NUMBER_OF_IK	// Num Implementated KEYs MAX
	,ESP32_DeVICE_Set_ImplementedKeys				// Implementated Keys
	,defArgsText							// our args text
	,defArgsTextLen);						// our args text len

  // parsing may report an problem. args contain: unknown keys, double keys, ...?
  if (!parsedKVInput) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! Args '%.*s' not taken! Check the KEYs!"
		,defArgsTextLen
		,defArgsText);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // set required Keys -> Keys that should be there in this request
  parsedKVInput->requiredKVBF = 0;/*( (1 << ESP32_SwITCH_SET_GPIO)
			        | (1 << ESP32_SwITCH_SET_BLOCK)
			        | (1 << ESP32_SwITCH_SET_CHANNEL)
			        | (1 << ESP32_SwITCH_SET_TIMER)
			        | (1 << ESP32_SwITCH_SET_DUTY)
			        | (1 << ESP32_SwITCH_SET_HPOINT)
			        | (1 << ESP32_SwITCH_SET_SIG_OUT_EN)
			        | (1 << ESP32_SwITCH_SET_IDLE_LV)
			        | (1 << ESP32_SwITCH_SET_RESOLUTION)
			        | (1 << ESP32_SwITCH_SET_TICK_SOURCE)
			        | (1 << ESP32_SwITCH_SET_FREQ_HZ) );*/

  // set forbidden Keys -> Keys that are not allowed in this request
  parsedKVInput->forbiddenKVBF = 0;

  // process the set-args (key=value@) protocol
  if (ESP32_DeVICE_ProcessKVInputArgs(ESP32_DeVICE_Definition
	,parsedKVInput				// KVInput parsed
	,defArgsText				// our args text
	,defArgsTextLen) ) {			// our args text len

 	// Processing reports an problem. Args not taken. Response with error text.

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Processing Error! Args '%.*s' not taken! Check the VALUEs!"
		,defArgsTextLen
		,defArgsText);

	// free allocated memory for query result key-field
	free(parsedKVInput);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // set affected readings
 // ESP32_DeVICE_SetAffectedReadings(ESP32_DeVICE_Definition
//	,parsedKVInput->affectedReadingsBF);

// ------------------------------------------------------------------------------------------------

  // free allocated memory for query result key-field
  free(parsedKVInput);

  return retMsg;
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Initialize
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
ESP32_DeVICE_Initialize(SCDERoot_t* SCDERootptr)
{

  // make data root locally available
  SCDERoot_at_ESP32_DeVICE_M = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn_at_ESP32_DeVICE_M = SCDERootptr->SCDEFn;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 3
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(ESP32_DeVICE_ProvidedByModule.typeName
	,ESP32_DeVICE_ProvidedByModule.typeNameLen
	,3
	,"Executing InitializeFn of Module '%.*s' to make it useable."
	,ESP32_DeVICE_ProvidedByModule.typeNameLen
	,ESP32_DeVICE_ProvidedByModule.typeName);
  #endif

// -------------------------------------------------------------------------------------------------

  return 0;
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Rename
 *  Desc: FN is called when a Definition of this Module was renamed. Module can do further things here. 
 *  Info: An FN for Rename is optional!
 *  Para: ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> ESP32_DeVICE Definition that was renamed
 *        uint8_t *newName -> ptr to the new name (do we need it???)
 *        size_t newNameLen -> length of the new name
 *        uint8_t *oldName -> ptr to old name
 *        size_t oldNameLen -> length of the old name
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_DeVICE_Rename(Common_Definition_t *Common_Definition
	,uint8_t *newName
	,size_t newNameLen
	,uint8_t *oldName
	,size_t oldNameLen)
{
	
  // for Fn response msg
  strTextMultiple_t *multipleRetMsg = NULL;
	
  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
		(ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,5
		,"Executing RenameFn of Module '%.*s'. "
		 "An definition is renamed from old name '%.*s' to new name '%.*s'."
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName
		,oldName
		,oldNameLen
		,newName
		,newNameLen);
  #endif
	
// -------------------------------------------------------------------------------------------------

  return multipleRetMsg;
}
	

























/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char * ICACHE_FLASH_ATTR
ESP32_DeVICE_strnstr(s, find, slen)
	const char *s;
	const char *find;
	size_t slen;
{
	char c, sc;
	size_t len;

	if ((c = *find++) != '\0') {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == '\0' || slen-- < 1)
					return (NULL);
			} while (sc != c);
			if (len > slen)
				return (NULL);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}














/*"WSAP_Password ";
	$list .= "WSAP_RF_Channel:uzsuSelectRadio,1,2,3,4,5,6,7,8,9,10,11,12,13 ";

"WSAP_Beacon_Interval:slider,100,10,60000 ";

uint8Slider,a,b,c
uint16Slider,a,b,c
uint32Slider,a,b,c
int8Slider,a,b,c
int16Slider,a,b,c
int32Slider,a,b,c


*/








	







/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Set
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set ESP32_DeVICE_Definition.common.Name setArgs'
 *  Para: ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_DeVICE_Set(Common_Definition_t* Common_Definition
	,uint8_t *setArgsText
	,size_t setArgsTextLen)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
	(ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,5
		,"Executing SetFn of Module '%.*s' for Definition '%.*s' with attached args '%.*s'."
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName
		,ESP32_DeVICE_Definition->common.nameLen
		,ESP32_DeVICE_Definition->common.name
		,setArgsTextLen
		,setArgsText);
  #endif

// ------------------------------------------------------------------------------------------------

//args ? here

// ------------------------------------------------------------------------------------------------

  // Parse set-args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t *parsedKVInput = 
	SCDEFn_at_ESP32_DeVICE_M->ParseKVInputArgsFn(ESP32_DeVICE_Set_NUMBER_OF_IK	// Num Implementated KEYs MAX
	,ESP32_DeVICE_Set_ImplementedKeys				// Implementated Keys
	,setArgsText
	,setArgsTextLen);

  // parsing may report an problem. args contain: unknown keys, double keys, ...?
  if (!parsedKVInput) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! Args '%.*s' not taken! Check the KEYs!"
		,setArgsTextLen
		,setArgsText);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // set required Keys -> Keys that should be there in this request
  parsedKVInput->requiredKVBF = 0;/*( (1 << ESP32_SwITCH_SET_GPIO)
			        | (1 << ESP32_SwITCH_SET_BLOCK)
			        | (1 << ESP32_SwITCH_SET_CHANNEL)
			        | (1 << ESP32_SwITCH_SET_TIMER)
			        | (1 << ESP32_SwITCH_SET_DUTY)
			        | (1 << ESP32_SwITCH_SET_HPOINT)
			        | (1 << ESP32_SwITCH_SET_SIG_OUT_EN)
			        | (1 << ESP32_SwITCH_SET_IDLE_LV)
			        | (1 << ESP32_SwITCH_SET_RESOLUTION)
			        | (1 << ESP32_SwITCH_SET_TICK_SOURCE)
			        | (1 << ESP32_SwITCH_SET_FREQ_HZ) );*/

  // set forbidden Keys -> Keys that are not allowed in this request
  parsedKVInput->forbiddenKVBF = 0;

  // process the set-args (key=value@) protocol
  if (ESP32_DeVICE_ProcessKVInputArgs(ESP32_DeVICE_Definition
	,parsedKVInput
	,setArgsText
	,setArgsTextLen) ) {

 	// Processing reports an problem. Args not taken. Response with error text.

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Processing Error! Args '%.*s' not taken! Check the VALUEs!"
		,setArgsTextLen
		,setArgsText);

	// free allocated memory for query result key-field
	free(parsedKVInput);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // set affected readings
 // ESP32_DeVICE_SetAffectedReadings(ESP32_DeVICE_Definition
//	,parsedKVInput->affectedReadingsBF);

// ------------------------------------------------------------------------------------------------

  // free allocated memory for query result key-field
  free(parsedKVInput);

  return retMsg;
}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Shutdown
 *  Desc: called after 
 *  Info: Invoked by cmd-line 'Undefine ESP32_DeVICE_Definition.common.Name'
 *  Para: ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_DeVICE_Shutdown(Common_Definition_t* Common_Definition)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
	(ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,5
	,"Executing ShutdownFn of Module '%.*s' for Definition '%.*s'."
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName
		,ESP32_DeVICE_Definition->common.nameLen
	,ESP32_DeVICE_Definition->common.name);
  #endif

// -------------------------------------------------------------------------------------------------

  // 8. Wi-Fi Deinit Phase

  // s8.1: Call esp_wifi_disconnect() to disconnect the Wi-Fi connectivity.
  esp_wifi_disconnect();

  // s8.2: Call esp_wifi_stop() to stop the Wi-Fi driver.
  esp_wifi_stop();

  // s8.3: Call esp_wifi_deinit() to unload the Wi-Fi driver.
  esp_wifi_deinit();

  return retMsg;
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_State
 *  Desc: FN is called when a Definition of this Module gets, and should set an state update. 
 *        Normally called from setstate cmd when executing state (=readings?) recovery from save .cfg
 *  Info: An FN for State in an Module is optional!
 *  Para: const Common_Definition_t *ESP32_DeVICE_Definition -> Definition that should execute an status update
 *        const time_t stateTiSt -> time stamp for status update / reading value
 *        const xString_t stateNameString -> state Name for status update / reading value
 *        const xString_t stateValueString -> state Value for the status update / reading value
 *        const xString_t stateMimeString -> state Mime for the status update / reading value
 *  Rets: xMultipleStringSLTQE_t -> singly linked tail queue element which stores one xString_t string
 *                                  as response text in allocated memory. NULL=no response text
 * -------------------------------------------------------------------------------------------------
 */
xMultipleStringSLTQE_t*
ESP32_DeVICE_State(Common_Definition_t *Common_Definition
	,const time_t stateTiSt
	,const xString_t stateNameString
	,const xString_t stateValueString
	,const xString_t stateMimeString)
{
  // Fn return message. NULL = no return message
  xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE = NULL;
	
  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
		(ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  // prepare TiSt-string for LogFn
  strText_t timeString =
  	SCDEFn_at_ESP32_DeVICE_M->FmtDateTimeFn(stateTiSt);

  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,5
	,"Executing StateFn of Module '%.*s' for Definition '%.*s'. "
         "Should set State for '%.*s' with Value '%.*s' and Mime '%.*s'. TimeStamp '%.*s'."
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
	,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName
	,Common_Definition->nameLen
	,Common_Definition->name
	,stateNameString.length
	,stateNameString.characters
	,stateValueString.length
	,stateValueString.characters
	,stateMimeString.length
	,stateMimeString.characters
	,timeString.strTextLen
	,timeString.strText);

  // free TiSt-string from LogFn
  free(timeString.strText);
  #endif

/*
// ------------------------------------------------------------------------------------------------

  // build an temp KEY=VALUE string
  xString_t argsString;

  // check if there is a KEY
  if (!stateNameString.length) {

	// alloc mem for the singly linked tail queue element that stores the Return Message
	retMsgMultipleStringSLTQE = 
		malloc(sizeof(xMultipleStringSLTQE_t));

	// fill Return Message with error text
	retMsgMultipleStringSLTQE->string.length = 
		asprintf(&retMsgMultipleStringSLTQE->string.characters
			,"Error in StateFn! Fn called without State Name");

	return retMsgMultipleStringSLTQE;
  }

  argsString.length = 
	asprintf(&argsString.characters
		,"%.*s=%.*s"
		,stateNameString.length
		,stateNameString.characters
		,stateValueString.length
		,stateValueString.characters);

// ------------------------------------------------------------------------------------------------

  // Parse State-Args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t *parsedKVInput = 
	SCDEFn_at_ESP32_DeVICE_M->ParseKVInputArgsFn(ESP32_DeVICE_Set_NUMBER_OF_IK	// Num Implementated KEYs MAX
		,ESP32_DeVICE_Set_ImplementedKeys			// Implementated Keys
		,argsString.characters
		,argsString.length);

  // NULL? Parsing reports an problem. args contain: unknown keys, double keys, ...?
  if (!parsedKVInput) {

	// alloc mem for the singly linked tail queue element that stores the Return Message
	retMsgMultipleStringSLTQE = 
		malloc(sizeof(xMultipleStringSLTQE_t));

	// fill Return Message with error text
	retMsgMultipleStringSLTQE->string.length = 
		asprintf(&retMsgMultipleStringSLTQE->string.characters
			,"Parsing Error! Args '%.*s' not taken! Check the KEYs!"
			,argsString.length
			,argsString.characters);

	// free the temp KEY=VALUE string
	if (argsString.characters) free (argsString.characters);

	return retMsgMultipleStringSLTQE;
  }

// -------------------------------------------------------------------------------------------------

  // free allocated memory for query result key-field
  free(parsedKVInput);

  // free the temp KEY=VALUE string
  if (argsString.characters) free (argsString.characters);
*/
  return retMsgMultipleStringSLTQE;
}



/** TEXT IST FALSCH!!!!!!!!!!
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Undefine
 *  Desc: Removes the define of an "device" of 'WebIF' type. Contains devicespecific init code.
 *  Info: Invoked by cmd-line 'Undefine ESP32_DeVICE_Definition.common.Name'
 *  Para: ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_DeVICE_Undefine(Common_Definition_t* Common_Definition)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
	(ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,5
		,"Executing UndefineFn of Module '%.*s' for Definition '%.*s'."
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeNameLen
		,ESP32_DeVICE_Definition->common.module->ProvidedByModule->typeName
		,ESP32_DeVICE_Definition->common.nameLen
		,ESP32_DeVICE_Definition->common.name);
  #endif

// -------------------------------------------------------------------------------------------------

  // response with error text

  // alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"ESP32_DeVICE_Set, Name:%.*s"
	,ESP32_DeVICE_Definition->common.nameLen
	,ESP32_DeVICE_Definition->common.name);

  return retMsg;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_WiFiEventHandler
 *  Desc: Wifi event handler
 *  Para: System_Event_t *event -> struct with incoming event information for wifi
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
static esp_err_t
ESP32_DeVICE_WiFiEventHandler(void *ctx, system_event_t *event)
{
  switch (event->event_id)

	{

//--------------------------------------------------------------------------------------------------

	// ESP32 WiFi ready
	case SYSTEM_EVENT_WIFI_READY:

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 finish scanning AP
	case SYSTEM_EVENT_SCAN_DONE:

	// struct contains:
  	// uint32_t status;          /**< status of scanning APs */
  	// uint8_t  number;
  	// uint8_t  scan_id;
	// system_event_sta_scan_done_t;

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 station start
	case SYSTEM_EVENT_STA_START:

	// station started - connect to station
	esp_wifi_connect();

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 station stop
	case SYSTEM_EVENT_STA_STOP:

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 station connected to AP
	case SYSTEM_EVENT_STA_CONNECTED:

	// struct contains:
   	// uint8_t ssid[32];         /**< SSID of connected AP */
   	// uint8_t ssid_len;         /**< SSID length of connected AP */
   	// uint8_t bssid[6];         /**< BSSID of connected AP*/
   	// uint8_t channel;          /**< channel of connected AP*/
   	// wifi_auth_mode_t authmode;
	// system_event_sta_connected_t;

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station connected to an access-point. SSID:%s, channel:%d"
		,event->event_info.connected.ssid
		,event->event_info.connected.channel);

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 station disconnected from AP
	case SYSTEM_EVENT_STA_DISCONNECTED:

	// struct contains:
   	// uint8_t ssid[32];         /**< SSID of disconnected AP */
   	// uint8_t ssid_len;         /**< SSID length of disconnected AP */
   	// uint8_t bssid[6];         /**< BSSID of disconnected AP */
   	// uint8_t reason;           /**< reason of disconnection */
	// system_event_sta_disconnected_t;

	// This is a workaround as ESP32 WiFi libs don't currently auto-reassociate
	esp_wifi_connect();
	xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station disconnected from an access-point. SSID:%s, reason:%d"
		,event->event_info.disconnected.ssid
		,event->event_info.disconnected.reason);

	break;

//--------------------------------------------------------------------------------------------------

	// the auth mode of AP connected by ESP32 station changed
	case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:

	// struct contains:
   	// wifi_auth_mode_t old_mode;         /**< the old auth mode of AP */
    	// wifi_auth_mode_t new_mode;         /**< the new auth mode of AP */
	// system_event_sta_authmode_change_t;

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station - Authmode changed from %d to %d",
		event->event_info.auth_change.old_mode,
		event->event_info.auth_change.new_mode);

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 station got IP from connected AP
	case SYSTEM_EVENT_STA_GOT_IP:

	// struct contains:
    	// tcpip_adapter_ip_info_t ip_info;
	// system_event_sta_got_ip_t;

	// ??
	xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station got IP from access point:"IPSTR ", mask:"IPSTR", gw:"IPSTR,
		IP2STR(&event->event_info.got_ip.ip_info.ip),
		IP2STR(&event->event_info.got_ip.ip_info.netmask),
		IP2STR(&event->event_info.got_ip.ip_info.gw));

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 soft-AP start
	case SYSTEM_EVENT_AP_START:

	break;

//--------------------------------------------------------------------------------------------------

	// ESP32 soft-AP stop
	case SYSTEM_EVENT_AP_STOP:

	break;

//--------------------------------------------------------------------------------------------------

	// a station connected to ESP32 soft-AP
	case SYSTEM_EVENT_AP_STACONNECTED:

	// struct contains:
 	// uint8_t mac[6];           /**< MAC address of the station connected to ESP32 soft-AP */
 	// uint8_t aid;              /**< the aid that ESP32 soft-AP gives to the station connected to  */
	// system_event_ap_staconnected_t;
	
	SCDEFn_at_ESP32_DeVICE_M->Log3Fn( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"I:WSAP Device conn.: "MACSTR ", AID = %d",
		MAC2STR(event->event_info.sta_connected.mac),
		event->event_info.sta_connected.aid);

	break;

//--------------------------------------------------------------------------------------------------

	// a station disconnected from ESP32 soft-AP
	case SYSTEM_EVENT_AP_STADISCONNECTED:

	// struct contains:
   	// uint8_t mac[6];           /**< MAC address of the station disconnects to ESP32 soft-AP */
   	// uint8_t aid;              /**< the aid that ESP32 soft-AP gave to the station disconnects to  */
	// system_event_ap_stadisconnected_t;

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"WSAP - Device disconnected: "MACSTR ", AID = %d",
		MAC2STR(event->event_info.sta_disconnected.mac),
		event->event_info.sta_disconnected.aid);

	break;

//--------------------------------------------------------------------------------------------------

	// Receive probe request packet in soft-AP interface
	case SYSTEM_EVENT_AP_PROBEREQRECVED:

	// struct contains:
 	// int rssi;                 /**< Received probe request signal strength */
 	// uint8_t mac[6];           /**< MAC address of the station which send probe request */
	// system_event_ap_probe_req_rx_t;

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"WSAP - ProbeReq.Rcved:%d"
		,event->event_info.ap_probereqrecved.rssi);

	break;

//--------------------------------------------------------------------------------------------------

	default:
	break;

//--------------------------------------------------------------------------------------------------

	}

  return ESP_OK;
}



















#include "apps/sntp/sntp.h"

static void 
Initialize_SNTP(void)
{
  printf ("Initializing SNTP");

  sntp_setoperatingmode(SNTP_OPMODE_POLL);

  sntp_setservername(0, "pool.ntp.org");

  sntp_init();
}





static void
Obtain_Time(void)
{
  Initialize_SNTP();

  // wait for time to be set
  time_t now = 0;

  struct tm timeinfo = { 0 };

  int retry = 0;

  const int retry_count = 10;

  while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {

	printf ("Waiting for system time to be set... (%d/%d)",
		retry,
		retry_count);

	vTaskDelay(2000 / portTICK_PERIOD_MS);

	time(&now);

	localtime_r(&now, &timeinfo);
  }
}



/*

  time_t now;
      struct tm timeinfo;
      time(&now);
      localtime_r(&now, &timeinfo);

      char strftime_buf[64];

      // Set timezone to Eastern Standard Time and print local time
         setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
         tzset();
         localtime_r(&now, &timeinfo);
         strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
         LOGD("New York is: %s"
        	,strftime_buf);
*/

 




















/*
 * ------------------  helpers provided for module operation starting here ------------------------
 */



// eine für Set und eine für define ????

/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_ProcessKVInputArgs
 *  Desc: Processes the KEY=VALUE input from Args. Information is pre-parsed to *parsedKVInput
 *  Info: .requiredKV -> Keys that should be there / .forbiddenKV -> Keys that are not allowed
 *  Para: ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> Definition that should get the input
 *        parsedKVInputArgs_t *parsedKVInput -> ptr to allocated mem filled with results from parse
 *        uint8_t *argsText -> the ArgsText
 *        size_t argsTextLen -> length of the ArgsText
 *  Rets: Bool -> TRUE = input parsed successful, FALSE = input parsed, but NOT successful, CANCELED
 * -------------------------------------------------------------------------------------------------
 */
bool
ESP32_DeVICE_ProcessKVInputArgs(ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition
	,parsedKVInputArgs_t *parsedKVInput
	,uint8_t *argsText
	,size_t argsTextLen)
{



printf ("The Args: '%.*s'.",argsTextLen,argsText);



// -------------------------------------------------------------------------------------------------
// 1. Step: Create backup structures, if required mirror current values from SDK
// -------------------------------------------------------------------------------------------------

  // block #01 mirror 'wifi_config_t' (WSAP-Configuration)
  wifi_config_t ap_wifi_config;
  if (parsedKVInput->keysFoundBF & 
				( (1 << ESP32_DeVICE_Set_IK_Name)
				| (1 << ESP32_DeVICE_Set_IK_WSAP_Password)
				| (1 << ESP32_DeVICE_Set_IK_WSAP_RF_Channel)
				| (1 << ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections)
				| (1 << ESP32_DeVICE_Set_IK_WSAP_Authentication_Method)
				| (1 << ESP32_DeVICE_Set_IK_WSAP_SSID)
				| (1 << ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval) ) ) {

	esp_wifi_get_config (WIFI_IF_AP, &ap_wifi_config);
  }

// -------------------------------------------------------------------------------------------------






  // Processed block #02 mirror 'tcpip_adapter_ip_info_t' (WSAP-IP-Settings)
  tcpip_adapter_ip_info_t ap_ip_info;
  if (parsedKVInput->keysFoundBF &  ( ESP32_DeVICE_R_WSAP_IP_Adress
				    | ESP32_DeVICE_R_WSAP_Netmask
				    | ESP32_DeVICE_R_WSAP_Gateway_Adress) ) {

	tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip_info);
  }

// -------------------------------------------------------------------------------------------------

  // block #3 get current Service AP MAC Adress
  uint8_t ap_mac_addr[8];
  esp_wifi_get_mac(WIFI_IF_AP,(uint8_t *) &ap_mac_addr);

  // block #4 get the bandwith of the Service Access Point
  wifi_bandwidth_t ap_wifi_bandwidth;
  esp_wifi_get_bandwidth(WIFI_IF_AP, &ap_wifi_bandwidth);

  // block #5 get the WiFi Country
  wifi_country_t country;
  esp_wifi_get_country(&country);

  // block #6 get current Station IP Settings
  tcpip_adapter_ip_info_t sta_ip_info;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip_info);

  // block #7 get current Station MAC Adress
  uint8_t sta_mac_addr[8];
  esp_wifi_get_mac(WIFI_IF_STA, (uint8_t *) &sta_mac_addr);

  // block #8 get current Station Auto-Connect-Status
  bool sta_autoconnect_status;
  esp_wifi_get_auto_connect(&sta_autoconnect_status);

 // block #9 mirror 'wifi_bandwidth_t' (WIFI STA Configuration)
  wifi_bandwidth_t wifiBandwidthSTA;
  if (parsedKVInput->keysFoundBF & (1 << ESP32_DeVICE_R_WSAP_WiFi_Bandwidth) ) {

	esp_wifi_get_bandwidth(WIFI_IF_STA, &wifiBandwidthSTA);
  }










//#3
  // block #6 get current Station Settings
  wifi_config_t sta_wifi_config;
  esp_wifi_get_config (WIFI_IF_STA, &sta_wifi_config);

  // block #3 get current Service AP DHCP-Server-Status
  tcpip_adapter_dhcp_status_t ap_dhcp_status;
  tcpip_adapter_dhcps_get_status(TCPIP_ADAPTER_IF_AP,&ap_dhcp_status); 
//esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);
//esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);

  // block #8 get current Station DHCP-Client-Status
  tcpip_adapter_dhcp_status_t sta_dhcp_status;
  tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA,&sta_dhcp_status); 
//esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);
//esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);



/*

  // block #13 mirror 'wifi_mode_t' (WIFI WSAP+STA Configuration)
  wifi_mode_t wifi_mode;
  if (parsedKVInput->keysFoundBF & (1 << ESP32_DeVICE_Set_IK_?) ) {

	esp_wifi_get_mode(&wifi_mode);
  }

  // block #xx mirror 'wifi_ps_type_t' (WIFI WSAP+STA Configuration)
  wifi_ps_type_t wifi_ps_type;
  if (parsedKVInput->keysFoundBF & (1 << ESP32_DeVICE_Set_IK_?) ) {

	esp_wifi_set_ps(&wifi_ps_type);
  }

  // block #xx mirror 'protocol_bitmap' (WIFI WSAP Configuration)
  uint8_t protocolBitmapAP;
  if (parsedKVInput->keysFoundBF & (1 << ESP32_DeVICE_Set_IK_?) ) {

	esp_wifi_get_protocol(TCPIP_ADAPTER_IF_AP, &protocolBitmapAP);
  }

  // block #xx mirror 'protocol_bitmap' (WIFI STA Configuration)
  uint8_t protocolBitmapSTA;
  if (parsedKVInput->keysFoundBF & (1 << ESP32_DeVICE_Set_IK_?) ) {

	esp_wifi_get_protocol(TCPIP_ADAPTER_IF_STA, &protocolBitmapSTA);
  }

 // block #xx mirror 'wifi_bandwidth_t' (WIFI WSAP Configuration)
  wifi_bandwidth_t wifiBandwidthSTA;
  if (parsedKVInput->keysFoundBF & (1 << ESP32_DeVICE_R_STA_WiFi_Bandwidth) ) {

	esp_wifi_get_bandwidth(WIFI_IF_STA, &wifiBandwidthSTA);
  }
*/






  // block #14 get current Wifi operating mode (Station + Service AP)
  wifi_ps_type_t wifi_ps_type;
  esp_wifi_get_ps(&wifi_ps_type);


/*
  // block #3 get current Service AP DHCP-Settings
//spz    struct dhcps_lease dhcps_lease;
//spz   wifi_softap_get_dhcps_lease(&dhcps_lease);

//spz  // block #11 get current Station Physical Mode
//spz  int WifiStationPhyMode = wifi_get_phy_mode();

*/


/*
   // Block 1 - func
    ESP32_DeVICE_Set_IK_Name		= 0		// Bit #00 'Name'			-> 
  , ESP32_DeVICE_Set_IK_WSAP_Password			// Bit #01 'WSAP_Password'		-> 
  , ESP32_DeVICE_Set_IK_WSAP_RF_Channel		        // Bit #02 'WSAP_RF_Channel' 		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections	// Bit #03 'WSAP_Maximal_Connections'  	-> 
  , ESP32_DeVICE_Set_IK_WSAP_Authentication_Method	// Bit #04 'WSAP_Authentication_Method' -> 
  , ESP32_DeVICE_Set_IK_WSAP_SSID			// Bit #05 'WSAP_SSID'  		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval		// Bit #06 'WSAP_Beacon_Interval'  	-> 
*/






printf ("parsedKVInput->keysFoundBF:%lld", parsedKVInput->keysFoundBF);

// -------------------------------------------------------------------------------------------------
// 2. Step: Check for found KEYs and if the given values match the requirements
// -------------------------------------------------------------------------------------------------

  // remember the readings affected by the parsing process
  uint32_t affectedReadings = 0;

// -------------------------------------------------------------------------------------------------
// Name=[a-zA-Z0-9_.] -> Setze Device-Name = WSAP-Name (char) (0-sizeof (ap_wifi_config.ap.ssid)-1)
// Name=[a-zA-Z0-9_.] -> Set Device-Name = WSAP-Name (char) (0-sizeof (ap_wifi_config.ap.ssid)-1)
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Name) {

	// valid input ?
	if (SCDEH_GetSpecialStrVal(argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Name].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Name].len
		,(char*) &ap_wifi_config.ap.ssid
		,sizeof (ap_wifi_config.ap.ssid)-1
		,2) ) {	

		// mark affected readings for TX
		affectedReadings |= 
			parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Name].affectedReadings;

//		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=name");
	}

	// invalid input detected ?
	else return true;
  }

// -------------------------------------------------------------------------------------------------
// WSAP_Password=[a-zA-Z0-9_.] -> Setze das Wireless Service Access Point Passwort (char) (0-sizeof (ap_wifi_config.ap.password)-1)
// WSAP_Password=[a-zA-Z0-9_.] -> Set the Wireless Service Access Point Passwort (char) (0-sizeof (ap_wifi_config.ap.password)-1)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_Password) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Password].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Password].len
		,(char*) &ap_wifi_config.ap.password
		,sizeof (ap_wifi_config.ap.password)-1
		,2) ) {	

		// mark affected readings for TX
		affectedReadings |= 
			parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Password].affectedReadings;

//		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_Password");
	}

	// invalid input detected ?
	else return true;
  }

// -------------------------------------------------------------------------------------------------
// WSAP_RF_Channel=[0-9] -> Setze den Wireless Service Access Point Kanal (1-13 aber Station Kanal hat Priorität!)
// WSAP_RF_Channel=[0-9] -> Set the Wireless Service Access Point channel (1-13 but Station Channel has priority)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_RF_Channel) {

	uint8_t NewChan;

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val(argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].len
		,&NewChan)) {

		if ( (NewChan >= 1) && (NewChan <= 13) ) {

			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_RF_Channel");
		}
	}

	// invalid input detected ?
	else return true;
  }

// -------------------------------------------------------------------------------------------------
// WSAP_Maximal_Connections=[0-9] -> Setze die maximale Anzahl der Verbindungen zum WSAP (0-4)
// WSAP_Maximal_Connections=[0-9] -> Set maximum number of connections to the WSAP (0-4)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections) {

	uint8_t NewMaxConn;

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].len, &NewMaxConn)) {

		if ( (NewMaxConn >= 0) && (NewMaxConn <= 4) ) {

//			// save new max conn
//			ap_wifi_config.ap.max_connection = MaxConn;

			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=WSAP_RF_Channel");
		}
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------
// WSAP_Authentication_Method=[OPEN|WEP|WPA_PSK|WPA2_PSK|WPA_WPA2_PSK|WPA2_ENTERPRISE|MAX] -> Setze die Authentifizierungsmethode für den Wireless Service Access Point
// WSAP_Authentication_Method=[OPEN|WEP|WPA_PSK|WPA2_PSK|WPA_WPA2_PSK|WPA2_ENTERPRISE|MAX] -> Set the authentication method for the Wireless Service Access Point

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_Authentication_Method) {

	uint8_t NewAuthMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].len, &NewAuthMode, ESP32_DeVICE_AUTH_MODE)) {

//		// save auth mode
//		ap_wifi_config.ap.authmode = NewAuthMode;
printf ("XE");
		// mark affected readings for TX
		affectedReadings |= 
			parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].affectedReadings;

//		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_Authentication_Method");
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------

  // WSAP_SSID=[0-9] -> Setze Wert für den gruenen Anteil vom RGB Kanal (uint8) (0-x)
  // WSAP_SSID=[0-9] -> Set value for the gree component of the RGB channel (uint8) (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_SSID) {

	uint8_t NewSSIDSetting;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID].len, &NewSSIDSetting, ESP32_DeVICE_SSID)) {

		// save auth mode
//		ap_wifi_config.ap.ssid_hidden = NewSSIDSetting;
printf ("XF");
		// mark affected readings for TX
		affectedReadings |= 
			parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_SSID");

	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------

  // WSAP-Beacon-Interval=[100-60000] -> Setze den Beacon-Sendeinterval des Wireless Service Access Points
  // WSAP-Beacon-Interval=[100-60000] -> Set the beacon interval of the Wireless Service Access Point

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval) {

	uint16_t NewBI;

	// valid input happened ?
	if (SCDEH_GetDecUInt16Val(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].len, &NewBI)) {

		if ( (NewBI >= 100) && (NewBI <= 60000) ) {

			// save beacon interval
//			ap_wifi_config.ap.beacon_interval = (uint16_t) NewBI;
printf ("XG");
			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].affectedReadings;

			// New Timestamp
//			SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//				GetUniqueTIST();

			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP-Beacon-Interval");

		}
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------

  // gmax=[ ] -> Setze vom RGB Kanal den gruenen Anteil maximal (0-x)
  // gmax=[ ] -> Set green component of the RGB channel to maximum (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_IP_Adress) {

  }

// --------------------------------------------------------------------------------------------------

  // gmin=[ ] -> Setze vom RGB Kanal den gruenen Anteil minimal (0-x)
  // gmin=[ ] -> Set green component of the RGB channel to minimum (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_Netmask) {

  }

// --------------------------------------------------------------------------------------------------

  // bval=[0-9] -> Setze Wert für den blauen Anteil vom RGB Kanal (uint8) (0-x)
  // bval=[0-9] -> Set value for the blue component of the RGB channel (uint8) (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_Gateway_Adress) {

  }

// --------------------------------------------------------------------------------------------------


  // red=[ ] -> Setze RGB Kanal auf Leuchtfarbe rot (0-x)
  // red=[ ] -> Set RGB Channel SCDEing red (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_MAC_Adress) {

  }

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WiFi_Country) {

  }

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_WiFi_Bandwidth) {

  }

// --------------------------------------------------------------------------------------------------

  // blue=[ ] -> Setze RGB Kanal auf Leuchtfarbe blau (0-x)
  // blue=[ ] -> Set RGB Channel SCDEing blue (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_IP_Adress) {

  }

// --------------------------------------------------------------------------------------------------

  // oran=[ ] -> Setze RGB Kanal auf Leuchtfarbe orange (0-x)
  // oran=[ ] -> Set RGB Channel SCDEing orange (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_Netmask) {

  }

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_Gateway_Adress) {

  }

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_MAC_Adress) {

  }

// --------------------------------------------------------------------------------------------------

  // wval=[0-9] -> Setze Wert für warm-weissen Kanal (uint8) (0-x)
  // wval=[0-9] -> Set value for warm white channel (uint8) (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_Auto_Connect) {

  }

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth) {

  }

// --------------------------------------------------------------------------------------------------




















  // bmax=[ ] -> Setze vom RGB Kanal den blauen Anteil maximal (0-x)
  // bmax=[ ] -> Set blue component of the RGB channel to maximum (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_DHCPS_Lease_Start_IP) {

  }

//--------------------------------------------------------------------------------------------------

  // bmin=[ ] -> Setze vom RGB Kanal den blauen Anteil minimal (0-x)
  // bmin=[ ] -> Set blue component of the RGB channel to minimum (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_DHCPS_Lease_End_IP) {

  }

//--------------------------------------------------------------------------------------------------

  // bdim=[0-9] -> Setze dimmer für den blauen Anteil vom RGB Kanal (uint8) (0-x)
  // bdim=[0-9] -> Set dimming for the blue component of the RGB channel (uint8) (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_DHCPS) {

	uint8_t NewDHCPSStatus;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_DHCPS].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_DHCPS].len, &NewDHCPSStatus, DisEn)) {

		// save DHCPS Status
		WifiSoftAPDHCPSStatus = NewDHCPSStatus;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_DHCPS].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB03_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=WSAP_DHCPS");

	}
  }
*/
//--------------------------------------------------------------------------------------------------

  // gree=[ ] -> Setze RGB Kanal auf Leuchtfarbe grün (0-x)
  // gree=[ ] -> Set RGB Channel SCDEing green (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP) {

	uint8_t NewApOpMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP].len, &NewApOpMode, DisEn)) {

		// save ap op mode
		WifiApOpMode = NewApOpMode;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB05_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=WSAP");

//		RespArgsWPos = SCDEH_AddCmdToWritePos(RespArgsWPos
//			,SCDE_QueryKeys
//			,ESP32_DeVICE_Set_IK_WSAP);

	}
  }
*/
//--------------------------------------------------------------------------------------------------

  // blck=[ ] -> Setze RGB Kanal auf Leuchtfarbe schwarz (0-x)
  // blck=[ ] -> Set RGB Channel SCDEing black (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station) {

  }

//--------------------------------------------------------------------------------------------------

  // whte=[ ] -> Setze RGB Kanal auf Leuchtfarbe weiss (0-x)
  // whte=[ ] -> Set RGB Channel SCDEing white (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_DHCP) {

  }

//--------------------------------------------------------------------------------------------------

  // wdim=[0-9] -> Setze dimmer für warm-weissen Kanal (uint8) (0-x)
  // wdim=[0-9] -> Set dimming for warm white channel (uint8) (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_Physical_Mode) {

	uint8_t NewPhyMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Physical_Mode].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Physical_Mode].len, &NewPhyMode, PhMode)) {

		// save Phy Mode
		WifiStationPhyMode = NewPhyMode;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Physical_Mode].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB10_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=Station_Physical_Mode");

	}
  }
*/
//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setze warm-weissen Kanal maximal (0-x)
  // wmax=[ ] -> Set warm white channel maximum (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Station_Get_RSSI) {

  }

//--------------------------------------------------------------------------------------------------

  // TiSt_NTP_Server_Main=[azAZ09._] -> Setze NTP-Server Adresse (0-x)
  // TiSt_NTP_Server_Main=[azAZ09._] -> Set NTP-Server adress (0-x)
/*
  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Main) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Main].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Main].len
		,(char*) &SysCfgRamFlashMirrored->NTDomain1
		,sizeof (SysCfgRamFlashMirrored->NTDomain1)
		,2) ) {	

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// stop sntp for changes
		sntp_stop();

		// set new  sntp server name
		sntp_setservername(0, (char*) &SysCfgRamFlashMirrored->NTDomain1);

		// reinit sntp
		sntp_init();

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Main].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_NTP_Server_Main");

	}
  }
*/
//--------------------------------------------------------------------------------------------------

  // TiSt_NTP_Server_Backup_1=[azAZ09._] -> Setze NTP-Server Adresse, Backup 1 (0-x)
  // TiSt_NTP_Server_Backup_1=[azAZ09._] -> Set NTP-Server adress, Backup 1 (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_1) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_1].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_1].len
		,(char*) &SysCfgRamFlashMirrored->NTDomain2
		,sizeof (SysCfgRamFlashMirrored->NTDomain2)
		,2) ) {	

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// stop sntp for changes
		sntp_stop();

		// set new  sntp server name
		sntp_setservername(1, (char*) &SysCfgRamFlashMirrored->NTDomain2);

		// reinit sntp
		sntp_init();

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_1].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_NTP_Server_Backup_1");

	}
  }
*/
//--------------------------------------------------------------------------------------------------

  // TiSt_NTP_Server_Backup_2=[azAZ09._] char[?] -> Setze NTP-Server Adresse, Backup 2 (0-x)
  // TiSt_NTP_Server_Backup_2=[azAZ09._] char[?] -> Set NTP-Server adress, Backup 2 (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_2) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_2].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_2].len
		,(char*) &SysCfgRamFlashMirrored->NTDomain3
		,sizeof (SysCfgRamFlashMirrored->NTDomain3)
		,2) ) {	

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// stop sntp for changes
		sntp_stop();

		// set new  sntp server name
		sntp_setservername(2, (char*) &SysCfgRamFlashMirrored->NTDomain3);

		// reinit sntp
		sntp_init();

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_2].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_NTP_Server_Backup_2");

	}
  }
*/
//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setze warm-weissen Kanal maximal (0-x) 
  // wmax=[ ] -> Set warm white channel maximum (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_TiSt_Time_Zone) {

	int8 NewTZ;

	// valid input happened ?
	if (SCDEH_GetDecInt8Val((uint8*)Args+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_Time_Zone].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_Time_Zone].len, &NewTZ)) {

		if ( (NewTZ >= -11) && (NewTZ <= 13) ) {

			// save new time-zone
			SysCfgRamFlashMirrored->NTTimezone = NewTZ;

			// save mirror
			SCDE_SaveSysCfgRamFlashMirroredToMirror();

			// stop sntp for changes
			sntp_stop();

			// set new sntp TZ
			sntp_set_timezone(NewTZ);

			// reinit sntp
			sntp_init();

			// mark affected readings for TX
			affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_TiSt_Time_Zone].affectedReadings;

			// New Timestamp
			SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
				GetUniqueTIST();

			// Push processed CMD to to Response ...
			RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_Time_Zone");

		}
	}

  }
*/
/*
//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setze warm-weissen Kanal maximal (0-x)
  // wmax=[ ] -> Set warm white channel maximum (0-x)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_TiSt_Get_Time) {

  }


//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setzt das Geräte warm-weissen Kanal maximal (0-x)
  // wmax=[ ] -> Set the Device into state 'restart'

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_SoCHW_Restart) {

	system_restart();

  }
*/
//--------------------------------------------------------------------------------------------------

  // SoCHW_Factory_Reset_SCDE=[ ] -> Setzt die Smart-Connected-Device-Engine auf Werkseinstellung zurück 'factory reset'
  // SoCHW_Factory_Reset_SCDE=[ ] -> Set Smart-Connected-Device-Engine to state 'factory reset'
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset_SCDE) {

//	SCDE_ReconstructSysCfgRomNoMirror();		// todo
//	SCDE_ReconstructSysCfgRamNoMirror();		// reconstruct Sys-Cfg stored in ram (done by reboot)
	SCDE_ReconstructSysCfgRamFlashMirrored();	// reconstruct Sys-Cfg stored in ram (flash-mirrored) 
	SCDE_ReconstructSysCfgRamRtcMirrored();		// reconstruct Sys-Cfg stored in rtc

	// initial setup / correction of the SDK parameters
	SCDE_SDKInitialParameterSetup();

	// fresh start
	system_restart();

  }
*/
//--------------------------------------------------------------------------------------------------

  // SoCHW_Factory_Reset_CAPI=[ ] -> Setzt die SoC-Hardware-Chip-API auf Werkseinstellung zurück 'factory reset'
  // SoCHW_Factory_Reset_CAPI=[ ] -> Set SoC-Hardware-Chip-API to state 'factory reset'

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset_CAPI) {

	// restore SDK parameters (SDK-factory reset)
//	system_restore();

//	SCDE_ReconstructSysCfgRomNoMirror();		// todo
//	SCDE_ReconstructSysCfgRamNoMirror();		// reconstruct Sys-Cfg stored in ram (done by reboot)
//	SCDE_ReconstructSysCfgRamFlashMirrored();	// reconstruct Sys-Cfg stored in ram (flash-mirrored) 
//	SCDE_ReconstructSysCfgRamRtcMirrored();		// reconstruct Sys-Cfg stored in rtc

	// initial setup / correction of the SDK parameters
//	SCDE_SDKInitialParameterSetup();

	// fresh start
//	system_restart();

  }

//--------------------------------------------------------------------------------------------------

  // SoCHW_Factory_Reset_CAPI=[ ] -> Setzt die SoC-Hardware-Chip-API auf Werkseinstellung zurück 'factory reset'
  // SoCHW_Factory_Reset_CAPI=[ ] -> Set SoC-Hardware-Chip-API to state 'factory reset'

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset) {

	// restore SDK parameters (SDK-factory reset)
//	system_restore();

	// initial setup / correction of the SDK parameters
//	SCDE_SDKInitialParameterSetup();

	// fresh start
//	system_restart();

  }

//>Caps Bit n.a.------------------------------------------------------------------------------------

  // name=[a-zA-Z0-9_.] -> Setze einen neuen Feature Namen (char[31]) (0-x)
  // name=[a-zA-Z0-9_.] -> Set a new Feature Name (char[31]) (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_Name) {

  }
*/
//>Caps Bit n.a.------------------------------------------------------------------------------------

  // caps=[ ] -> Fragt nach den Fähigkeiten-Bits (0-x)
  // caps=[ ] -> Request for Capabilities-Bits (0-x)
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_caps) {

	// mark affected readings for TX
	affectedReadings |= parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_caps].affectedReadings;

	// New Timestamp
//	WriteInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].IB01_X_TiSt,
//		GetUniqueTIST());
//bug in anderem code

	// Push processed CMD to to Response ...
//	RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=caps");

  }
*/

















// ------------------------------------------------------------------------------------------------

// 3. Step: Check if the ammount of input elements meets our criteria

  // check if all required Key=Value components are included -> return with FALSE
  if ( (parsedKVInput->keysFoundBF & parsedKVInput->requiredKVBF) != parsedKVInput->requiredKVBF) {

	#if ESP32_SwITCH_Module_DBG >= 5
	printf("|requiredKVBF, no match!>");	
  	#endif

	return true;
  }

  // check if forbidden Key=Value components are included -> return with FALSE
  if (parsedKVInput->keysFoundBF & parsedKVInput->forbiddenKVBF) {

	#if ESP32_SwITCH_Module_DBG >= 5
	printf("|forbiddenKVBF, match!>");	
  	#endif

	return true;
  }

// -------------------------------------------------------------------------------------------------
// 4. Step: Passed. Store the new values to SDK
// -------------------------------------------------------------------------------------------------
/*

  // block #01 readings, store ap_wifi_config (WSAP-Wifi-Configuration), then save & set
  if (parsedKVInput->keysFoundBF & ( (1 << ESP32_DeVICE_Set_IK_Name)
				   | (1 << ESP32_DeVICE_Set_IK_WSAP_Password)
				   | (1 << ESP32_DeVICE_Set_IK_WSAP_RF_Channel)
				   | (1 << ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections)
				   | (1 << ESP32_DeVICE_Set_IK_WSAP_Authentication_Method)
				   | (1 << ESP32_DeVICE_Set_IK_WSAP_SSID)
				   | (1 << ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval) ) ) {

	# if ESP32_SwITCH_Module_DBG >= 5
	SCDEFn_at_ESP32_DeVICE_M->HexDumpOutFn ("WSAP: ap_wifi_config changed !",
		&ap_wifi_config,
		sizeof(ap_wifi_config));
	# endif

	// store length of ssid (may be changed)
	ap_wifi_config.ap.ssid_len = 
		strlen((char*)&ap_wifi_config.ap.ssid);

	// Set new WSAP Cfg
	esp_wifi_set_config(WIFI_IF_AP, &ap_wifi_config);

	// Set new Hostname
	tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, (const char *) &ap_wifi_config.ap.ssid);
  }
*/
// -------------------------------------------------------------------------------------------------



























/*
// =================================================================================================
// =================================================================================================
// =================================================================================================

// -------------------------------------------------------------------------------------------------
// Save changes to SDK
// -------------------------------------------------------------------------------------------------

  // Processed block #01 readings, that affects ap_wifi_config (WSAP-Wifi-Configuration) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_Name
			| ESP32_DeVICE_R_WSAP_Password
			| ESP32_DeVICE_R_WSAP_RF_Channel
			| ESP32_DeVICE_R_WSAP_Maximal_Connections
			| ESP32_DeVICE_R_WSAP_Authentication_Method
			| ESP32_DeVICE_R_WSAP_SSID
			| ESP32_DeVICE_R_WSAP_Beacon_Interval) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP ap_wifi_config ch!",
		&ap_wifi_config,
		sizeof(ap_wifi_config));
	# endif

	// store length of ssid (may be changed)
	ap_wifi_config.ap.ssid_len = 
		strlen((char*)&ap_wifi_config.ap.ssid);

	// Set new WSAP Cfg
	esp_wifi_set_config (WIFI_IF_AP, &ap_wifi_config);

	// Set new Hostname
	tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, (const char *) &ap_wifi_config.ap.ssid);

  }

// -------------------------------------------------------------------------------------------------

   // Processed block #02 readings, that affects AP_ip_info (WSAP-Ip-Info) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_WSAP_IP_Adress
			| ESP32_DeVICE_R_WSAP_Netmask
			| ESP32_DeVICE_R_WSAP_Gateway_Adress) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP ip_info ch!",
		&ap_ip_info,
		sizeof(ap_ip_info));
	# endif

	// Set new AP IP INFO
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip_info);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #03 readings, that affects ap_mac_addr (WSAP-MAC-Adress) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_WSAP_MAC_Adress ) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP mac_addr ch!",
		&ap_mac_addr,
		sizeof(ap_mac_addr));
	# endif

	// Set new AP macaddr
	esp_wifi_set_mac(WIFI_IF_AP,(uint8_t *) &ap_mac_addr);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #04 readings, that affects ap_wifi_bandwidth (WSAP-WiFi-Bandwidth) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_WSAP_WiFi_Bandwidth ) ) {

	// Set new AP wifi_bandwidth
	esp_wifi_set_bandwidth(WIFI_IF_AP, ap_wifi_bandwidth);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #05 readings, that affects country WiFi-Country) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_WiFi_Country ) ) {

	// Set new WiFi country
	esp_wifi_set_country(&country);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #06 readings, that affects sta_ip_info (Station-Ip-Info) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_Station_IP_Adress
			|ESP32_DeVICE_R_Station_Netmask
			|ESP32_DeVICE_R_Station_Gateway_Adress ) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("STA ip_info ch!",
		&sta_ip_info,
		sizeof(sta_ip_info));
	# endif

	// Set new STA ip_info
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip_info);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #07 readings, that affects sta_mac_addr (Station-MAC-Adress) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_Station_MAC_Adress ) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("STA mac_addr ch!",
		&sta_mac_addr,
		sizeof(sta_mac_addr));
	# endif

	// Set new AP macaddr
	esp_wifi_set_mac(WIFI_IF_STA,(uint8_t *) &sta_mac_addr);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #08 readings, that affects sta_autoconnect_status (Station_Auto_Connect) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_Station_Auto_Connect ) ) {

	// Set new auto-connect-status
	esp_wifi_set_auto_connect(sta_autoconnect_status);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #09 readings, that affects sta_wifi_bandwidth (Station-WiFi-Bandwidth) ? then save & set
  if (affectedReadings & ( ESP32_DeVICE_R_Station_WiFi_Bandwidth ) ) {

	// Set new station bandwidth
	esp_wifi_set_bandwidth(WIFI_IF_STA, sta_wifi_bandwidth);

  }

// -------------------------------------------------------------------------------------------------

*/









/*
  // Processed block #03 readings, that affects AP_dhcps_lease (Service-Access-Point-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_WSAP_DHCPS_Lease_Start_IP
			| SCDE_R_WSAP_DHCPS_Lease_End_IP
			| SCDE_R_WSAP_IP_Adress) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP dhcps_lease ch!",
		&dhcps_lease,
		sizeof(dhcps_lease));
	# endif

	// Set new AP dhcps_lease
	wifi_softap_set_dhcps_lease(&dhcps_lease);

	// set new DHCPS Status
	if (WifiSoftAPDHCPSStatus) tcpip_adapter_dhcpc_start(WIFI_IF_AP);
	else tcpip_adapter_dhcpc_stop(WIFI_IF_AP);

	}
*/



/*  // block #5 get the bandwith of the Service Access Point
  wifi_bandwidth_t ap_wifi_bandwidth;
  esp_wifi_get_bandwidth(WIFI_IF_AP, &ap_wifi_bandwidth);

  // block #6 get current Station Settings
  wifi_config_t sta_wifi_config;
  esp_wifi_get_config (WIFI_IF_STA, &sta_wifi_config);*/



/*
  // Processed block 05 readings, that affects AP dhcps status (Service-Access-Point-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_WSAP ) )

	{

	// Set new AP opmode
	esp_wifi_set_mode( (esp_wifi_get_mode() & 0b01) | (WifiApOpMode << 1) );

	}
*/






/*
 // Processed block 07 readings, that affects STA opmode (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station ) )

	{

	// Set new AP opmode
 	//  int WifiStaOpMode = (wifi_get_opmode() & 0b01);

	}


  // Processed block 08 readings, that affects STA dhcp (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_DHCP ) )

	{

	// Set new ?
 	//  int WifiStationDHCPStatus = wifi_station_dhcpc_status();

	tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);
	tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);
	}


  // Processed block 9 readings, that affects STA auto connect status (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_Auto_Connect ) )

	{

	// Set new STA auto connect status
 	//wifi_station_set_auto_connect(WifiStationAutoConnectStatus);

	}


 // Processed block 10 readings, that affects STA phy mode (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_Physical_Mode ) )

	{

	// Set new STA phy mode
 	wifi_set_phy_mode(WifiStationPhyMode);

	}
*/





// -------------------------------------------------------------------------------------------------
// 5. Step: update the Readings according to 'affectedReadings' - if any ...
// -------------------------------------------------------------------------------------------------

  // debugview all readings
 // affectedReadings = 0xffffffff;

 printf ("AffectedReadings:%d", affectedReadings);

  // update the Readings according to 'affectedReadings' - if any ...
  if (affectedReadings) {

	SCDEFn_at_ESP32_DeVICE_M->readingsBeginUpdateFn((Common_Definition_t*) ESP32_DeVICE_Definition);

// ------------------------------------------------------------------------------------------------- 

	// Reading 'Name'
	if (affectedReadings & ESP32_DeVICE_R_Name) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Name].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Name].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Name].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Name].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WSAP_Password'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Password) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Password].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Password].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Password].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Password].off
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_RF_Channel'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_RF_Channel) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].off
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_Maximal_Connections'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Maximal_Connections) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].off
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_Authentication_Method'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Authentication_Method) {

 		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].off
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_SSID'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_SSID) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_SSID].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_SSID].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID].off
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_Beacon_Interval'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Beacon_Interval) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WSAP_IP_Adress'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_IP_Adress) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_IP_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_IP_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_IP_Adress].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_IP_Adress].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WSAP_Netmask'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Netmask) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Netmask].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Netmask].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Netmask].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Netmask].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WSAP_Gateway_Adress'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Gateway_Adress) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Gateway_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Gateway_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Gateway_Adress].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Gateway_Adress].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WSAP_MAC_Adress'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_MAC_Adress) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_MAC_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_MAC_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_MAC_Adress].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_MAC_Adress].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WSAP_WiFi_Bandwidth'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_WiFi_Bandwidth) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_WiFi_Bandwidth].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_WiFi_Bandwidth].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_WiFi_Bandwidth].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_WiFi_Bandwidth].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WiFi_Country'
	if (affectedReadings & ESP32_DeVICE_R_WiFi_Country) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WiFi_Country].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WiFi_Country].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WiFi_Country].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WiFi_Country].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'Station_IP_Adress'
	if (affectedReadings & ESP32_DeVICE_R_Station_IP_Adress) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_IP_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_IP_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_IP_Adress].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_IP_Adress].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// add Reading 'Station_Netmask'
	if (affectedReadings & ESP32_DeVICE_R_Station_Netmask) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Netmask].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Netmask].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Netmask].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Netmask].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// add Reading 'Station_Gateway_Adress'
	if (affectedReadings & ESP32_DeVICE_R_Station_Gateway_Adress) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'Station_MAC_Adress'
	if (affectedReadings & ESP32_DeVICE_R_Station_MAC_Adress) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_MAC_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_MAC_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_MAC_Adress].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_MAC_Adress].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'Station_Auto_Connect'
	if (affectedReadings & ESP32_DeVICE_R_Station_Auto_Connect) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Auto_Connect].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Auto_Connect].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Auto_Connect].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Auto_Connect].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// add Reading 'Station_WiFi_Bandwidth'
	if (affectedReadings & ESP32_DeVICE_R_Station_WiFi_Bandwidth) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readingsBulkUpdate2Fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].off
			,true);
	}
//uint8_t *argsText
// -------------------------------------------------------------------------------------------------

  SCDEFn_at_ESP32_DeVICE_M->readingsEndUpdateFn((Common_Definition_t*) ESP32_DeVICE_Definition, true);
  }

// =================================================================================================
// =================================================================================================
// =================================================================================================





// ------------------------------------------------------------------------------------------------

// 5. Step: Passed. Return OK

  return false;

}




















/*
//> Add Readings: Internals - TiSt-Sharing - Block 3 ------------------------------------------------

  if (affectedReadings & (SCDE_R_WSAP_DHCPS_Lease_Start_IP
		| SCDE_R_WSAP_DHCPS_Lease_End_IP
		| SCDE_R_WSAP_IP_Adress ) ) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&WSAP_DHCPS_Lease_Start_IP=%03d.%03d.%03d.%03d"
		 "&WSAP_DHCPS_Lease_End_IP=%03d.%03d.%03d.%03d"
		 "&WSAP_IP_Adress=%s"
		, dhcps_lease.start_ip.addr & 0xff
		,(dhcps_lease.start_ip.addr >> 8) & 0xff
		,(dhcps_lease.start_ip.addr >> 16) & 0xff
		, dhcps_lease.start_ip.addr >> 24
		, dhcps_lease.end_ip.addr & 0xff
		,(dhcps_lease.end_ip.addr >> 8) & 0xff
		,(dhcps_lease.end_ip.addr >> 16) & 0xff
		, dhcps_lease.end_ip.addr >> 24
		,SCDE_GetDesc(DisEn, WifiSoftAPDHCPSStatus));
  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 5 ------------------------------------------------

  if (affectedReadings & SCDE_R_WSAP) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&WSAP=%s"
		,SCDE_GetDesc(DisEn, WifiApOpMode));

  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 7 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station=%s"
		,SCDE_GetDesc(DisEn, WifiStaOpMode) );

  }

//> Add Readings: Internals - TiSt-Sharing - Block 8 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station_DHCP) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station_DHCP=%s"
		,SCDE_GetDesc(DisEn, WifiStationDHCPStatus));

  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 10 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station_Physical_Mode) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station_Physical_Mode=%s"
		,SCDE_GetDesc(PhMode, WifiStationPhyMode));

  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 12 ------------------------------------------------

  if (affectedReadings & (SCDE_R_TiSt_NTP_Server_Main
		| SCDE_R_TiSt_NTP_Server_Backup_1
		| SCDE_R_TiSt_NTP_Server_Backup_2
		| SCDE_R_TiSt_Time_Zone) ) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&TiSt_NTP_Server_Main=%s"
		 "&TiSt_NTP_Server_Backup_1=%s"
		 "&TiSt_NTP_Server_Backup_2=%s"
		 "&TiSt_Time_Zone=%d"
		,sntp_getservername(0)
		,sntp_getservername(1)
		,sntp_getservername(2)
		,sntp_get_timezone());

  }

//> Add Readings: Readings - TiSt-Sharing - Block 1 ------------------------------------------------

  if (affectedReadings & (SCDE_R_Station_RSSI
		| SCDE_R_TiSt_Time) ) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station_RSSI=%d"
		 "&TiSt_Time=%s"
		,wifi_station_get_rssi()
		,sntp_get_real_time(sntp_get_current_timestamp()));

  }

//> Add Readings: Readings - TiSt-Sharing - Block 2 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station_RSSI) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&SoC_Info=SDKV:%s ChID:%X BootV:%d BootP:%X FlashID:%X"
		 "&Firmware=V %u.%u, Build %u"
		,system_get_sdk_version()
		,system_get_chip_id()
		,system_get_boot_version()
		,system_upgrade_userbin_check()
		,spi_flash_get_id()
		,(unsigned int) SysCfgRamFlashMirrored->Version_major
		,(unsigned int) SysCfgRamFlashMirrored->Version_minor
		,(unsigned int) SysCfgRamFlashMirrored->build);

  }


//>always add Reading - state ----------------------------------------------------------------------

	// are we adding ?
//problem	if (RespArgsWPos != RespArgsBuf) RespArgsWPos += os_sprintf(RespArgsWPos,"&");

	// always add state ! on / off
	if ( (SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB01_RGBPixel[0] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB01_RGBPixel[1] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB01_RGBPixel[2] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB02_WWPixel[0] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB03_CWPixel[0] == 0) )
		RespArgsWPos += os_sprintf(RespArgsWPos, "state=off");
	else  RespArgsWPos += os_sprintf(RespArgsWPos, "state=on");
	RespArgsWPos += os_sprintf(RespArgsWPos, "state=todo");

//--------------------------------------------------------------------------------------------------
*/
/*
  // free allocated memory for query result key-field
  free(parsedKVInput);


























  // make common ptr to modul specific ptr
  ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
	(ESP32_DeVICE_Definition_t*) ESP32_DeVICE_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|ESP32_DeVICE_Set, Name:%.*s, got args:%.*s>"
	,ESP32_DeVICE_Definition->common.nameLen
	,ESP32_DeVICE_Definition->common.name
	,setArgsLen
	,setArgs);
  #endif


  // response with error text
  // response with error text
	// alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"ESP32_DeVICE_Set, Name:%.*s, got args:%.*s"
	,ESP32_DeVICE_Definition->common.nameLen
	,ESP32_DeVICE_Definition->common.name
	,setArgsLen
	,setArgs);

  return retMsg;

  }

*/

















