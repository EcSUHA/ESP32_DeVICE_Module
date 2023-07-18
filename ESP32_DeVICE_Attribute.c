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
#include "ESP32_DeVICE_Attribute.h"








/* -------------------------------------------------------------------------------------------------
 *  FName: Attribute Fn
 *  Desc: Informs an 'definition' of this 'module' for 'attribute' activities (set/delete)
 *  Info: 'attr_cmmand' is the command for the activity: set, delete
 *        'attr_name' is the attribute name
 *        'attr_value' is the attribute value
 *  Para: Common_Definition_t* p_entry_definition -> the 'definition' identified for the activities
 *	  const String_t attr_command -> the attribute-command
 *	  const String_t attr_name -> the attribute-name
 *	  const String_t attr_value -> the attribute-value
 *  Rets: Entry_String_t* -> = SCDE_OK (no ret msg) or VETO (SLTQ Entry_String_t* with ret msg)
 * -------------------------------------------------------------------------------------------------
 */
Entry_String_t *
ESP32_DeVICE_Attribute(entry_common_definition_t *p_entry_definition,
	 const String_t attr_command,
	 const String_t attr_name,
	 const String_t attr_value)
{
  // make common ptr to modul specific ptr
  Entry_ESP32_DeVICE_Definition_t* p_entry_esp32_device_definition =
		  (Entry_ESP32_DeVICE_Definition_t*) p_entry_definition;

  // to store the ret_msg. SCDE_OK = no msg 
  Entry_String_t* p_entry_ret_msg = SCDE_OK;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(p_entry_esp32_device_definition->common.name,
	p_entry_esp32_device_definition->common.nameLen,
	5,
	"Attribute Fn (Module '%.*s') is called with args: "
	"attr_command '%.*s' attr_name '%.*s' attr_value '%.*s'",
	p_entry_esp32_device_definition->common.module->provided->typeNameLen,
	p_entry_esp32_device_definition->common.module->provided->typeName,attr_command.len,
	attr_command.p_char,
  	attr_name.len,
	attr_name.p_char,
	attr_value.len,
	attr_value.p_char);
  #endif

// ------------------------------------------------------------------------------------------------

  return p_entry_ret_msg;
}




