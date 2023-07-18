﻿// SCDE-Module ESP32_DeVICE

#ifndef ESP32_DEVICE_MODULE_H
#define ESP32_DEVICE_MODULE_H

// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef ESP32_DeVICE_Module_DBG  
#define ESP32_DeVICE_Module_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------

// stores the Root Data of the Smart Connected Devices Engine - at/for this Module
SCDERoot_t* SCDERoot_at_ESP32_DeVICE_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_ESP32_DeVICE_M;

// -------------------------------------------------------------------------------------------------

//
//ESP32_DeVICE_Fn_t ESP32_DeVICE_Fn;

//
ProvidedByModule_t ESP32_DeVICE_ProvidedByModule;

//
const WebIf_ActiveResourcesDataA_t ESP32_DeVICE_ActiveResourcesDataA_forWebIf[];

//
const WebIf_ActiveResourcesDataB_t ESP32_DeVICE_ActiveResourcesDataB_forWebIf[];

// -------------------------------------------------------------------------------------------------





/*
#include "freertos/event_groups.h"

static EventGroupHandle_t wifi_event_group;

static const int CONNECTED_BIT;
*/




esp_err_t ESP32_DeVICE_WiFiEventHandler(void *ctx, system_event_t *event);

//static void Initialize_SNTP(void);

//static void Obtain_Time(void);











/**
 * For Type: ESP32_DeVICE
 * Implemented-Keys (Args input via Key=value) for SetFn input (MAX 64!)
 * KEYs-Field-Set for Feature specific query. Will be generated by SCDEH_ParseStrToparsedKVInput()
 * parsed result is stored in ESP32_DeVICE_parsedKVInput_t
 */
enum ESP32_DeVICE_Set_IK {				// Bit #XX for debugging

  // Block 1 - func
    ESP32_DeVICE_Set_IK_Name		= 0				// Bit #00 'Name'						-> 
  , ESP32_DeVICE_Set_IK_WSAP_Password				// Bit #01 'WSAP_Password'				-> 
  , ESP32_DeVICE_Set_IK_WSAP_RF_Channel		        // Bit #02 'WSAP_RF_Channel' 			-> 
  , ESP32_DeVICE_Set_IK_WSAP_Maximal_Connections	// Bit #03 'WSAP_Maximal_Connections'  	-> 
  , ESP32_DeVICE_Set_IK_WSAP_Authentication_Method	// Bit #04 'WSAP_Authentication_Method' -> 
  , ESP32_DeVICE_Set_IK_WSAP_SSID_Visibility		// Bit #05 'WSAP_SSID'  				-> 
  , ESP32_DeVICE_Set_IK_WSAP_Beacon_Interval		// Bit #06 'WSAP_Beacon_Interval'  		-> 

   // Block 2 - func
  , ESP32_DeVICE_Set_IK_WSAP_IP_Adress				// Bit #07 'WSAP_IP_Adress'  		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Netmask				// Bit #08 'WSAP_Netmask'  		-> 
  , ESP32_DeVICE_Set_IK_WSAP_Gateway_Adress			// Bit #09 'WSAP_Gateway_Adress'  	-> 

   // Block 3 - func
  , ESP32_DeVICE_Set_IK_WSAP_MAC_Adress		        // Bit #10 'WSAP_MAC_Adress'  		-> 

   // Block 4 - func
  , ESP32_DeVICE_Set_IK_WSAP_WiFi_Bandwidth			// Bit #11 'WSAP_WiFi_Bandwidth'	-> 

   // Block 5 - func
  , ESP32_DeVICE_Set_IK_WiFi_Country				// Bit #12 'WiFi_Country'		-> 

   // Block 6 - func
  , ESP32_DeVICE_Set_IK_Station_IP_Adress			// Bit #13 'Station_IP_Adress'  	-> 
  , ESP32_DeVICE_Set_IK_Station_Netmask		        // Bit #14 'Station_Netmask'  		-> 
  , ESP32_DeVICE_Set_IK_Station_Gateway_Adress		// Bit #15 'Station_Gateway_Adress'  	-> 

   // Block 7 - func
  , ESP32_DeVICE_Set_IK_Station_MAC_Adress			// Bit #16 'Station_MAC_Adress'		-> 

   // Block 8 - func
  , ESP32_DeVICE_Set_IK_Station_Auto_Connect		// Bit #17 'Station_Auto_Connect'	-> 

   // Block 9 - func
  , ESP32_DeVICE_Set_IK_Station_WiFi_Bandwidth		// Bit #18 'Station_WiFi_Bandwidth'	-> 






   // Block 3 - func
  , ESP32_DeVICE_Set_IK_WSAP_DHCPS_Lease_Start_IP	// Bit #10 'WSAP_DHCPS_Lease_Start_IP'  -> 
  , ESP32_DeVICE_Set_IK_WSAP_DHCPS_Lease_End_IP	    // Bit #11 'WSAP_DHCPS_Lease_End_IP'  	-> 

   // Block 4 - func
  , ESP32_DeVICE_Set_IK_WSAP_DHCPS					// Bit #12 'WSAP_DHCPS'  		-> 


   // Block 6 - func
  , ESP32_DeVICE_Set_IK_WSAP						// Bit #14 'WSAP'  			-> 


   // Block 8 - func
  , ESP32_DeVICE_Set_IK_Station			       		// Bit #18 'Station'  			-> 

   // Block 9 - func
  , ESP32_DeVICE_Set_IK_Station_DHCP				// Bit #19 'Station_DHCP'		-> 


   // Block 11 - func
  , ESP32_DeVICE_Set_IK_Station_Physical_Mode		// Bit #21 'Station_Physical_Mode'	-> 

   // Block 12 - func
  , ESP32_DeVICE_Set_IK_Station_Get_RSSI			// Bit #22 'Station_Get_RSSI'		-> 




   // Block 14 - func
  , ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Main		// Bit #24 'TiSt_NTP_Server_Main'	-> 

   // Block 15 - func
  , ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_1	// Bit #25 'TiSt_NTP_Server_Backup_1'	-> 

   // Block 16 - func
  , ESP32_DeVICE_Set_IK_TiSt_NTP_Server_Backup_2	// Bit #26 'TiSt_NTP_Server_Backup_2'	-> 

   // Block 17 - func
  , ESP32_DeVICE_Set_IK_TiSt_Time_Zone				// Bit #27 'TiSt_Time_Zone'		-> 

   // Block 18 - func
  , ESP32_DeVICE_Set_IK_TiSt_Get_Time				// Bit #28 'TiSt_Get_Time'		-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Restart				// Bit #29 'SoCHW_Restart'		-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset_SCDE	// Bit #30 'SoCHW_Factory_Reset_SCDE'	-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset_CAPI	// Bit #31 'SoCHW_Factory_Reset_CAPI'	-> 

   // Block xx - func
  , ESP32_DeVICE_Set_IK_SoCHW_Factory_Reset			// Bit #32 'SoCHW_Factory_Reset'	-> 

   // helper to get number of implemented keys
  , ESP32_DeVICE_Set_NUMBER_OF_IK					// Bit #34 MAX 64 IMPLEMENTED !
};



/* For Type: ESP32_DeVICE
 * Implemented readings (MAX 32!)
 * Can be assigned to Implemented Keys, if affected
 */
enum ESP32_DeVICE_Readings {				 // Bit #XX for debugging
  // Internals - TiSt-Sharing - Block 1 - func
   ESP32_DeVICE_R_Name							= (1<<0) // Bit #01 'Name'			-> 
  ,ESP32_DeVICE_R_WSAP_Password					= (1<<1) // Bit #01 'WSAP_Password'		-> 
  ,ESP32_DeVICE_R_WSAP_RF_Channel				= (1<<2) // Bit #01 'WSAP_RF_Channel'		-> 
  ,ESP32_DeVICE_R_WSAP_Maximal_Connections		= (1<<3) // Bit #01 'WSAP_Maximal_Connections'	-> 
  ,ESP32_DeVICE_R_WSAP_Authentication_Method	= (1<<4) // Bit #01 'WSAP_Authentication_Method'-> 
  ,ESP32_DeVICE_R_WSAP_SSID_Visibility						= (1<<5) // Bit #01 'WSAP_SSID'			->
  ,ESP32_DeVICE_R_WSAP_Beacon_Interval			= (1<<6) // Bit #01 'WSAP_Beacon_Interval'	-> 

  // Internals - TiSt-Sharing - Block 2 - func
  ,ESP32_DeVICE_R_WSAP_IP_Adress				= (1<<7) // Bit #02 'WSAP_IP_Adress'		-> 
  ,ESP32_DeVICE_R_WSAP_Netmask					= (1<<8) // Bit #02 'WSAP_Netmask'		-> 
  ,ESP32_DeVICE_R_WSAP_Gateway_Adress			= (1<<9) // Bit #02 'WSAP_Gateway_Adress'	-> 

  // Internals - TiSt-Sharing - Block 3 - func
  ,ESP32_DeVICE_R_WSAP_MAC_Adress				= (1<<10) // Bit #04 'WSAP_MAC_Adress'		-> 

  // Internals - TiSt-Sharing - Block 4 - func
  ,ESP32_DeVICE_R_WSAP_WiFi_Bandwidth			= (1<<11) // Bit #07 'WSAP_WiFi_Bandwidth'	-> 

  // Internals - TiSt-Sharing - Block 5 - func
  ,ESP32_DeVICE_R_WiFi_Country					= (1<<12) // Bit #07 'WiFi_Country'		-> 

  // Internals - TiSt-Sharing - Block 6 - func
  ,ESP32_DeVICE_R_Station_IP_Adress				= (1<<13) // Bit #06 'Station_IP_Adress'	-> 
  ,ESP32_DeVICE_R_Station_Netmask				= (1<<14) // Bit #06 'Station_Netmask'		-> 
  ,ESP32_DeVICE_R_Station_Gateway_Adress		= (1<<15) // Bit #06 'Station_Gateway_Adress'	-> 

  // Internals - TiSt-Sharing - Block 7 - func
  ,ESP32_DeVICE_R_Station_MAC_Adress			= (1<<16) // Bit #11 'Station_MAC_Adress'	-> 

  // Internals - TiSt-Sharing - Block 8 - func
  ,ESP32_DeVICE_R_Station_Auto_Connect			= (1<<16) // Bit #09 'Station_Auto_Connect'	-> 

  // Internals - TiSt-Sharing - Block 9 - func
  ,ESP32_DeVICE_R_Station_WiFi_Bandwidth		= (1<<16) // Bit #07 'Station_WiFi_Bandwidth'	-> 






  // Internals - TiSt-Sharing - Block 3 - func
  ,ESP32_DeVICE_R_WSAP_DHCPS_Lease_Start_IP		= (1<<2) // Bit #03 'WSAP_DHCPS_Lease_Start_IP'	-> 
  ,ESP32_DeVICE_R_WSAP_DHCPS_Lease_End_IP		= (1<<2) // Bit #03 'WSAP_DHCPS_Lease_End_IP'	-> 
  ,ESP32_DeVICE_R_WSAP_DHCPS					= (1<<2) // Bit #03 'WSAP_DHCPS'		-> 


  // Internals - TiSt-Sharing - Block 5 - func
  ,ESP32_DeVICE_R_WSAP							= (1<<4) // Bit #05 'WSAP'			-> 


  // Internals - TiSt-Sharing - Block 7 - func
  ,ESP32_DeVICE_R_Station						= (1<<6) // Bit #07 'Station'			-> 

  // Internals - TiSt-Sharing - Block 8 - func
  ,ESP32_DeVICE_R_Station_DHCP					= (1<<7) // Bit #08 'Station_DHCP'		-> 


  // Internals - TiSt-Sharing - Block 10 - func
  ,ESP32_DeVICE_R_Station_Physical_Mode			= (1<<9) // Bit #10 'Station_Physical_Mode'	-> 


  // Internals - TiSt-Sharing - Block 12 - func
  ,ESP32_DeVICE_R_TiSt_NTP_Server_Main			= (1<<11) // Bit #12 'TiSt_NTP_Server_Main'	-> 
  ,ESP32_DeVICE_R_TiSt_NTP_Server_Backup_1		= (1<<11) // Bit #12 'TiSt_NTP_Server_Backup_1'	-> 
  ,ESP32_DeVICE_R_TiSt_NTP_Server_Backup_2		= (1<<11) // Bit #12 'TiSt_NTP_Server_Backup_2'	-> 
  ,ESP32_DeVICE_R_TiSt_Time_Zone				= (1<<11) // Bit #12 'TiSt_Time_Zone'		-> 


  // Readings - TiSt-Sharing - Block 1 - func
  ,ESP32_DeVICE_R_Station_RSSI					= (1<<12) // Bit #13 'Station_RSSI'		-> 
  ,ESP32_DeVICE_R_TiSt_Time						= (1<<12) // Bit #13 'TiSt_Time'		-> 

  ,ESP32_DeVICE_R_Namex							= (1<<13) // Bit #02 'name' + caps + ufid -> 
};



kvParseImplementedKeys_t ESP32_DeVICE_Set_ImplementedKeys[];


































/* 
 * ESP32_DeVICE_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct Entry_ESP32_DeVICE_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  uint32_t demo;			// custom data



  // block #01
  Reading_t Name;
  Reading_t WSAP_Password;
  Reading_t WSAP_RF_Channel;
  Reading_t WSAP_Maximal_Connections;
  Reading_t WSAP_Authentication_Method;
  Reading_t WSAP_SSID;
  Reading_t WSAP_Beacon_Interval;

  // block #02
  Reading_t WSAP_IP_Adress;
  Reading_t WSAP_Netmask;
  Reading_t WSAP_Gateway_Adress;

  // block #03
  Reading_t ap_mac_addr;

  // block #04
  Reading_t ap_wifi_bandwidth;

  // block #05
  Reading_t country;

  // block #06
  Reading_t sta_ip_info;

  // block #07
  Reading_t sta_mac_addr;

  // block #08
  Reading_t sta_autoconnect_status;

  // block #09
  Reading_t wifiBandwidthSTA;
  
  // the readings for direct access + state
  reading2_t *p_WSAP;                            // link to reading, maintained by this module
  reading2_t *p_WSAP_Beacon_Interval;            // link to reading, maintained by this module

} Entry_ESP32_DeVICE_Definition_t;



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */

// Attribute Fn - called in case of attribute changes for this definition, to check them
//Entry_String_t* ESP32_DeVICE_Attribute (Common_Definition_t* p_entry_definition, const String_t attr_command, const String_t attr_name, const String_t attr_value);

//
//Entry_String_t * ESP32_DeVICE_Define(entry_common_definition_t *p_entry_common_definition);

//
//Entry_String_t* ESP32_DeVICE_GetV2(Entry_Definition_t* p_Entry_Definition, char **argv, int argc);

//
//strTextMultiple_t* ESP32_DeVICE_Get(Common_Definition_t* Common_Definition, uint8_t *getArgsText, size_t getArgsTextLen);

//
//int ESP32_DeVICE_Initialize(SCDERoot_t* SCDERoot);

//
//Entry_String_t * ESP32_DeVICE_Notify(Entry_Common_Definition_t *p_notifying_entry_common_definition, Entry_Common_Definition_t *p_own_entry_common_definition);

//   
//strTextMultiple_t* ESP32_DeVICE_Rename(Common_Definition_t *Common_Definition, uint8_t *newName, size_t newNameLen, uint8_t *oldName, size_t oldNameLen);

//
//entry_string_t * ESP32_DeVICE_Set(entry_common_definition_t *p_entry_common_definition, char **p_p_argv, int argc);

//
//strTextMultiple_t* ESP32_DeVICE_Shutdown(Common_Definition_t* Common_Definition);

//
//Entry_String_t* ESP32_DeVICE_State(Common_Definition_t *Common_Definition, const time_t stateTiSt, const String_t stateNameString, const String_t stateValueString, const String_t stateMimeString);

//
//strTextMultiple_t* ESP32_DeVICE_Undefine(Common_Definition_t* Common_Definition);





/*
 *  helpers provided to module for type operation
 */
bool ESP32_DeVICE_ProcessKVInputArgs(Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_DeVICE_SetAffectedReadings(Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition, uint64_t affectedReadings);


int ESP32_DeVICE_APST2_WSAP_cb_b(struct argparse *self, const struct argparse_option *option, int flags,char ***stage1Value);

int ESP32_DeVICE_APST2_WSAP_cb_b(struct argparse *self, const struct argparse_option *option, int flags, char ***stage1Value);

int ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb_b(struct argparse *self, const struct argparse_option *option, int flags, char ***stage1Value);


int
ESP32_DeVICE_APST2_WSAP_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);



int
ESP32_DeVICE_APST2_Name_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);




int
ESP32_DeVICE_APST2_WSAP_Password_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);


int
ESP32_DeVICE_APST2_WSAP_RF_Channel_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);





int
ESP32_DeVICE_APST2_WSAP_Authentication_Method_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);


int
ESP32_DeVICE_APST2_WSAP_SSID_Visibility_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);



int
ESP32_DeVICE_APST2_WSAP_Maximal_Connections_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,int **stage1Value);

int
ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,int **stage1Value);


int
ESP32_DeVICE_APST2_Station_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);


int
ESP32_SwITCH_Set_H_cb(struct argparse *self
                ,const struct argparse_option *option
                ,int flags
                ,void *stage1Value);


int
ESP32_DeVICE_APST2_WIFI_COUNTRY_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,char ***stage1Value);




SelectAData ESP32_DeVICE_DisEn[];
SelectAData ESP32_DeVICE_MAX_CONN[];
SelectAData ESP32_DeVICE_CHANNEL[];
SelectAData ESP32_DeVICE_SSID_VISIBILITY[];
SelectAData ESP32_DeVICE_AP[];
SelectAData ESP32_DeVICE_PH_MODE[];
SelectAData ESP32_DeVICE_WIFI_COUNTRY[];
SelectAData ESP32_DeVICE_AUTH_MODE[];
SelectAData ESP32_DeVICE_WIFI_BANDWIDTH[];





#endif /*ESP32_DEVICE_MODULE_H*/
