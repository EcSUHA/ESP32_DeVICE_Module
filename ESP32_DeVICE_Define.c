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
#include "ESP32_DeVICE_Define.h"








#include "freertos/event_groups.h"



// FreeRTOS event group to signal when we are connected & ready to make a request
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? */
//static const int CONNECTED_BIT = BIT0;




/*
// FreeRTOS event group to signal when we are connected & ready to make a request
static EventGroupHandle_t wifi_event_group;

/ * The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? * /
static const int CONNECTED_BIT = BIT0;
*/







#include "esp_sntp.h"

//#include "sntp.h"
//#include "lwip/include/apps/sntp/sntp.h"

//#include "apps/sntp/sntp.h"


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
Entry_String_t *
ESP32_DeVICE_Define(entry_common_definition_t *p_entry_common_definition)
{
  // the ret msg
  entry_string_t *p_entry_ret_msg = SCDE_OK;

  // make common ptr to modul specific ptr
  Entry_ESP32_DeVICE_Definition_t* p_entry_ESP32_DeVICE_definition =
		  (Entry_ESP32_DeVICE_Definition_t*) p_entry_common_definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(p_entry_common_definition->name
	,p_entry_common_definition->nameLen
	,5
	,"Define Fn ('%.*s' Module), will continue creation of definition '%.*s' with args '%.*s'."
	,p_entry_ESP32_DeVICE_definition->common.module->provided->typeNameLen
	,p_entry_ESP32_DeVICE_definition->common.module->provided->typeName
	,p_entry_ESP32_DeVICE_definition->common.nameLen
	,p_entry_ESP32_DeVICE_definition->common.name
	,p_entry_ESP32_DeVICE_definition->common.definition.len
	,p_entry_ESP32_DeVICE_definition->common.definition.p_char);
  #endif

// -------------------------------------------------------------------------------------------------

  // Check for args. This type requires args...
  if (!p_entry_common_definition->definition.len) {

	// alloc mem for ret_msg
	p_entry_ret_msg = malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Error! Expected definition arguments.");

	return p_entry_ret_msg;
  }
  
// -------------------------------------------------------------------------------------------------

// check: nur 1 x erlaubt

// -------------------------------------------------------------------------------------------

  // store FD to Definition. Will than be processed in global loop ... -> THIS MODULE USES NO FD
  p_entry_ESP32_DeVICE_definition->common.fd = -1;

// -------------------------------------------------------------------------------------------

  // init WebIf_Provided offset
//  p_entry_ESP32_DeVICE_definition->common.link =
//	&p_entry_ESP32_DeVICE_definition->WebIf_Provided;

  // check for loaded Module 'WebIf' -> get provided Fn
  p_entry_ESP32_DeVICE_definition->WebIf_Provided.WebIf_FnProvided =
	NULL;//(WebIf_FnProvided_t *) SCDEFn_at_ESP32_DeVICE_M->GetFnProvidedByModule("WebIf");

 // Providing data for WebIf? Initialise data provided for WebIf
  if (p_entry_ESP32_DeVICE_definition->WebIf_Provided.WebIf_FnProvided) {

	p_entry_ESP32_DeVICE_definition->WebIf_Provided.ActiveResourcesDataA =
		(WebIf_ActiveResourcesDataA_t *) &ESP32_DeVICE_ActiveResourcesDataA_forWebIf;

	p_entry_ESP32_DeVICE_definition->WebIf_Provided.ActiveResourcesDataB =
		(WebIf_ActiveResourcesDataB_t *) &ESP32_DeVICE_ActiveResourcesDataB_forWebIf;

	}

  else	{

	SCDEFn_at_ESP32_DeVICE_M->Log3Fn(p_entry_common_definition->name
		,p_entry_common_definition->nameLen
		,1
		,"Could not enable WebIf support for '%.*s'. "
		 "Type '%.*s' detects Type 'WebIf' is NOT loaded!"
		,p_entry_ESP32_DeVICE_definition->common.nameLen
		,p_entry_ESP32_DeVICE_definition->common.name
		,p_entry_ESP32_DeVICE_definition->common.module->provided->typeNameLen
		,p_entry_ESP32_DeVICE_definition->common.module->provided->typeName);
	}

//temporary install web if resources here
p_entry_ESP32_DeVICE_definition->common.ActiveResourcesDataA = 
    &ESP32_DeVICE_ActiveResourcesDataA_forWebIf;
p_entry_ESP32_DeVICE_definition->common.ActiveResourcesDataB = 
    &ESP32_DeVICE_ActiveResourcesDataB_forWebIf;

// -------------------------------------------------------------------------------------------

// GENERAL WIFI INIT (

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
//  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );







  wifi_config_t sta_config = {
        .sta = {
            .ssid = "SF4_AP",
            .password = "pcmcia91",
            .bssid_set = false
        }
  };

  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );





/*
  wifi_config_t ap_config = {
        .ap = {
            .ssid = "test",
          //.ssid_len =
            .password = "pcmcia91",
            .ssid_hidden = false,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = 4,
            .beacon_interval =102,
            .channel =4
        }
  };

  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_config) );
*/



  // s3.1: Call esp_wifi_start to start the Wi-Fi driver.
  ESP_ERROR_CHECK( esp_wifi_start() );




  ESP_ERROR_CHECK( esp_wifi_connect() );
/*
 // wait till wifi is connected
  xEventGroupWaitBits(wifi_event_group
		  , CONNECTED_BIT
		  ,false
		  ,true
		  ,portMAX_DELAY);
*/
  // sync time via SNTP
  Obtain_Time();










// -------------------------------------------------------------------------------------------


// create readings here?

  // create maintained reading 'WSAP' (ESP32_DeVICE:WSAP type), and store ptr in definition for fast access
  p_entry_ESP32_DeVICE_definition->p_WSAP =
      SCDEFn_at_ESP32_DeVICE_M->Create_Reading((entry_common_definition_t *)p_entry_ESP32_DeVICE_definition,
          NULL, // use default ("state")
          NULL, // initial value as text, or NULL
          NULL, // keep default unit
//        "ESP32_DeVICE", "WSAP");
          "scde", "WSAP");

  // create maintained reading 'WSAP_Beacon_Interval' (ESP32_DeVICE:WSAP_Beacon_Interval type), and store ptr in definition for fast access
  p_entry_ESP32_DeVICE_definition->p_WSAP_Beacon_Interval =
      SCDEFn_at_ESP32_DeVICE_M->Create_Reading((entry_common_definition_t *)p_entry_ESP32_DeVICE_definition,
          NULL, // use default ("state")
          NULL, // initial value as text, or NULL
          NULL, // keep default unit
//          "ESP32_DeVICE", "WSAP_Beacon_Interval");
          "scde", "WSAP_Beacon_Interval");








// -------------------------------------------------------------------------------------------------
// 1. Step: Split 'definition' args into allocated memory
//             aa bbb ccc "dd d" ee         <- original string
//             ---------------------
//             aa0bbb0ccc00dd d00ee0        <- transformed string
//   arg  c    |  |   |    |     |
//   argv[0] __/  /   /    /     /
//   argv[1] ____/   /    /     /
//   argv[2] _______/    /     /
//   argv[3] ___________/     /
//   argv[4] ________________/ 
//   Note: args inside '""' are not splitted !
// -------------------------------------------------------------------------------------------------

  int argc;

  // split 'definition' args into allocated memory
  char **argv = SCDEFn_at_ESP32_DeVICE_M->ArgParse_SplitArgsToAllocatedMemFn(&argc,
      p_entry_ESP32_DeVICE_definition->common.definition.p_char,
      p_entry_ESP32_DeVICE_definition->common.definition.len);

// -------------------------------------------------------------------------------------------------
// 2. Step: Parse KEYs into mirrored data, opt. 1st check cb if the given values match the requirements
// -------------------------------------------------------------------------------------------------

  // our supplied argparse struct
  struct argparse argparse;
  
  // listing of possible usages ...
/*static const char *const define_usages[] = {
	"define definition [options] [[--] args]",
  	"define definition [options]",
  	NULL,
 };*/
 
  enum DEFINE_ARGS {				// Bit #XX for debugging
  // Block 1 - I2C general cfg  
  DEFINE_ARG_IO_DEVICE,        	    // Bit #00 'IO_DEVICE'			-> 
  };

  struct argparse_option define_options[] = {
//	OPT_HELP(), // is 'h', "help", NULL, "show this help message and exit", argparse_help_cb, 0, OPT_NONEG  ,0,0,0)
	OPT_BOOLEAN	 ('h', "help", 						NULL, 			  	"show this help message",   	argparse_help_cb,       							0, OPT_NONEG    ,0,0,0),
//    OPT_BOOLEAN  ('?', "?",                         NULL,             	"hctrl/fhem gets help",     	&ESP32_I2C_Master_Set_H_cb, 						0, OPT_NONEG    ,0,0,0),  //noneg????

    OPT_GROUP  ("sensor configuration"),
//     OPT_STRING   ('a', "IO_DEVICE",         p_entry_BH1750_definition, 	                "definition name",	            	&BH1750_APST2_IO_DEVICE_cb,                	0, OPT_NONE, DEFINE_ARG_IO_DEVICE,           0,0),

    OPT_GROUP  ("Built-in Wireless-Service-Access-Point configuration"),
/*
    OPT_STRING   ('x', "WSAP",                      &wifi_mode,      	"DISABLED,ENABLED",          	&ESP32_DeVICE_APST2_WSAP_cb, 						0, 0),
    OPT_STRING   ('n', "Name",                      &wifi_ap_config, 	"characters, max.31",  			&ESP32_DeVICE_APST2_Name_cb, 						0, 0),
    OPT_STRING   ('p', "WSAP_Password",             &wifi_ap_config, 	"characters, max.63",         	&ESP32_DeVICE_APST2_WSAP_Password_cb, 				0, 0),
    OPT_STRING   ('c', "WSAP_RF_Channel",           &wifi_ap_config, 	"CH_1 - CH_14",		           	&ESP32_DeVICE_APST2_WSAP_RF_Channel_cb, 			0, 0),
    OPT_STRING   ('a', "WSAP_Authentication_Method",&wifi_ap_config, 	"OPEN,WPA2_PSK, ... try ?",		&ESP32_DeVICE_APST2_WSAP_Authentication_Method_cb, 	0, 0),
    OPT_STRING   ('v', "WSAP_SSID_Visibility",      &wifi_ap_config, 	"VISIBLE,HIDDEN",              	&ESP32_DeVICE_APST2_WSAP_SSID_Visibility_cb, 		0, 0),
    OPT_INTEGER  ('m', "WSAP_Maximal_Connections",  &wifi_ap_config, 	"1 - 10", 					 	&ESP32_DeVICE_APST2_WSAP_Maximal_Connections_cb, 	0, 0),
*/
    OPT_STRING  ('x', "WSAP",                       p_entry_ESP32_DeVICE_definition->p_WSAP,	                "DISABLED,ENABLED",			    &ESP32_DeVICE_APST2_WSAP_cb_b, 		                0, 0),
    OPT_STRING  ('i', "WSAP_Beacon_Interval",       p_entry_ESP32_DeVICE_definition->p_WSAP_Beacon_Interval,	"100 - 60000",			       	&ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb_b, 		0, 0),



    OPT_END(),
  };
  
  // init the arg-parse Fn - NO usages + prolog + epilog in define fn
  p_entry_ret_msg = SCDEFn_at_ESP32_DeVICE_M->ArgParse_InitFn(&argparse
	,define_options                                 // the options (required!)
	,NULL                                           // provide cmd usages
	,0	                                            // flags: e.g. ARGPARSE_STOP_AT_NON_OPTION ?
	,NULL								            // "\noptions prolog"
	,NULL);                         	            // "\noptions epilog"
  if (p_entry_ret_msg) goto err;	// got an error-msg, stop definition with error msg

  // call the arg-parse Fn
  p_entry_ret_msg = SCDEFn_at_ESP32_DeVICE_M->ArgParse_ParseFn(&argparse, argc, argv);
  if (p_entry_ret_msg) goto err;	// got an error-msg, stop definition with error msg

 
 

  // wie genau?? mem leak?
  // get the resulting args-no from arg parse fn
  argc = argparse.cpidx + argparse.argc;


  if (argc < 0) printf("An error occured? argc: %d\n", argc);
  
// -------------------------------------------------------------------------------------------------
 // deep debug
  printf("\nDeep debugging, parsed args bf: %lld, listing parsed values:\n",
  	 argparse.parsed_args_bf);
/*
  if (argparse.parsed_args_bf & (1 << DEFINE_ARG_IO_DEVICE))
        printf("IO_DEVICE: '%.*s'\n"
        ,p_entry_ESP32_DeVICE_definition->stage1_definition.len
	    ,p_entry_ESP32_DeVICE_definition->stage1_definition.p_char);*/

// -------------------------------------------------------------------------------------------------
// 5. Step: analyze if the summary of parsed arguments matches our requirements ...
// -------------------------------------------------------------------------------------------------
/*
  // check if quantity of arguments meet requirements to define
  if ((argparse.parsed_args_bf & (1 << DEFINE_ARG_IO_DEVICE)) == (1 << DEFINE_ARG_IO_DEVICE )) {
      // match !                                                                         
  } 

  // quantity of arguments do NOT meet requirements to create definition     
  else {

	// alloc mem for ret_msg
	p_entry_ret_msg = malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Error! Quantity of arguments not matching the requirements.");

	goto err;                                                                       
 }                                                    
*/
// -------------------------------------------------------------------------------------------------
// 6. Step: do something with the parsed arguments
// -------------------------------------------------------------------------------------------------









// ----------------------- INIT READINGS


















// -------------------------------------------------------------------------------------------
/*
  // Parse define-args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t *parsedKVInput = 
	SCDEFn_at_ESP32_DeVICE_M->ParseKVInputArgsFn(ESP32_DeVICE_Set_NUMBER_OF_IK	// Num Implementated KEYs MAX
	,ESP32_DeVICE_Set_ImplementedKeys				// Implementated Keys
	,p_entry_common_definition->definition.p_char	// our args text
	,p_entry_common_definition->definition.len);	// our args text len

  // parsing may report an problem. args contain: unknown keys, double keys, ...?
  if (!parsedKVInput) {

	// alloc mem for ret_msg
	p_entry_ret_msg = malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Parsing Error! Args '%.*s' not taken! Check the KEYs!"
		,p_entry_common_definition->definition.len
		,p_entry_common_definition->definition.p_char);

	return p_entry_ret_msg;
  }

// ------------------------------------------------------------------------------------------------

  // set required Keys -> Keys that should be there in this request
  parsedKVInput->requiredKVBF = 0;/ ( (1 << ESP32_SwITCH_SET_GPIO)
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
/*
  // set forbidden Keys -> Keys that are not allowed in this request
  parsedKVInput->forbiddenKVBF = 0;
  */
/*
  // process the set-args (key=value@) protocol
  if (ESP32_DeVICE_ProcessKVInputArgs(p_entry_ESP32_DeVICE_definition
	,parsedKVInput				// KVInput parsed
	,p_entry_common_definition->definition.p_char				// our args text
	,p_entry_common_definition->definition.len) ) {			// our args text len

 	// Processing reports an problem. Args not taken. Response with error text.
	
	// alloc mem for ret_msg
	p_entry_ret_msg = malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Processing Error! Args '%.*s' not taken! Check the VALUEs!"
		,p_entry_common_definition->definition.len
		,p_entry_common_definition->definition.p_char);

	return p_entry_ret_msg;
  }
*/
// ------------------------------------------------------------------------------------------------

  // set affected readings
 // ESP32_DeVICE_SetAffectedReadings(p_entry_ESP32_DeVICE_definition
//	,parsedKVInput->affectedReadingsBF);

// ------------------------------------------------------------------------------------------------


//old end
/*
  // free allocated memory for query result key-field
  free(parsedKVInput);

  return retMsg;
}
*/


//free(parsedKVInput);

// new end


  // create maintained reading 'state' (GLOBAL:STATE type), and store ptr in definition for fast access
  p_entry_ESP32_DeVICE_definition->common.p_state_reading =
      SCDEFn_at_ESP32_DeVICE_M->Create_Reading((entry_common_definition_t *)p_entry_ESP32_DeVICE_definition,
          NULL, // use default ("state")
          "initialized", // initial value as text, or NULL
          NULL, // keep default unit
          "global", "state");
     
// -------------------------------------------------------------------------------------------------

  // ArgParseFn has allocated mem. Free it.
  SCDEFn_at_ESP32_DeVICE_M->ArgParse_FreeSplittedArgsInAllocatedMemFn(argv);

  // set up 1st idle Callback
//  p_entry_ESP32_DeVICE_definition->common.Common_CtrlRegA |= F_WANTS_IDLE_TASK;

  return p_entry_ret_msg;

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

err:

  // alternative end in case of errors - free / destroy all allocated things and return with ret msg.

  // ArgParseFn has allocated mem. Free it.
  if (argv) SCDEFn_at_ESP32_DeVICE_M->ArgParse_FreeSplittedArgsInAllocatedMemFn(argv);

  return p_entry_ret_msg;
}



