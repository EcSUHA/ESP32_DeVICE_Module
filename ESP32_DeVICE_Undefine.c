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
#include "ESP32_DeVICE_Undefine.h"






/** TEXT IST FALSCH!!!!!!!!!!
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Undefine
 *  Desc: Removes the define of an "device" of 'WebIF' type. Contains devicespecific init code.
 *  Info: Invoked by cmd-line 'Undefine ESP32_DeVICE_Definition.common.Name'
 *  Para: Entry_ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_DeVICE_Undefine(Common_Definition_t* Common_Definition)
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
		,"Executing UndefineFn of Module '%.*s' for Definition '%.*s'."
		,ESP32_DeVICE_Definition->common.module->provided->typeNameLen
		,ESP32_DeVICE_Definition->common.module->provided->typeName
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



