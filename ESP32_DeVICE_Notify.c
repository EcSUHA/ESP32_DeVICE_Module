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
#include "ESP32_DeVICE_Notify.h"





/* -------------------------------------------------------------------------------------------------
 *  FName: Notify Fn
 *  Desc: Informs an 'definition' of this 'module' for 'reading' and 'state' changes
 *  Info: 
 *  Para: Entry_Common_Definition_t *p_notifying_entry_common_definition -> the 'definition' identified for the activities
 *	      Entry_Common_Definition_t *p_own_entry_common_definition -> the 'definition' identified for the activities
 *  Rets: Entry_String_t* -> = SCDE_OK (no ret msg) or SLTQ Entry_String_t* with ret msg
 * -------------------------------------------------------------------------------------------------
 */
Entry_String_t *
ESP32_DeVICE_Notify(Entry_Common_Definition_t *p_notifying_entry_common_definition,
    Entry_Common_Definition_t *p_own_entry_common_definition)
{
  // make common ptr to modul specific ptr
  Entry_ESP32_DeVICE_Definition_t* p_entry_esp32_device_definition =
		  (Entry_ESP32_DeVICE_Definition_t*) p_own_entry_common_definition;

  // to store the ret_msg. SCDE_OK = no msg 
  Entry_String_t *p_entry_ret_msg = SCDE_OK;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(p_entry_esp32_device_definition->common.name,
	p_entry_esp32_device_definition->common.nameLen,
	5,
	"Notify Fn (Module '%.*s') is called. Definition '%.*s' (Module '%.*s') informs about pending notifies.",
	p_entry_esp32_device_definition->common.module->provided->typeNameLen,
	p_entry_esp32_device_definition->common.module->provided->typeName,
	p_notifying_entry_common_definition->nname.len,
	p_notifying_entry_common_definition->nname.p_char,
	p_notifying_entry_common_definition->module->provided->typeNameLen,
	p_notifying_entry_common_definition->module->provided->typeName);
  #endif

// ------------------------------------------------------------------------------------------------

  string_t td_string = 
      SCDEFn_at_ESP32_DeVICE_M->get_formated_date_time_fn(p_notifying_entry_common_definition->p_changed->update_timestamp); 

  printf("List of notifys (common timestamp '%.*s') from definition '%.*s':\n",
      td_string.len,
      td_string.p_char,
      p_notifying_entry_common_definition->nname.len,
	  p_notifying_entry_common_definition->nname.p_char);
  free(td_string.p_char);
   
  // list currently added notifies stored for processing
  entry_notify_t *p_current_entry_notify;
  STAILQ_FOREACH(p_current_entry_notify, &p_notifying_entry_common_definition->p_changed->head_notifies, entries) {
  	
      string_t td_string = 
          SCDEFn_at_ESP32_DeVICE_M->get_formated_date_time_fn(p_current_entry_notify->notify.reading->timestamp);	
      string_t value_as_text = 
          p_current_entry_notify->notify.reading->p_reading_type->p_get_raw_reading_as_text_fn(p_current_entry_notify->notify.reading);				

      printf("L  %.*s | %.*s = %.*s\n"
	      ,td_string.len
		  ,td_string.p_char
		  ,p_current_entry_notify->notify.reading->name.len
		  ,p_current_entry_notify->notify.reading->name.p_char
		  ,value_as_text.len
          ,value_as_text.p_char);
  
	  free(value_as_text.p_char);
	  free(td_string.p_char);
  }

// -------------------------------------------------------------------------------------------------

  return p_entry_ret_msg;
}







