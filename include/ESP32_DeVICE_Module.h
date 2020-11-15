// SCDE-Module ESP32_DeVICE

#ifndef ESP32_DEVICE_MODULE_H
#define ESP32_DEVICE_MODULE_H


// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef ESP32_DeVICE_Module_DBG  
#define ESP32_DeVICE_Module_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef ESP32_SCDE_Module_DBG  
#define ESP32_SCDE_Module_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------











// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
#include "WebIf_Module global types.h"
//#include "WebIf_Module_s.h"

// this Module uses an 1st stage:
// -- no ---

// -------------------------------------------------------------------------------------------------

// stores the Root Data of the Smart Connected Devices Engine - at/for this Module
SCDERoot_t* SCDERoot_at_ESP32_DeVICE_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_ESP32_DeVICE_M;

// -------------------------------------------------------------------------------------------------



/* 
 * ESP32_DeVICE_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct Entry_ESP32_DeVICE_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  uint32_t demo;			// custom data



  // block #01
  Reading2_t Name;
  Reading2_t WSAP_Password;
  Reading2_t WSAP_RF_Channel;
  Reading2_t WSAP_Maximal_Connections;
  Reading2_t WSAP_Authentication_Method;
  Reading2_t WSAP_SSID;
  Reading2_t WSAP_Beacon_Interval;

  // block #02
  Reading2_t WSAP_IP_Adress;
  Reading2_t WSAP_Netmask;
  Reading2_t WSAP_Gateway_Adress;

  // block #03
  Reading2_t ap_mac_addr;

  // block #04
  Reading2_t ap_wifi_bandwidth;

  // block #05
  Reading2_t country;

  // block #06
  Reading2_t sta_ip_info;

  // block #07
  Reading2_t sta_mac_addr;

  // block #08
  Reading2_t sta_autoconnect_status;

  // block #09
  Reading2_t wifiBandwidthSTA;

} Entry_ESP32_DeVICE_Definition_t;



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */

// Attribute Fn - called in case of attribute changes for this definition, to check them
Entry_String_t* ESP32_DeVICE_Attribute (Common_Definition_t* p_entry_definition, const String_t attr_command, const String_t attr_name, const String_t attr_value);

strTextMultiple_t* ESP32_DeVICE_Define(Common_Definition_t *Common_Definition);

//
Entry_String_t* ESP32_DeVICE_GetV2(Entry_Definition_t* p_Entry_Definition, char **argv, int argc);

//
strTextMultiple_t* ESP32_DeVICE_Get(Common_Definition_t* Common_Definition, uint8_t *getArgsText, size_t getArgsTextLen);

int ESP32_DeVICE_Initialize(SCDERoot_t* SCDERoot);

strTextMultiple_t* ESP32_DeVICE_Rename(Common_Definition_t *Common_Definition, uint8_t *newName, size_t newNameLen, uint8_t *oldName, size_t oldNameLen);

strTextMultiple_t* ESP32_DeVICE_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

strTextMultiple_t* ESP32_DeVICE_Shutdown(Common_Definition_t* Common_Definition);

Entry_String_t* ESP32_DeVICE_State(Common_Definition_t *Common_Definition, const time_t stateTiSt, const String_t stateNameString, const String_t stateValueString, const String_t stateMimeString);

strTextMultiple_t* ESP32_DeVICE_Undefine(Common_Definition_t* Common_Definition);

static esp_err_t ESP32_DeVICE_WiFiEventHandler(void *ctx, system_event_t *event);

static void Initialize_SNTP(void);

static void Obtain_Time(void);



/*
 *  helpers provided to module for type operation
 */
bool ESP32_DeVICE_ProcessKVInputArgs(Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_DeVICE_SetAffectedReadings(Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition, uint64_t affectedReadings);



#endif /*ESP32_DEVICE_MODULE_H*/
