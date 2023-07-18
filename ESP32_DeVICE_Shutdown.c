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
#include "ESP32_DeVICE_Shutdown.h"
#include "ESP32_DeVICE_readings.h"
//#include "ESP32_DeVICE_Webif_Get_Cb.h"
//#include "ESP32_DeVICE_Webif_Set_Cb.h"
//#include "ESP32_DeVICE_Webif_Jso_Cb.h"







/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Shutdown
 *  Desc: called after 
 *  Info: Invoked by cmd-line 'Undefine ESP32_DeVICE_Definition.common.Name'
 *  Para: Entry_ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_DeVICE_Shutdown(Common_Definition_t* Common_Definition)
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
	,"Executing ShutdownFn of Module '%.*s' for Definition '%.*s'."
	,ESP32_DeVICE_Definition->common.module->provided->typeNameLen
	,ESP32_DeVICE_Definition->common.module->provided->typeName
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






