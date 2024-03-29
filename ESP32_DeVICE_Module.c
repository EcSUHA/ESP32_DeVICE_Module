﻿/* #################################################################################################
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

//##################################################################################################
#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>



// the Smart-Connected-Device-Engine structures & types ...
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module_s.h"
#include "WebIf_Module.h"

/*
 * this Modules structures & types ...
 */
#include "ESP32_DeVICE_Module.h"
#include "ESP32_DeVICE_readings.h"

/*
 *      the core module fn
 */ 
#include "ESP32_DeVICE_Attribute.h"
#include "ESP32_DeVICE_Define.h"
//#include "ESP32_DeVICE_Delete.c"
//#include ""ESP32_DeVICE_DirectRead.c"
//#include ""ESP32_DeVICE_DirectWrite.c"
//#include ""ESP32_DeVICE_Except.c"
#include "ESP32_DeVICE_Get.h"
//#include ""ESP32_DeVICE_Idle.c"
#include "ESP32_DeVICE_Initialize.h"
#include "ESP32_DeVICE_Notify.h"
//#include ""ESP32_DeVICE_Parse.c"		
//#include ""ESP32_DeVICE_Read.c"	
//#include ""ESP32_DeVICE_Ready.c"	
#include "ESP32_DeVICE_Rename.h"
#include "ESP32_DeVICE_Set.h"
#include "ESP32_DeVICE_Shutdown.h"
#include "ESP32_DeVICE_State.h"
#include "ESP32_DeVICE_Undefine.h"
//#include "ESP32_DeVICE_Write.h"

/*
 *      the old webif v1.5
 */
#include "ESP32_DeVICE_Webif_Get_Cb.h"
#include "ESP32_DeVICE_Webif_Jso_Cb.h"
#include "ESP32_DeVICE_Webif_Set_Cb.h"




// unsorted 

#include <WebIf_EspFSStdFileTX.h>
#include <WebIf_EspFSAdvFileTX.h>

#include "HTools_cgi.h"
#include <ServAPCfg_tpl.h>
#include <CGI_Redirect.h>


#include "freertos/event_groups.h"



















/**
 * -------------------------------------------------------------------------------------------------
 *  DName: _ProvidedByModule (new:_provided_by_module)
 *  Desc: Data 'Provided By Module' for the core module (functions + infos this module provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */
ProvidedByModule_t ESP32_DeVICE_ProvidedByModule = { 	// A-Z order
  "scde"					        // Type-Name of module -> on Linux libfilename.so !
  ,4								// size of Type-Name

  ,&ESP32_DeVICE_reading_types      // embedded reading types
  
  ,NULL								// Add
  ,ESP32_DeVICE_Attribute			// Attribute
  ,ESP32_DeVICE_Define				// Define
  ,NULL								// Delete
  ,NULL								// DirectRead
  ,NULL								// DirectWrite
  ,NULL								// Except
  ,ESP32_DeVICE_Get					// Get
  ,NULL								// IdleCb
  ,ESP32_DeVICE_Initialize			// Initialize
  ,ESP32_DeVICE_Notify				// Notify
  ,NULL								// Parse
  ,NULL								// Read
  ,NULL								// Ready
  ,ESP32_DeVICE_Rename				// Rename
  ,ESP32_DeVICE_Set					// Set
  ,ESP32_DeVICE_Shutdown			// Shutdown
  ,ESP32_DeVICE_State				// State
  ,NULL								// Sub
  ,ESP32_DeVICE_Undefine			// Undefine
  ,NULL								// Write
  ,NULL								// FnProvided
  ,sizeof(Entry_ESP32_DeVICE_Definition_t)	// Modul specific Size (Common_Definition_t + X)
};



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: _ActiveResourcesDataA (Neu: _webif_active_directory_content_a)
 *  Desc: Resource-Content-structure of active Directory - PART A (Resource-Data-Row)
 *  Data: WebIf_ActiveResourcesDataA_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// Content:   AllowedMethodBF          | AllowedDocMimeBF  | AllowedSchemeBF |free|CgiNo| EnaByBit | Url
const WebIf_ActiveResourcesDataA_t ESP32_DeVICE_ActiveResourcesDataA_forWebIf[] = {

// Multi SCDE(es)-Feature Control
  // GET http://192.168.0.24:80/SCDE .htm oder .jso
  // POST http://192.168.0.24:80/SCDE .htm oder .jso
  { 0b00000000000000000000000000001010, 0b0000000000001010, 0b0000000000000001,     0,    3, 0b00000000, "/SCDE"},

  // SCDE-Feature Control (http + http compatibility resources incl. method compatibility mode) - adressing by seq. no. of definition (from this TYPE)
  // GET http://192.168.0.24:80/3 .htm, .cgi, .jso, .set oder .get
  // POST http://192.168.0.24:80/3 .htm, .cgi, .jso, .set oder .get
  { 0b00000000000000000000000000001010, 0b0000000001011110, 0b0000000000000001,     0,    1, 0b00000000, "/\xf1/SCDE\x00 /SCDEX.htm"},

  // SCDE-Feature Control (http + http compatibility resources incl. method compatibility mode) - adressing by NAME
  // GET http://192.168.0.24:80/SSR.1.at.GPIO.13 .htm, .cgi, .jso, .set oder .get
  // POST http://192.168.0.24:80/SSR.1.at.GPIO.13 .htm, .cgi, .jso, .set oder .get
  { 0b00000000000000000000000000001010, 0b0000000001011110, 0b0000000000000001,     0,    1, 0b00000000, "/\xf3\x00 /SCDEX.htm"},

// SCDE-Feature Control (Home Automation scheme SCDE incl. method compatibility mode) - adressing by seq. no. of definition (from this TYPE)
  // GET scde://192.168.0.24:80/3                     SCDE-Feature-No.     Control - Home Automation Scheme SCDE:// access Resources
  { 0b00000000000000000000000000000010, 0b0000000000000001, 0b0000000000000010,     0,    1, 0b00000000, "/\xf1\x00\x08"},
  // SET scde://192.168.0.24:80/3                     SCDE-Feature-No.     Control - Home Automation Scheme SCDE:// access Resources
  { 0b00000000000000000000000000000100, 0b0000000000000001, 0b0000000000000010,     0,    2, 0b00000000, "/\xf1\x00\x08"},
    // kompatibilität - nötig ?
    // GET scde://192.168.0.24:80/3.get                 SCDE-Feature-No.     Control - Home Automation Scheme SCDE:// access Resources
    { 0b00000000000000000000000000000010, 0b0000000001000000, 0b0000000000000010,   0,    1, 0b00000000, "/\xf1\x00\x08"},
    // GET scde://192.168.0.24:80/3.set                 SCDE-Feature-No.     Control - Home Automation Scheme SCDE:// access Resources
    { 0b00000000000000000000000000000010, 0b0000000000010000, 0b0000000000000010,   0,    1, 0b00000000, "/\xf1\x00\x08"},

//SCDE-Feature Control (Home Automation scheme SCDE incl. method compatibility mode) - adressing by NAME
  // GET scde://192.168.0.24:80/SSR.1.at.GPIO.13      RENAMED SCDE-Feature Control - Home Automation SCDE:// access Resources
  { 0b00000000000000000000000000000010, 0b0000000000000001, 0b0000000000000010,     0,    1, 0b00000000, "/\xf3\x00\x08"},
  // SET scde://192.168.0.24:80/SSR.1.at.GPIO.13      RENAMED SCDE-Feature Control - Home Automation SCDE:// access Resources
  { 0b00000000000000000000000000000100, 0b0000000000000001, 0b0000000000000010,     0,    2, 0b00000000, "/\xf3\x00\x08"},
    // kompatibilität - nötig ?
    // GET scde://192.168.0.24:80/SSR.1.at.GPIO.13.get  RENAMED SCDE-Feature Control - Home Automation SCDE:// access Resources
    { 0b00000000000000000000000000000010, 0b0000000001000000, 0b0000000000000010,   0,    1, 0b00000000, "/\xf3\x00\x08"},
    // GET scde://192.168.0.24:80/SSR.1.at.GPIO.13.set  RENAMED SCDE-Feature Control - Home Automation SCDE:// access Resources
    { 0b00000000000000000000000000000010, 0b0000000000010000, 0b0000000000000010,   0,    1, 0b00000000, "/\xf3\x00\x08"},


// old stuff

  // SOC Hardware Cfg
  { 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  4, 0b00000000, "/SoCHWCfg"}

  // WiFi Stations Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  5, 0b00000000, "/WiFi/StationCfg"}

  // WIFI Q-Connect PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  6, 0b00000000, "/WiFi/QConnect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000001000, 0b0000000000000001,  0,  7, 0b00000000, "/WiFi/WiFiScan"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  8, 0b00000000, "/WiFi/Connect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  9, 0b00000000, "/WiFi/Setmode"}

  // Service Access Point Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 10, 0b00000000, "/WiFi/ServAPCfg"}

  // TimeStamp Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 11, 0b00000000, "/TiStCfg"}

  // Firmware Update
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 12, 0b00000000, "/Firmware"}

  // Redirects
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 13, 0b00000000, "/WiFi"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 13, 0b00000000, "/WiFi/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 14, 0b00000000, "/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 14, 0b00000000, "/index"}	// .htm

  // Secret services
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 15, 0b00000000, "/32MBitFlash"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 16, 0b00000000, "/wfs"}

 ,{0,0,0,0,0,0,"*"}
};



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: _ActiveResourcesDataB_forWebIf (Neu: _webif_active_directory_content_a)
 *  Desc: Resource-Content-structure of active Directory - PART B (Procedure-Call-Data-Row) 
 *  Data: WebIf_ActiveResourcesDataB_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
const WebIf_ActiveResourcesDataB_t ESP32_DeVICE_ActiveResourcesDataB_forWebIf[] =  {
// CgiFucID=(No.<<16)+AllowedSchemeBF |      cgi            |     cgi_data

  // SCDE-Definition-Feature Control
//{(1<<16) +	0b0000000000000010,    WebIf_EspFSAdvFileTX,        ESP32_SCDEtpl },
//{(1<<16) +	0b0000000000000100,    ESP32_SCDE_cgi,    	        NULL },
  {(1<<16) +	0b0000000000001001,    ESP32_DeVICE_Webif_Jso_Cb,   NULL }, // ohne, .jso,
  {(1<<16) +	0b0000000001000000,    ESP32_DeVICE_Webif_Get_Cb,	NULL }, // experimental .get
  {(1<<16) +	0b0000000000010000,    ESP32_DeVICE_Webif_Set_Cb,   NULL },
  {(2<<16) +	0b0000000000010001,    ESP32_DeVICE_Webif_Set_Cb,   NULL }, // Ergänzung für .set ,2 ohne mime

  // SCDE-Module-Feature Control (All SCDE(es)-Feature Control)
//{(3<<16) +	0b0000000000000010,    WebIf_EspFSAdvFileTX,        ESP32_SCDE_tpl },
//{(3<<16) +	0b0000000000001000,    ESP32_SCDE_jso,              NULL }


// old stuff

  // ### SOC Hardware Cfg ###
  {( 4<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SoCHWCfg_tpl	}
 ,{( 4<<16) +	0b0000000000000100,	NULL,NULL}//	SoCHWCfg_cgi,		NULL		}
 ,{( 4<<16) +	0b0000000000001000,	NULL,NULL}//	SoCHWCfg_jso,		NULL		}

  // ### WiFi Station Cfg ###
 ,{( 5<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	StationCfg_tpl	}
 ,{( 5<<16) +	0b0000000000000100,	NULL,NULL}//	StationCfg_cgi,		NULL		}
 ,{( 5<<16) +	0b0000000000001000,	NULL,NULL}//	StationCfg_jso,		NULL		}

  // ### WIFI Q-Connect PAGES ###
 ,{( 6<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	WifiQConnect_tpl}
 ,{( 7<<16) +	0b0000000000001000,	NULL,NULL}//	WiFiScan_jso,		NULL		}
 ,{( 8<<16) +	0b0000000000000100,	NULL,NULL}//	WiFiConnect_cgi,	NULL		}
 ,{( 9<<16) +	0b0000000000000100,	NULL,NULL}//	WifiSetMode_cgi,	NULL		}	

  // ### Service Access Point Cfg ###
 ,{(10<<16) +	0b0000000000000010,	WebIf_EspFSAdvFileTX,	ServAPCfg_tpl	}
 ,{(10<<16) +	0b0000000000000100,	NULL,NULL}//	ServAPCfg_cgi,		NULL		}
 ,{(10<<16) +	0b0000000000001000,	NULL,NULL}//	ServAPCfg_jso,		NULL		}

  // ### TimeStamp Cfg ###
 ,{(11<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	TiStCfg_tpl	}
 ,{(11<<16) +	0b0000000000000100,	NULL,NULL}//	TiStCfg_cgi,		NULL		}
 ,{(11<<16) +	0b0000000000001000,	NULL,NULL}//	TiStCfg_jso,		NULL		}

  // ### Firmware Update ###
 ,{(12<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	FirmwareUd_tpl	}
 ,{(12<<16) +	0b0000000000000100,	NULL,NULL}//	FirmwareUd_cgi,		NULL		}
 ,{(12<<16) +	0b0000000000001000,	NULL,NULL}//	FirmwareUd_jso,		NULL		}

  // ### Redirects ###
 ,{(13<<16) +	0b0000000000000001,	cgiRedirect,	"/WiFi/QConnect.htm"		}
 ,{(14<<16) +	0b0000000000000001,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}
 ,{(14<<16) +	0b0000000000000010,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}

  // ### Secret services ###
 ,{(15<<16) +	0b0000000000100000,	ReadFullFlash_cgi,	NULL		}		//.bin
 ,{(16<<16) +	0b0000000000100000,	NULL,NULL}//	WriteFileSystem_cgi,	NULL		}		//.bin
};



// -------------------------------------------------------------------------------------------------
































/*
 * Implemented Values for Keys
 */

// ESP32_DeVICE_SET_SIG_OUT_EN -> 'SIG_OUT_EN' -> DISABLED|ENABLED
SelectAData ESP32_DeVICE_DisEn[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"DISABLED"},
  {1,"ENABLED"}, 
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

// ESP32_DeVICE_SET_CHANNEL -> 'SSID' -> VISIBLE|HIDDEN
SelectAData ESP32_DeVICE_SSID_VISIBILITY[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"VISIBLE"},
  {1,"HIDDEN"},
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
  {6,"WPA3_PSK"},
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




















/* For: SCDE_cgi
 * Implemented KEYS and assigned readings for this query - analyzed by by http_parser_parse_url()
 * Num should meet enum XX_QueryFields
 * Result is stored in struct SCDE_XX_parsedKVInput
 */
kvParseImplementedKeys_t ESP32_DeVICE_Set_ImplementedKeys[] =
  {//affected_reading | affected_reading			|  CMD
  // Block 1 - func
   { ESP32_DeVICE_R_Name						, "Name" }			// #00
  ,{ ESP32_DeVICE_R_WSAP_Password				, "WSAP_Password" }		// #01
  ,{ ESP32_DeVICE_R_WSAP_RF_Channel				, "WSAP_RF_Channel" }		// #02
  ,{ ESP32_DeVICE_R_WSAP_Maximal_Connections	, "WSAP_Maximal_Connections" }	// #03
  ,{ ESP32_DeVICE_R_WSAP_Authentication_Method	, "WSAP_Authentication_Method" }// #04
  ,{ ESP32_DeVICE_R_WSAP_SSID_Visibility		, "WSAP_SSID" }			// #05
  ,{ ESP32_DeVICE_R_WSAP_Beacon_Interval		, "WSAP_Beacon_Interval" }	// #06

  // Block 2 - func
  ,{ ESP32_DeVICE_R_WSAP_IP_Adress				, "WSAP_IP_Adress" }		// #07
  ,{ ESP32_DeVICE_R_WSAP_Netmask				, "WSAP_Netmask" }		// #08
  ,{ ESP32_DeVICE_R_WSAP_Gateway_Adress			, "WSAP_Gateway_Adress" }	// #09

  // Block 3 - func
  ,{ ESP32_DeVICE_R_WSAP_MAC_Adress				, "WSAP_MAC_Adress" }		// #10

  // Block 4 - func
  ,{ ESP32_DeVICE_R_WSAP_WiFi_Bandwidth			, "WSAP_WiFi_Bandwidth" }	// #11

  // Block 5 - func
  ,{ ESP32_DeVICE_R_WiFi_Country				, "WiFi-Country" }		// #12

  // Block 6 - func
  ,{ ESP32_DeVICE_R_Station_IP_Adress			, "Station_IP_Adress" }		// #13
  ,{ ESP32_DeVICE_R_Station_Netmask				, "Station_Netmask" }		// #14
  ,{ ESP32_DeVICE_R_Station_Gateway_Adress		, "Station_Gateway_Adress" }	// #15

  // Block 7 - func
  ,{ ESP32_DeVICE_R_Station_MAC_Adress			, "Station_MAC_Adress" }	// #16

  // Block 8 - func
  ,{ ESP32_DeVICE_R_Station_Auto_Connect		, "Station_Auto_Connect" }	// #17

  // Block 9 - func
  ,{ ESP32_DeVICE_R_Station_WiFi_Bandwidth		, "Station_WiFi_Bandwidth" }	// #18









  // Block 3 - func
  ,{ ESP32_DeVICE_R_WSAP_DHCPS_Lease_Start_IP	, "WSAP_DHCPS_Lease_Start_IP" }	// #10
  ,{ ESP32_DeVICE_R_WSAP_DHCPS_Lease_End_IP		, "WSAP_DHCPS_Lease_End_IP" }	// #11

  // Block 4 - func
  ,{ ESP32_DeVICE_R_WSAP_DHCPS					, "WSAP_DHCPS" }		// #12

  // Block 6 - func
  ,{ ESP32_DeVICE_R_WSAP						, "WSAP" }			// #14

  // Block 8 - func
  ,{ ESP32_DeVICE_R_Station						, "Station" }			// #18

  // Block 9 - func
  ,{ ESP32_DeVICE_R_Station_DHCP				, "Station_DHCP" }		// #19

  // Block 11 - func
  ,{ ESP32_DeVICE_R_Station_Physical_Mode		, "Station_Physical_Mode" }	// #21

  // Block 12 - func
  ,{ ESP32_DeVICE_R_Station_RSSI				, "Station_Get_RSSI" }		// #22

   // Block 14 - func
  ,{ ESP32_DeVICE_R_TiSt_NTP_Server_Main		, "TiSt_NTP_Server_Main" }	// #24

  // Block 15 - func
  ,{ ESP32_DeVICE_R_TiSt_NTP_Server_Backup_1	, "TiSt_NTP_Server_Backup_1" }	// #25

  // Block 16 - func
  ,{ ESP32_DeVICE_R_TiSt_NTP_Server_Backup_2	, "TiSt_NTP_Server_Backup_2" }	// #26

  // Block 17 - func
  ,{ ESP32_DeVICE_R_TiSt_Time_Zone				, "TiSt_Time_Zone" }		// #27

  // Block 18 - func
  ,{ ESP32_DeVICE_R_TiSt_Time					, "TiSt_Get_Time" }		// #28

  // Block 18 - func
  ,{ 0											, "SoCHW_Restart" }		// #29

  // Block 18 - func
  ,{ 0											, "SoCHW_Factory_Reset_SCDE" }	// #30

  // Block 18 - func
  ,{ 0											, "SoCHW_Factory_Reset_CAPI" }	// #31

 // Block 18 - func
  ,{ 0											, "SoCHW_Factory_Reset" }	// #32

}; // !!! number of implemented keys should be equal with XX_IK_Number_of_keys, LIMIT IS 64 ELEMENTS !!!

















 // reading2_t *p_WSAP_Beacon_Interval;            // link to reading, maintained by this module








/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP'

 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
 /*
int
ESP32_DeVICE_APST2_WSAP_cb_b(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_mode_t *wifi_mode =
        (wifi_mode_t *)option->value;

  uint8_t new_wifi_mode;

  // check if the argument is in the allowed list

  if (SCDEH_GetQueryKeyID((uint8_t*) **stage1Value
        ,strlen(**stage1Value)
        ,(uint8_t*) &new_wifi_mode
        ,&ESP32_DeVICE_DisEn) ) {

    // store the parsed argument
    wifi_mode = ( ((uint8_t) wifi_mode & 0b01) | (new_wifi_mode << 1) ) ;

    // mark argument as sucessfully parsed
    self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP);	

    return 0;	// OK

  } else {

	// argument is not in the allowed list

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "value not accepted", flags);

    return -3;  // error msg -> leadin + usage
  }
}
*/















/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_Beacon_Interval'
 *  Info: - given stage 1 value should be an string 'OPT_INTEGER'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_cb_b(struct argparse *self,
    const struct argparse_option *option, int flags, char ***stage1Value)
{
  // get ptr to reading
  reading2_t *p_WSAP =
        (reading2_t *)option->value;

  // store input-text to raw-readig
  if (!p_WSAP->p_reading_type->p_Store_Raw_Reading_From_Text(p_WSAP,
          **stage1Value)) {

      SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "arg not accepted as value", flags);

      return -3;  // error msg -> leadin + usage
  }

  // mark argument as sucessfully parsed
  self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP);	
  
  // mark reading as touched
  self->affected_readings_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP);	


  return 0;	// OK
}

/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_Beacon_Interval'
 *  Info: - given stage 1 value should be an string 'OPT_INTEGER'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb_b(struct argparse *self,
    const struct argparse_option *option, int flags, char ***stage1Value)
{
  // get ptr to reading
  reading2_t *p_WSAP_Beacon_Interval =
        (reading2_t *)option->value;

  // store input-text to raw-readig
  if (!p_WSAP_Beacon_Interval->p_reading_type->p_Store_Raw_Reading_From_Text(p_WSAP_Beacon_Interval,
          **stage1Value)) {

      SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "arg not accepted as value", flags);

      return -3;  // error msg -> leadin + usage
  }

  // mark argument as sucessfully parsed
  self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval);	
  
  // mark reading as touched
  self->affected_readings_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval);	

  return 0;	// OK
}



























/*
 * ----------------------------- Arg-Parse Stage 2 processing code --------------------------------
 */



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP'

 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_mode_t *wifi_mode =
        (wifi_mode_t *)option->value;

  uint8_t new_wifi_mode;

  // check if the argument is in the allowed list

  if (SCDEH_GetQueryKeyID((uint8_t*) **stage1Value
        ,strlen(**stage1Value)
        ,(uint8_t*) &new_wifi_mode
        ,&ESP32_DeVICE_DisEn) ) {

    // store the parsed argument
    wifi_mode = ( ((uint8_t) wifi_mode & 0b01) | (new_wifi_mode << 1) ) ;

    // mark argument as sucessfully parsed
    self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP);	

    return 0;	// OK

  } else {

	// argument is not in the allowed list

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "value not accepted", flags);

    return -3;  // error msg -> leadin + usage
  }
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_Name_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'Name'
 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_Name_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_ap_config_t *wifi_ap_config =
        (wifi_ap_config_t *)option->value;

  uint8_t ssid_len = strlen(**stage1Value);

  // check the argument for an valid input
  if (!ssid_len) {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "assigned value contains no characters", flags);

    return -3;  // error msg -> leadin + usage
  }

  // check the argument length
  if (ssid_len > 32 ) {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "allows max 32 characters", flags);

    return -3;  // error msg -> leadin + usage
  }

  // store the parsed argument
  wifi_ap_config->ssid_len = ssid_len;
  strncpy(&wifi_ap_config->ssid , **stage1Value, ssid_len);

  // mark argument as sucessfully parsed
  self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_Name);	

  return 0;	// OK
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_Password_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_Password'
 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_Password_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_ap_config_t *wifi_ap_config =
        (wifi_ap_config_t *)option->value;

  uint8_t password_len = strlen(**stage1Value);

  // check the argument length
  if (password_len > 63 ) {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "allows max 63 characters", flags);

    return -3;  // error msg -> leadin + usage
  }
  
  // store the parsed argument
  strcpy(&wifi_ap_config->password , **stage1Value);

  // mark argument as sucessfully parsed
  self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_Password);	

  return 0;	// OK
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_RF_Channel_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_RF_Channel'
 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_RF_Channel_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_ap_config_t *wifi_ap_config =
        (wifi_ap_config_t *)option->value;

  uint8_t new_channel;

  // check if the argument is in the allowed list
  if (SCDEH_GetQueryKeyID((uint8_t*) **stage1Value
        ,strlen(**stage1Value)
        ,(uint8_t*) &new_channel
        ,&ESP32_DeVICE_CHANNEL) ) {

    // store the parsed argument
    wifi_ap_config->channel = new_channel;

	// mark argument as sucessfully parsed
  	self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_RF_Channel);	

    return 0;	// OK

  } else {

	// argument is not in the allowed list
    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "channel not accepted", flags);

    return -3;  // error msg -> leadin + usage
  }
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_Authentication_Method_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_Authentication_Method'
 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_Authentication_Method_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_ap_config_t *wifi_ap_config =
        (wifi_ap_config_t *)option->value;

  uint8_t new_authmode;

  // check if the argument is in the allowed list
  if (SCDEH_GetQueryKeyID((uint8_t*) **stage1Value
        ,strlen(**stage1Value)
        ,(uint8_t*) &new_authmode
        ,&ESP32_DeVICE_AUTH_MODE) ) {

    // store the parsed argument
    wifi_ap_config->authmode = new_authmode;

	// mark argument as sucessfully parsed
  	self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_Authentication_Method);	

    return 0;	// OK

  } else {

	// argument is not in the allowed list
    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "method not accepted", flags);

    return -3;  // error msg -> leadin + usage
  }
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_SSID_Visibility_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_SSID_Visibility'
 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_SSID_Visibility_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_ap_config_t *wifi_ap_config =
        (wifi_ap_config_t *)option->value;

  uint8_t new_ssid_hidden;
  
  // check if the argument is in the allowed list
  if (SCDEH_GetQueryKeyID((uint8_t*) **stage1Value
        ,strlen(**stage1Value)
        ,(uint8_t*) &new_ssid_hidden
        ,&ESP32_DeVICE_SSID_VISIBILITY) ) {

	// store the parsed argument
    wifi_ap_config->ssid_hidden = new_ssid_hidden;

	// mark argument as sucessfully parsed
  	self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility);	

    return 0;	// OK

  } else {

	// argument is not in the allowed list
    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "value not accepted", flags);

    return -3;  // error msg -> leadin + usage
  }
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_Maximal_Connections_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_Maximal_Connections'
 *  Info: - given stage 1 value should be an string 'OPT_INTEGER'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_Maximal_Connections_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,int **stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_ap_config_t *wifi_ap_config =
        (wifi_ap_config_t *)option->value;

  int new_max_connection = **stage1Value;

  // argument to low ?
  if (new_max_connection < 1) {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "must be =>1", flags);

    return -3;  // error msg -> leadin + usage
  }

  // argument to high ?
  if (new_max_connection > 10) {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "max 10 allowed", flags);

    return -3;  // error msg -> leadin + usage
  }

  // store the parsed argument
  wifi_ap_config->max_connection = (uint8_t) new_max_connection;

  // mark argument as sucessfully parsed
  self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections);	

  return 0;	// OK
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'WSAP_Beacon_Interval'
 *  Info: - given stage 1 value should be an string 'OPT_INTEGER'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,int **stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_ap_config_t *wifi_ap_config =
        (wifi_ap_config_t *)option->value;

  int new_beacon_interval = **stage1Value;

  // argument to low ?
  if (new_beacon_interval < 100) {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "maximum is 100 ms", flags);

    return -3;  // error msg -> leadin + usage
  }

  // argument to high ?
  if (new_beacon_interval > 60000) {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "minimum is 60000 ms", flags);

    return -3;  // error msg -> leadin + usage
  }

  // store the parsed argument
  wifi_ap_config->beacon_interval = (uint16_t) new_beacon_interval;

  // mark argument as sucessfully parsed
  self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval);	

  return 0;	// OK
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_Station_cb
 *  Desc: ArgParse Stage 2 parsing callback - for key 'Staton'
 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_Station_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{
  // get ptr to wifi_ap_config_t
  wifi_mode_t *wifi_mode =
        (wifi_mode_t *) option->value;

  uint8_t new_wifi_mode;

  // check if the argument is in the allowed list
  if (SCDEH_GetQueryKeyID((uint8_t*) **stage1Value
        ,strlen(**stage1Value)
        ,(uint8_t*) &new_wifi_mode
        ,&ESP32_DeVICE_DisEn) ) {

	// store the parsed argument
    wifi_mode = ( ((uint8_t) wifi_mode & 0b10) | new_wifi_mode ) ;

    // mark argument as sucessfully parsed
    self->parsed_args_bf |= (1 << ESP32_DeVICE_Set_IK_Station);	

    return 0;	// OK

  } else {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "value not accepted", flags);

	// argument is not in the allowed list
    return -3;  // error msg -> leadin + usage
  }
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Set_H_cb
 *  Desc: Processes the device-specific command line arguments from the get command
 *  Info: Invoked by cmd-line 'Get ESP32Control_Definition.common.Name getArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *getArgsText -> the getArgsText
 *        size_t getArgsTextLen -> length of the getArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_SwITCH_Set_H_cb(struct argparse *self
                ,const struct argparse_option *option
                ,int flags
                ,void *stage1Value)
{
  // allocate mem to prepare ret msg
  self->ret_msg = malloc(sizeof(Entry_String_t));

  // generate concrete error msg - long_name
  self->ret_msg->string.len = asprintf(&self->ret_msg->string.p_char,
	"?, from available: "
	// Built-in Wireless-Service-Access-Point configuration
	"WSAP:uzsuSelectRadio,DISABLED,ENABLED "
	"Name!!max32 "
	"WSAP_Password !!max63 "
	"WSAP_RF_Channel:uzsuSelectRadio,CH_1,CH_2,CH_3,CH_4,CH_5,CH_6,CH_7,CH_8,CH_9,CH_10,CH_11,CH_12,CH_13,CH_14 "
	"WSAP_Authentication_Method:uzsuSelectRadio,OPEN,WEP,WPA_PSK,WPA2_PSK,WPA_WPA2_PSK,WPA2_ENTERPRISE,WPA3_PSK "
	"WSAP_SSID_Visibility:uzsuSelectRadio,VISIBLE,HIDDEN "
	"WSAP_Maximal_Connections:slider,1,1,10 "
	"WSAP_Beacon_Interval:slider,100,10,60000");

  return -3; // because ArgParse_Usage prepares an retMsg
}

  

/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_APST2_WIFI_COUNTRY_cb
 *  Desc: ArgParse Stage 2 parsing callback for key 'WIFI_COUNTRY'
 *  Info: - given stage 1 value should be an string 'OPT_STRING'
 *        - the destination-value should be of type 'uint8_t' 
 *  Para: struct argparse *self -> 
 *        const struct argparse_option *option -> 
 *        int flags ->  
 *        char ***stage1Value -> the input value (as a string)
 *  Rets: int result -> 0=OK, -3=ERROR(leadin error msg only, if any), do not use -2,-1  
 * -------------------------------------------------------------------------------------------------
 */
int
ESP32_DeVICE_APST2_WIFI_COUNTRY_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value)
{

//  wifi_country_t new_WIFI_COUNTRY;
  uint8_t new_WIFI_COUNTRY;

  if (SCDEH_GetQueryKeyID((uint8_t*) **stage1Value
        ,strlen(**stage1Value)
        ,(uint8_t*) &new_WIFI_COUNTRY
        ,&ESP32_DeVICE_WIFI_COUNTRY) ) {

    *(uint8_t *)option->value = new_WIFI_COUNTRY;

    return 0;	// OK

  } else {

    SCDEFn_at_ESP32_DeVICE_M->ArgParse_PrepareLeadinErrorMsgFn(self
          ,option, "value not accepted", flags);

    return -3;  // error msg -> leadin + usage
  }
}






















int
ESP32_DeVICE_help_cb(struct argparse *self, const struct argparse_option *option)
{
  (void)option;

//  ArgParse_Usage(self);

printf("HELP-callback\n");

 return 0;//-3; // because ArgParse_Usage prepares an retMsg
//    exit(0);
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

















/*
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_WiFiEventHandler
 *  Desc: Wifi event handler
 *  Para: System_Event_t *event -> struct with incoming event information for wifi
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
esp_err_t
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
//	esp_wifi_connect();
//	xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

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
//	xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

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

































/*
 * ------------------  helpers provided for module operation starting here ------------------------
 */



// eine für Set und eine für define ????

/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_ProcessKVInputArgs
 *  Desc: Processes the KEY=VALUE input from Args. Information is pre-parsed to *parsedKVInput
 *  Info: .requiredKV -> Keys that should be there / .forbiddenKV -> Keys that are not allowed
 *  Para: Entry_ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> Definition that should get the input
 *        parsedKVInputArgs_t *parsedKVInput -> ptr to allocated mem filled with results from parse
 *        uint8_t *argsText -> the ArgsText
 *        size_t argsTextLen -> length of the ArgsText
 *  Rets: Bool -> TRUE = input parsed successful, FALSE = input parsed, but NOT successful, CANCELED
 * -------------------------------------------------------------------------------------------------
 */
bool
ESP32_DeVICE_ProcessKVInputArgs(Entry_ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition
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
				| (1 << ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility)
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
 // esp_wifi_get_auto_connect(&sta_autoconnect_status);

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
  , ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility			// Bit #05 'WSAP_SSID'  		-> 
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

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility) {

	uint8_t NewSSIDSetting;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].off
		,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].len, &NewSSIDSetting, ESP32_DeVICE_SSID_VISIBILITY)) {

		// save auth mode
//		ap_wifi_config.ap.ssid_hidden = NewSSIDSetting;
printf ("XF");
		// mark affected readings for TX
		affectedReadings |= 
			parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].affectedReadings;

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
				   | (1 << ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility)
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
			| ESP32_DeVICE_R_WSAP_SSID_Visibility
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

	SCDEFn_at_ESP32_DeVICE_M->readings_begin_update_fn((Common_Definition_t*) ESP32_DeVICE_Definition);

// ------------------------------------------------------------------------------------------------- 
/*
	// Reading 'Name'
	if (affectedReadings & ESP32_DeVICE_R_Name) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Name].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Name].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Name].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Name].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'WSAP_Password'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Password) {
*/


/*
WSAP_Password->nameString.length = strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Password].implementedKey);

WSAP_Password->nameString.characters = (const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Password].implementedKey;

WSAP_Password->valueString.characters = argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Password].off;

WSAP_Password->valueString.length = parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Password].len;
*/



/*
		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].off
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_SSID'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_SSID_Visibility) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].len
			,argsText+parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].off
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_Beacon_Interval'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Beacon_Interval) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
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
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Gateway_Adress].off
			,true);
	}

// -------------------------------------------------------------------------------------------------

	// Reading 'Station_MAC_Adress'
	if (affectedReadings & ESP32_DeVICE_R_Station_MAC_Adress) {
/
		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_MAC_Adress].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_MAC_Adress].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_MAC_Adress].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_MAC_Adress].off
			,true);
	}
*/
// -------------------------------------------------------------------------------------------------

  // temp, for reading creation
  String_t reading_name;
  String_t reading_value;

	// Reading 'Station_Auto_Connect'
	if (affectedReadings & ESP32_DeVICE_R_Station_Auto_Connect) {

	    reading_name.len = strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Auto_Connect].implementedKey);
	    reading_name.p_char = (const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Auto_Connect].implementedKey;
	    reading_value.len = parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].len;
	    reading_value.p_char = argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].off;
		
/*	    SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update_fn((Common_Definition_t*) ESP32_DeVICE_Definition
		    ,reading_name
		    ,reading_value
		    ,1,0);
*/


/*
		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update_fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Auto_Connect].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Auto_Connect].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Auto_Connect].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_Auto_Connect].off
			,1,0);*/
	}

// -------------------------------------------------------------------------------------------------

	// add Reading 'Station_WiFi_Bandwidth'
	if (affectedReadings & ESP32_DeVICE_R_Station_WiFi_Bandwidth) {

	    reading_name.len = strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].implementedKey);
	    reading_name.p_char = (const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].implementedKey;
	    reading_value.len = parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].len;
	    reading_value.p_char = argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].off;
		
/*	    SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update_fn((Common_Definition_t*) ESP32_DeVICE_Definition
		    ,reading_name
		    ,reading_value
		    ,1,0);*/

/*
		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update_fn((Common_Definition_t*) ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].implementedKey
			,parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].len
			,argsText + parsedKVInput->keyData_t[ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth].off
			,1,0);*/
	}
//uint8_t *argsText
// -------------------------------------------------------------------------------------------------

  SCDEFn_at_ESP32_DeVICE_M->readings_end_update_fn((Common_Definition_t*) ESP32_DeVICE_Definition, true);
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
  Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
	(Entry_ESP32_DeVICE_Definition_t*) ESP32_DeVICE_Definition;

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

















