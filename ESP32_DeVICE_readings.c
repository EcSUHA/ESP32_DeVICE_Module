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
#include "ESP32_DeVICE_readings.h"

// -------------------------------------------------------------------------------------------------



string_t
ESP32_DeVICE_Get_Reading_State_As_Text(reading2_t *p_state_reading)
{
  string_t state_text;
    
  state_text.len = asprintf((char **) &state_text.p_char
		  ,"%.*s"
          ,p_state_reading->raw_data.len
          ,p_state_reading->raw_data.p_char);
  
  return state_text;
}

/*
string_t
ESP32_DeVICE_Get_Reading_Luminosity_As_Text(reading2_t *p_luminosity_reading)
{
  string_t luminosity_text;
  
  float luminosity = *((float *)p_luminosity_reading->raw_data.p_char);
  
  luminosity_text.len = asprintf((char **) &luminosity_text.p_char
		  ,"%.1f lx"  // (lx)
          ,luminosity);
  
  return luminosity_text;
}
*/

string_t
ESP32_DeVICE_Get_Reading_WSAP_As_Text(reading2_t *p_WSAP_reading)
{
  string_t WSAP_text;
  
//wifi_ap_config_t *wifi_ap_config =
//    (wifi_ap_config_t *)p_WSAP_Beacon_Interval_reading->raw_data.p_char;
  
  // get wifi_mode
  wifi_mode_t wifi_mode =
  esp_wifi_get_mode(&wifi_mode);
  
    // get bit 1 for AP
//  wifi_mode &= 0b10; //( ((uint8_t) wifi_mode & 0b01) | (new_wifi_mode << 1) ) ;
  wifi_mode = wifi_mode >> 1;

  char *WSAP = SCDE_GetDesc(&ESP32_DeVICE_DisEn, wifi_mode);
  
  WSAP_text.len = asprintf((char **) &WSAP_text.p_char
		  ,"%s"  // (-)
          ,WSAP);
  
  return WSAP_text;
}

string_t
ESP32_DeVICE_Get_Reading_WSAP_Beacon_Interval_As_Text(reading2_t *p_WSAP_Beacon_Interval_reading)
{
  string_t WSAP_Beacon_Interval_text;
  
//wifi_ap_config_t *wifi_ap_config =
//    (wifi_ap_config_t *)p_WSAP_Beacon_Interval_reading->raw_data.p_char;
  
  // get 'wifi_ap_config' (WSAP-Configuration)
  wifi_ap_config_t wifi_ap_config;
  esp_wifi_get_config (WIFI_IF_AP, &wifi_ap_config);
  
  WSAP_Beacon_Interval_text.len = asprintf((char **) &WSAP_Beacon_Interval_text.p_char
		  ,"%d"  // (ms)
          ,wifi_ap_config.beacon_interval);
  
  return WSAP_Beacon_Interval_text;
}


// -------------------------------------------------------------------------------------------------



static bool
str_to_uint16_2(const char *str, uint16_t *res)
{
  char *end;
  errno = 0;
  long val = strtol(str, &end, 10);
  if (errno || end == str || *end != '\0' || val < 0 || val >= 0x10000) {
      return false;
  }
  *res = (uint16_t)val;
  return true;
}

bool // if error (value not traken) -> returns FALSE
ESP32_DeVICE_Set_Reading_WSAP_From_Text(reading2_t *p_WSAP_Beacon_Interval_reading, char *p_value_as_text)
{
  uint8_t new_wifi_mode;

  // check if the argument is in the allowed list
  if (!SCDEH_GetQueryKeyID((uint8_t*) p_value_as_text
        ,strlen(p_value_as_text)
        ,(uint8_t*) &new_wifi_mode
        ,&ESP32_DeVICE_DisEn) ) return false;

  // get wifi_mode
  wifi_mode_t wifi_mode =
  esp_wifi_get_mode(&wifi_mode);

  // patch on/off
  wifi_mode = ( ((uint8_t) wifi_mode & 0b01) | (new_wifi_mode << 1) ) ;

  // set wifi_mode
  esp_wifi_set_mode(wifi_mode);

  return true;
}

bool // if error (value not traken) -> returns FALSE
ESP32_DeVICE_Set_Reading_WSAP_Beacon_Interval_From_Text(reading2_t *p_WSAP_Beacon_Interval_reading, char* p_value_as_text)
{
  uint16_t new_beacon_interval;
 
  // read uint16_t from char ptr, return false in case of error
  if (!str_to_uint16_2(p_value_as_text,
      &new_beacon_interval)) return false;


  printf("MAIKstoring2:%u (%s)!",new_beacon_interval, p_value_as_text);

  // argument to low / high ?
  if ( (new_beacon_interval < 100) || (new_beacon_interval > 60000) ) return false;

  // get 'wifi_ap_config' (WSAP-Configuration)
  wifi_ap_config_t wifi_ap_config;
  esp_wifi_get_config(WIFI_IF_AP, &wifi_ap_config);

  // set new interval
  wifi_ap_config.beacon_interval = new_beacon_interval;
  
  // set 'wifi_ap_config' (WSAP-Configuration)
  esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config);

  return true;
}





/*
  // do we have text?
  if ( (p_value_as_text) && (strlen(p_value_as_text)) ) {
 
      // yes 
      if (p_state_reading->raw_data.p_char) free(p_state_reading->raw_data.p_char);
        
      p_state_reading->raw_data.len = asprintf((char **) &p_state_reading->raw_data.p_char
		  ,"%s"
          ,p_value_as_text);
  } 
  
  /// no
  else {
  
  p_state_reading->raw_data.p_char = NULL;
  p_state_reading->raw_data.len = 0;
  }
  
  printf ("stored to cccc reading:'%.*s'",
          p_state_reading->raw_data.len,
          p_state_reading->raw_data.p_char);
  
  */
  
 



//wifi_ap_config->beacon_interval = (uint16_t) new_beacon_interval;


// -------------------------------------------------------------------------------------------------



// ESP32_DeVICE - embedded reading types
reading_type_t ESP32_DeVICE_reading_types[] = { 
 { // reading state
  "STATE"      , 
  "null", 
  &ESP32_DeVICE_ProvidedByModule, 
  ESP32_DeVICE_Get_Reading_State_As_Text,
  NULL
 },/*
 { // reading luminosity
  "LUMINOSITY", 
  "lx", 
  &ESP32_DeVICE_ProvidedByModule, 
  ESP32_DeVICE_Get_Reading_Luminosity_As_Text,
  NULL
 },*/

 { // reading WSAP
  "WSAP", 
  "null", 
  &ESP32_DeVICE_ProvidedByModule, 
  ESP32_DeVICE_Get_Reading_WSAP_As_Text,
  ESP32_DeVICE_Set_Reading_WSAP_From_Text
 },
  
 { // reading WSAP_Beacon_Interval
  "WSAP_Beacon_Interval", 
  "ms", 
  &ESP32_DeVICE_ProvidedByModule, 
  ESP32_DeVICE_Get_Reading_WSAP_Beacon_Interval_As_Text,
  ESP32_DeVICE_Set_Reading_WSAP_Beacon_Interval_From_Text
 },

 {NULL} // marks end of reading types
};



