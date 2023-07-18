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



#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>



// the Smart-Connected-Device-Engine structures & types ...
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module_s.h"
#include "WebIf_Module.h"

// this Modules structures & types ...
#include "ESP32_DeVICE_Module.h"
#include "ESP32_DeVICE_Get.h"









/* --- was soll das Gerät in FHEM können ---
?, from available:

Caps:noArg 

Block #1
Name  !! max32
WSAP_Password !!max63
WSAP_RF_Channel:uzsuSelectRadio,CH_1,CH_2,CH_3,CH_4,CH_5,CH_6,CH_7,CH_8,CH_9,CH_10,CH_11,CH_12,CH_13,CH_14 
WSAP_Authentication_Method:uzsuSelectRadio,OPEN,WEP,WPA_PSK,WPA2_PSK,WPA_WPA2_PSK
WSAP_SSID_Visibility:uzsuSelectRadio,VISIBLE,HIDDEN 
WSAP_Maximal_Connections:slider,1,1,10
WSAP_Beacon_Interval:slider,100,10,60000

WSAP_IP_Adress 
WSAP_Netmask 
WSAP_Gateway_Adress 

WSAP_DHCPS:uzsuSelectRadio,ENABLED,DISABLED 
WSAP_DHCPS_Lease_Start_IP 
WSAP_DHCPS_Lease_End_IP 

WSAP_MAC_Adress 
WSAP_State:uzsuSelectRadio,ENABLED,DISABLED 
Station_IP_Adress 
Station_Netmask 
Station_Gateway_Adress 
Station_State:uzsuSelectRadio,ENABLED,DISABLED 
Station_DHCP:uzsuSelectRadio,ENABLED,DISABLED 
Station_Auto_Connect:uzsuSelectRadio,ENABLED,DISABLED 
Station_Physical_Mode:uzsuSelectRadio,802.11b,802.11g,802.11n 
Station_Get_RSSI:noArg 
Station_MAC_Adress 
TiSt_NTP_Server_Main 
TiSt_NTP_Server_Backup_1 
TiSt_NTP_Server_Backup_2 
TiSt_Time_Zone 
TiSt_Get_Time 
SoCHW_Restart:noArg 
SoCHW_Factory_Reset_SCDE:noArg 
SoCHW_Factory_Reset_CAPI:noArg 
SoCHW_Factory_Reset:noArg 
*/



/** Get - New Version (2)
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Get V2
 *  Desc: Implements the device-specific get function. Is used to get information from device.
 *  Info: Typically invoked by cmd-line 'get ESP32Control_Definition.common.Name getArgs'
 *             aa bbb ccc "dd d" ee         <- original string
 *             ---------------------
 *             aa0bbb0ccc00dd d00ee0        <- transformed string
 *             |  |   |    |     |
 *   argv[0] __/  /   /    /     /
 *   argv[1] ____/   /    /     /
 *   argv[2] _______/    /     /
 *   argv[3] ___________/     /
 *   argv[4] ________________/ 
 *   Note: 1 args inside '""' are not splitted !
 *  Para: Entry_Definition_t* p_definition -> the ESP32_DeVICE_Definition that should be processed
 *        char **argv                      -> the arguments *argv[x]
 *        int argc                         -> the number of arguments (x)
 *  Rets: Entry_String_t* p_retMsg         -> response message as string, NULL=OK, without text
 * -------------------------------------------------------------------------------------------------
 */
Entry_String_t*
ESP32_DeVICE_GetV2(Entry_Definition_t* p_Definition
                ,char **argv
                ,int argc)
{
  // for Fn response msg
  Entry_String_t *p_retMsg = NULL;

  // make common ptr to modul specific ptr
  Entry_ESP32_DeVICE_Definition_t* p_ESP32_DeVICE_Definition =
	(Entry_ESP32_DeVICE_Definition_t*) p_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(p_Definition->name
	,p_Definition->nameLen
	,5
	,"GetV2Fn of Module '%.*s' is called for Definition '%.*s'. Definition shall respond to GET arguments '%s'."
	,p_Definition->module->provided->typeNameLen
	,p_Definition->module->provided->typeName
	,p_Definition->nameLen
	,p_Definition->name
	,*argv);
  #endif	

// -------------------------------------------------------------------------------------------------
// 1. Step: Create backup structures, if required mirror current values from SDK
// -------------------------------------------------------------------------------------------------

  // block #01 get 'wifi_ap_config' (WSAP-Configuration)
  wifi_ap_config_t wifi_ap_config;
  esp_wifi_get_config (WIFI_IF_AP, &wifi_ap_config);

// -------------------------------------------------------------------------------------------------

  // block #02 get 'tcpip_adapter_ip_info_t' (WSAP-IP-Settings)
  tcpip_adapter_ip_info_t ap_ip_info;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip_info);

/*				 ( ESP32_DeVICE_R_WSAP_IP_Adress
				    | ESP32_DeVICE_R_WSAP_Netmask
				    | ESP32_DeVICE_R_WSAP_Gateway_Adress) ) {*/

// -------------------------------------------------------------------------------------------------


/*
  ,ESP32_DeVICE_R_WSAP
  ,ESP32_DeVICE_R_Station*/



  // block #?? get 'wifi_mode_t' (WIFI WSAP+STA Configuration)
  wifi_mode_t wifi_mode;
  esp_wifi_get_mode(&wifi_mode);
 




  // block #3 get current Service AP MAC Adress
  uint8_t ap_mac_addr[8];
  esp_wifi_get_mac(WIFI_IF_AP,(uint8_t *) &ap_mac_addr);

  // block #4 get the bandwith of the Service Access Point
  wifi_bandwidth_t ap_wifi_bandwidth;
  esp_wifi_get_bandwidth(WIFI_IF_AP, &ap_wifi_bandwidth);

  // block #5 get the WiFi Country
//  wifi_country_t country;
//  esp_wifi_get_country(&country);

  // block #6 get current Station IP Settings
  tcpip_adapter_ip_info_t sta_ip_info;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip_info);

  // block #7 get current Station MAC Adress
  uint8_t sta_mac_addr[8];
  esp_wifi_get_mac(WIFI_IF_STA, (uint8_t *) &sta_mac_addr);

  // block #8 get current Station Auto-Connect-Status
  bool sta_autoconnect_status;
//  esp_wifi_get_auto_connect(&sta_autoconnect_status);

 // block #9 mirror 'wifi_bandwidth_t' (WIFI STA Configuration)
  wifi_bandwidth_t wifiBandwidthSTA;
  esp_wifi_get_bandwidth(WIFI_IF_STA, &wifiBandwidthSTA);

 		//(1 << ESP32_DeVICE_R_WSAP_WiFi_Bandwidth) ) {




//#3
  // block #6 get current Station Settings
  wifi_config_t sta_wifi_config;
  esp_wifi_get_config(WIFI_IF_STA, &sta_wifi_config);

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








// -------------------------------------------------------------------------------------------------
// 2. Step: Parse KEYs into mirrored data, including check if the given values match the requirements
// -------------------------------------------------------------------------------------------------





















































  // block #8 get current Station Auto-Connect-Status
//  bool sta_autoconnect_status;
 // esp_wifi_get_auto_connect(&sta_autoconnect_status);

  // block #5 get the WiFi Country
uint8_t country =2;
//  wifi_country_t country;
//  esp_wifi_get_country(&country);

  printf("WIFI_COUNTRY=%s"
         ,SCDE_GetDesc(ESP32_DeVICE_WIFI_COUNTRY, country));




#define PERM_READ  (1<<0)
#define PERM_WRITE (1<<1)
#define PERM_EXEC  (1<<2)


  // opt-group 1
  int force = 0;
  int test = 0;
  const char *path = NULL;
  int int_num = 0;
  float flt_num = 0.f;

  // opt-group 2
  int perms = 0;

  // opt-group 3
  float flt_test2 = 0.f;
  const char *path_test2 = NULL;
















  // our supplied argparse struct
  struct argparse argparse;

  // various possible usages. no,1,2 ... - NULL marks end of list
  static const char *const usages[] = {	//SET_usages
	"set definition [options] [[--] args]",
  	"set definition [options]",
  	NULL,
 };

  struct argparse_option options[] = { //SET_options
//	OPT_HELP(), // is 'h', "help", NULL, "show this help message and exit", argparse_help_cb, 0, OPT_NONEG)
	OPT_BOOLEAN	 ('h', "help", 						NULL, 			  	"show this help message",   	argparse_help_cb,       							0, OPT_NONEG),
    OPT_BOOLEAN  ('?', "?",                         NULL,             	"hctrl/fhem gets help",     	&ESP32_SwITCH_Set_H_cb, 							0, OPT_NONEG),  //noneg????

    OPT_GROUP  ("Built-in Wireless-Service-Access-Point configuration"),
    OPT_STRING   ('x', "WSAP",                      &wifi_mode,      	"DISABLED,ENABLED",          	&ESP32_DeVICE_APST2_WSAP_cb, 						0, 0),
    OPT_STRING   ('n', "Name",                      &wifi_ap_config, 	"characters, max.31",  			&ESP32_DeVICE_APST2_Name_cb, 						0, 0),
    OPT_STRING   ('p', "WSAP_Password",             &wifi_ap_config, 	"characters, max.63",         	&ESP32_DeVICE_APST2_WSAP_Password_cb, 				0, 0),
    OPT_STRING   ('c', "WSAP_RF_Channel",           &wifi_ap_config, 	"CH_1 - CH_14",		           	&ESP32_DeVICE_APST2_WSAP_RF_Channel_cb, 			0, 0),
    OPT_STRING   ('a', "WSAP_Authentication_Method",&wifi_ap_config, 	"OPEN,WPA2_PSK, ... try ?",		&ESP32_DeVICE_APST2_WSAP_Authentication_Method_cb, 	0, 0),
    OPT_STRING   ('v', "WSAP_SSID_Visibility",      &wifi_ap_config, 	"VISIBLE,HIDDEN",              	&ESP32_DeVICE_APST2_WSAP_SSID_Visibility_cb, 		0, 0),
    OPT_INT32    ('m', "WSAP_Maximal_Connections",  &wifi_ap_config, 	"1 - 10", 					 	&ESP32_DeVICE_APST2_WSAP_Maximal_Connections_cb, 	0, 0),
    OPT_INT32    ('i', "WSAP_Beacon_Interval",      &wifi_ap_config,	"100 - 60000",			       	&ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb, 		0, 0),

    OPT_GROUP  ("Device-to-Station connection configuration"),
    OPT_STRING   ('y', "Station",                   &wifi_mode,   		"ENABLED,DISABLED",            	&ESP32_DeVICE_APST2_Station_cb, 					0, 0),
    OPT_STRING   ('c', "WIFI_COUNTRY",   			&country,       	"station wifi country", 		&ESP32_DeVICE_APST2_WIFI_COUNTRY_cb, 				0, 0),
    OPT_BOOLEAN  ('f', "Station_Auto_Connect",  	&sta_autoconnect_status,      "force to do", 		NULL, 												0, 0),
    OPT_BOOLEAN  ('f', "force",  					&force,      		"force to do", 					NULL, 												0, 0),
    OPT_BOOLEAN  ('t', "test",   					&test,       		"test only", 					NULL, 												0, 0),
    OPT_STRING   ('p', "path",   					&path,       		"path to read", 				NULL, 												0, 0),
    OPT_INT32    ('i', "int",    					&int_num,    		"selected integer", 			NULL, 												0, 0),
    OPT_FLOAT    ('s', "float",  					&flt_num,    		"selected float", 				NULL, 												0, 0),

    OPT_GROUP  ("Debug testing configuration"),
    OPT_BIT      ( 0 , "read",   					&perms,      		"read perm", 					NULL, 												PERM_READ, OPT_NONEG),
    OPT_BIT      ( 0 , "write",  					&perms,      		"write perm", 					NULL, 												PERM_WRITE, 0),
    OPT_BIT      ( 0 , "exec",   					&perms,      		"exec perm", 					NULL, 												PERM_EXEC, 0),

    OPT_GROUP  ("Time configuration"),
    OPT_FLOAT    ('x', "float2", 					&flt_test2,  		"selected float2", 				NULL, 												0, 0),
    OPT_STRING   ('y', "path2",  					&path_test2, 		"path to read2", 				NULL, 												0, 0),
    OPT_END(),
  };

  // init the arg-parse Fn
  p_retMsg = SCDEFn_at_ESP32_DeVICE_M->ArgParse_InitFn(&argparse
	,options
	,usages
	,0	// flags: ARGPARSE_STOP_AT_NON_OPTION ?
	,"\nTo SET the configuration of the ESP32 DeVICE"										// prolog usage
	,"\nUse command help ESP32_DeVICE to get the detailed description of the arguments.");	// epilog usage
  if (p_retMsg) goto error2;	// got an error-msg text -> send it to the user!

  // call the parse Fn
  p_retMsg = SCDEFn_at_ESP32_DeVICE_M->ArgParse_ParseFn(&argparse, argc, argv);
  if (p_retMsg) goto error2;	// got an error-msg text -> send it to the user!

  // wie genau?? mem leak?
  // get the resulting args-no from arg parse fn
  argc = argparse.cpidx + argparse.argc;


  if (argc < 0) printf("An error occured? argc: %d\n", argc);







  // deep debug
  printf("\nDeep debugging, parsed args bf: %lld, listing parsed values:\n",
  	 argparse.parsed_args_bf);

  // debugout ssid aka name, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_Name) ) {
  	printf("parsed name=%.*s\n"
		,wifi_ap_config.ssid_len
		,wifi_ap_config.ssid);
  }

  // debugout WSAP_Password, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_WSAP_Password) ) {
  	printf("parsed WSAP_Password=%s\n"
		,wifi_ap_config.password);
  }

  // debugout WSAP_RF_Channel, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_WSAP_RF_Channel) ) {
  	printf("parsed WSAP_RF_Channel=%d\n"
		,wifi_ap_config.channel);
  }

  // debugout WSAP_Authentication_Method, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_WSAP_Authentication_Method) ) {
  	printf("WSAP_Authentication_Method=%d\n"
		,wifi_ap_config.authmode);
  }

  // debugout WSAP_SSID_Visibility, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility) ) {
  	printf("WSAP_SSID_Visibility=%d\n"
		,wifi_ap_config.ssid_hidden);
  }

  // debugout WSAP_Maximal_Connections, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections) ) {
  	printf("WSAP_Maximal_Connections=%d\n"
		,wifi_ap_config.max_connection);
  }

  // debugout WSAP_Beacon_Interval, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval) ) {
  	printf("WSAP_Beacon_Interval=%d\n"
		,wifi_ap_config.beacon_interval);
  }

 // debugout WSAP, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_WSAP) ) {
  	printf("WSAP=%d\n"
		,( (uint8_t) wifi_mode & 0b10) >>1 );
  }
  
 // debugout Station, if found ...
  if (argparse.parsed_args_bf & (1 << ESP32_DeVICE_Set_IK_Station) ) {
  	printf("Station=%d\n"

		,((uint8_t) wifi_mode & 0b01) );
  }






/*
Was wir brauchen:

  // set enabled keys -> keys that are currently enabled (for devices that change behviour)
  parsedKVInput->enabledKVBF = 0;

  // set required Keys -> Keys that should be there in this request
  parsedKVInput->requiredKVBF = ( (1 << ESP32_SwITCH_SET_GPIO)
			        | (1 << ESP32_SwITCH_SET_BLOCK)
			        | (1 << ESP32_SwITCH_SET_CHANNEL)
			        | (1 << ESP32_SwITCH_SET_TIMER)
			        | (1 << ESP32_SwITCH_SET_DUTY)
			        | (1 << ESP32_SwITCH_SET_HPOINT)
			        | (1 << ESP32_SwITCH_SET_SIG_OUT_EN)
			        | (1 << ESP32_SwITCH_SET_IDLE_LV)
			        | (1 << ESP32_SwITCH_SET_RESOLUTION)
			        | (1 << ESP32_SwITCH_SET_TICK_SOURCE)
			        | (1 << ESP32_SwITCH_SET_FREQ_HZ) );

  // set forbidden Keys -> Keys that are not allowed in this request
  parsedKVInput->forbiddenKVBF = 0;

+ timestamp für readings (reading blocks)
*/










  // opt-group 1
  if (force != 0)
        printf("force: %d\n", force);
  if (test != 0)
        printf("test: %d\n", test);
  if (path != NULL)
        printf("path: %s\n", path);
  if (int_num != 0)
        printf("int_num: %d\n", int_num);
  if (flt_num != 0)
        printf("flt_num: %g\n", flt_num);

  // opt-group 2
  if (perms) {
        printf("perms: %d\n", perms);
  }
 
  // opt-group 3
  if (flt_test2 != 0)
        printf("flt_test2: %g\n", flt_test2);

  if (path_test2 != NULL)
        printf("path_test2: %s\n", path_test2);

  // the rest ..
  if (argc > 0) { //!=
        printf("other args: %d\n", argc);
        int i;
        for (i = 0; i < argc; i++) {
            printf("argv[%d]: %s\n", i, *(argv + i));
        } 
  }

// wie kann festgestellt werden ob eine Eingabe erfolgte z.B. float 0 ?

  printf("WIFI_COUNTRY=%s"
         ,SCDE_GetDesc(ESP32_DeVICE_WIFI_COUNTRY, country));

// ------------------------------------------------------------------------------------------------












  // remember the readings affected by the parsing process
  // on value change can affect multipe readings -> bitfield
  uint32_t affectedReadings = 0;

 // debugview all readings
  affectedReadings = 0xffffffff;

 printf ("AffectedReadings:%d", affectedReadings);
 
 
// -------------------------------------------------------------------------------------------------
// 5. Step: update the Readings according to 'affectedReadings' - if any ...
// -------------------------------------------------------------------------------------------------

  // update the Readings according to 'affectedReadings' - if any ...
  if (affectedReadings) {
  
    // temp, for reading creation
    String_t reading_name;
    String_t reading_value;

	SCDEFn_at_ESP32_DeVICE_M->readings_begin_update_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition);

// ------------------------------------------------------------------------------------------------- 

	// Reading 'Name'
	if (affectedReadings & ESP32_DeVICE_R_Name) {

	    reading_name.len = strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Name].implementedKey);
	    reading_name.p_char = (const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station_Auto_Connect].implementedKey;
	    reading_value.len = wifi_ap_config.ssid_len;
	    reading_value.p_char = wifi_ap_config.ssid;
/*		
	    SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
		    ,reading_name
		    ,reading_value
		    ,true,0);
*/

/*
		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Name].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Name].implementedKey
			,wifi_ap_config.ssid_len
			,wifi_ap_config.ssid
			,true);*/
	}

// -------------------------------------------------------------------------------------------------
/*
	// Reading 'WSAP_Password'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Password) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Password].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Password].implementedKey
			,strlen((char *) wifi_ap_config.password)
			,wifi_ap_config.password
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_RF_Channel'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_RF_Channel) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_RF_Channel].implementedKey
			,strlen(SCDE_GetDesc(ESP32_DeVICE_CHANNEL, wifi_ap_config.channel))
			,(const uint8_t *) SCDE_GetDesc(ESP32_DeVICE_CHANNEL, wifi_ap_config.channel)
			,true);
	}
	
// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_Maximal_Connections'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Maximal_Connections) {

		String_t WSAP_Maximal_Connections;

		// create temporary text
		WSAP_Maximal_Connections.len = asprintf(&WSAP_Maximal_Connections.p_char,"%d"
			,wifi_ap_config.max_connection);

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections].implementedKey
			,WSAP_Maximal_Connections.len
			,WSAP_Maximal_Connections.p_char
			,true);
			
		free(WSAP_Maximal_Connections.p_char);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_Authentication_Method'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Authentication_Method) {

 		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Authentication_Method].implementedKey
			,strlen(SCDE_GetDesc(ESP32_DeVICE_AUTH_MODE, wifi_ap_config.ssid_hidden))
			,(const uint8_t *) SCDE_GetDesc(ESP32_DeVICE_AUTH_MODE, wifi_ap_config.ssid_hidden)
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_SSID_Visibility'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_SSID_Visibility) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility].implementedKey
			,strlen(SCDE_GetDesc(ESP32_DeVICE_SSID_VISIBILITY, wifi_ap_config.authmode))
			,(const uint8_t *) SCDE_GetDesc(ESP32_DeVICE_SSID_VISIBILITY, wifi_ap_config.authmode)
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP_Beacon_Interval'
	if (affectedReadings & ESP32_DeVICE_R_WSAP_Beacon_Interval) {

		String_t WSAP_Beacon_Interval;

		// create temporary text
		WSAP_Beacon_Interval.len = asprintf(&WSAP_Beacon_Interval.p_char,"%d"
			,wifi_ap_config.beacon_interval);

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval].implementedKey
			,WSAP_Beacon_Interval.len
			,WSAP_Beacon_Interval.p_char
			,true);
			
		free(WSAP_Beacon_Interval.p_char);
	}
	
// ------------------------------------------------------------------------------------------------- 

	// Reading 'WSAP'
	if (affectedReadings & ESP32_DeVICE_R_WSAP) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_WSAP].implementedKey
			,strlen(SCDE_GetDesc(ESP32_DeVICE_DisEn, (( (uint8_t) wifi_mode & 0b10) >>1 ) ))
			,(const uint8_t *) SCDE_GetDesc(ESP32_DeVICE_DisEn, (((uint8_t) wifi_mode & 0b10)>>1)  )
			,true);
	}

// ------------------------------------------------------------------------------------------------- 

	// Reading 'Station'
	if (affectedReadings & ESP32_DeVICE_R_Station) {

		// create / update reading
		SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update2_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition
			,strlen((const char *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station].implementedKey)
			,(const uint8_t *) ESP32_DeVICE_Set_ImplementedKeys[ESP32_DeVICE_Set_IK_Station].implementedKey
			,strlen(SCDE_GetDesc(ESP32_DeVICE_DisEn, ((uint8_t) wifi_mode & 0b01) ))
			,(const uint8_t *) SCDE_GetDesc(ESP32_DeVICE_DisEn, ((uint8_t) wifi_mode & 0b01) )
			,true);
	}
*/
// -------------------------------------------------------------------------------------------------

  SCDEFn_at_ESP32_DeVICE_M->readings_end_update_fn((Common_Definition_t*) p_ESP32_DeVICE_Definition, true);
  }



















// ------------------------------------------------------------------------------------------------

error2:

// ------------------------------------------------------------------------------------------------

//error1:

  return p_retMsg;
}
















/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Get
 *  Desc: Processes the device-specific command line arguments from the get command
 *  Info: Invoked by cmd-line 'Get ESP32Control_Definition.common.Name getArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *getArgsText -> the getArgsText
 *        size_t getArgsTextLen -> length of the getArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_DeVICE_Get(Common_Definition_t* Common_Definition
                ,uint8_t *getArgsText
                ,size_t getArgsTextLen)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
	(Entry_ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,5
	,"GetFn of Module '%.*s' is called for Definition '%.*s'. Definition shall respond to GET arguments '%.*s'."
	,Common_Definition->module->provided->typeNameLen
	,Common_Definition->module->provided->typeName
	,Common_Definition->nameLen
	,Common_Definition->name
	,getArgsTextLen
	,getArgsText);
  #endif	

// -------------------------------------------------------------------------------------------------

  // note: call to _Get Fn is blocked with getArgsTextLen = 0 !

/*
  // note: '?' KEY is not emplemented -> the error-msg is the correct answer !

  // getArgsText '?' -> respond with help
  if ( (getArgsTextLen > 0) && (*getArgsText == 'X') ) {

	// set start of possible Type-Name
	const uint8_t *tempTxt = getArgsText + 1;

	// a seek-counter
	int i = 1;

	// skip spaces after '?' (search for more ...)
	while( (i < getArgsTextLen) && (*tempTxt == ' ') ) {i++;tempTxt++;}


	// only spaces after '?' -> answer with get-Capabilities
	if (i == getArgsTextLen) {

		// response with error text
		// alloc mem for retMsg
		retMsg = malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"requested ? '%.*s' ! This will be removed..."
			,getArgsTextLen
			,getArgsText);

		return retMsg;

		}
	// '? + X' here !!! -> normal parse -> will throw error in next step
	}
*/
// ------------------------------------------------------------------------------------------------

  char **argv;
  int argc;

  // split 'getArgsText' the default way
  argv = SCDEFn_at_ESP32_DeVICE_M->ArgParse_SplitArgsToAllocatedMemFn(&argc, getArgsText, getArgsTextLen);

  retMsg = (strTextMultiple_t*) ESP32_DeVICE_GetV2(Common_Definition
                ,argv, argc);


  // ArgParseFn has allocated mem. Free it.
  SCDEFn_at_ESP32_DeVICE_M->ArgParse_FreeSplittedArgsInAllocatedMemFn(argv);

// ------------------------------------------------------------------------------------------------

  return retMsg;
}



