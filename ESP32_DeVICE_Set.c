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
#include "ESP32_DeVICE_Set.h"
#include "ESP32_DeVICE_readings.h"
//#include "ESP32_DeVICE_Webif_Get_Cb.h"
//#include "ESP32_DeVICE_Webif_Set_Cb.h"
//#include "ESP32_DeVICE_Webif_Jso_Cb.h"












/**
 * -------------------------------------------------------------------------------------------------
 *  FName: Set
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set ESP32_DeVICE_Definition.common.Name setArgs'
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

 
 *  Para: Entry_ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
entry_string_t *
ESP32_DeVICE_Set(entry_common_definition_t *p_entry_common_definition,
    char **argv, int argc)
{
  // set_fn should be called with argc >= 1 only !

/*  // conversion new
  string_t args;
  args.p_char = NULL;//p_args_char;
  args.len = 0;*/

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
//	,"Set Fn ('%.*s' Module) is called  with args '%.*s'."
//	,p_entry_ESP32_DeVICE_definition->common.module->provided->typeNameLen
//	,p_entry_ESP32_DeVICE_definition->common.module->provided->typeName
//	,args.len
//	,args.p_char);
	,"Set Fn ('%.*s' Module) is called  with %d args."
	,p_entry_ESP32_DeVICE_definition->common.module->provided->typeNameLen
	,p_entry_ESP32_DeVICE_definition->common.module->provided->typeName
	,argc);
  #endif
  
// -------------------------------------------------------------------------------------------------
/*
  // Check for args. This type requires args...
  if (!argc) {

	// alloc mem for ret_msg
	p_entry_ret_msg = malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Error! Expected set arguments.");

	return p_entry_ret_msg;
  }
*/
// -------------------------------------------------------------------------------------------------

  // our supplied argparse struct
  struct argparse argparse;

  // various possible usages. no,1,2 ... - NULL marks end of list
  static const char *const set_usages[] = {
	"set definition [options] [[--] args]",
  	"set definition [options]",
  	NULL,
 };

  struct argparse_option set_options[] = {
//	OPT_HELP(), // is 'h', "help", NULL, "show this help message and exit", argparse_help_cb, 0, OPT_NONEG)
	OPT_BOOLEAN	 ('h', "help", 						NULL, 			  	"show this help message",   	argparse_help_cb,       							0, OPT_NONEG),
    OPT_BOOLEAN  ('?', "?",                         NULL,             	"hctrl/fhem gets help",     	&ESP32_SwITCH_Set_H_cb, 							0, OPT_NONEG),  //noneg????

    OPT_GROUP  ("Built-in Wireless-Service-Access-Point configuration"),

    OPT_STRING  ('x', "WSAP",                       p_entry_ESP32_DeVICE_definition->p_WSAP,	                "DISABLED,ENABLED",			    &ESP32_DeVICE_APST2_WSAP_cb_b, 		                0, 0),
    OPT_STRING  ('i', "WSAP_Beacon_Interval",       p_entry_ESP32_DeVICE_definition->p_WSAP_Beacon_Interval,	"100 - 60000",			       	&ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb_b, 		0, 0),

/*
    OPT_STRING   ('x', "WSAP",                      &wifi_mode,      	"DISABLED,ENABLED",          	&ESP32_DeVICE_APST2_WSAP_cb, 						0, 0),
    OPT_STRING   ('n', "Name",                      &wifi_ap_config, 	"characters, max.31",  			&ESP32_DeVICE_APST2_Name_cb, 						0, 0),
    OPT_STRING   ('p', "WSAP_Password",             &wifi_ap_config, 	"characters, max.63",         	&ESP32_DeVICE_APST2_WSAP_Password_cb, 				0, 0),
    OPT_STRING   ('c', "WSAP_RF_Channel",           &wifi_ap_config, 	"CH_1 - CH_14",		           	&ESP32_DeVICE_APST2_WSAP_RF_Channel_cb, 			0, 0),
    OPT_STRING   ('a', "WSAP_Authentication_Method",&wifi_ap_config, 	"OPEN,WPA2_PSK, ... try ?",		&ESP32_DeVICE_APST2_WSAP_Authentication_Method_cb, 	0, 0),
    OPT_STRING   ('v', "WSAP_SSID_Visibility",      &wifi_ap_config, 	"VISIBLE,HIDDEN",              	&ESP32_DeVICE_APST2_WSAP_SSID_Visibility_cb, 		0, 0),
    OPT_INTEGER  ('m', "WSAP_Maximal_Connections",  &wifi_ap_config, 	"1 - 10", 					 	&ESP32_DeVICE_APST2_WSAP_Maximal_Connections_cb, 	0, 0),
    OPT_INTEGER  ('i', "WSAP_Beacon_Interval",      &wifi_ap_config,	"100 - 60000",			       	&ESP32_DeVICE_APST2_WSAP_Beacon_Interval_cb, 		0, 0),

    OPT_GROUP  ("Device-to-Station connection configuration"),
    OPT_STRING   ('y', "Station",                   &wifi_mode,   		"ENABLED,DISABLED",            	&ESP32_DeVICE_APST2_Station_cb, 					0, 0),
    OPT_STRING   ('c', "WIFI_COUNTRY",   			&country,       	"station wifi country", 		&ESP32_DeVICE_APST2_WIFI_COUNTRY_cb, 				0, 0),
    OPT_BOOLEAN  ('f', "Station_Auto_Connect",  	&sta_autoconnect_status,      "force to do", 		NULL, 												0, 0),
    OPT_BOOLEAN  ('f', "force",  					&force,      		"force to do", 					NULL, 												0, 0),
    OPT_BOOLEAN  ('t', "test",   					&test,       		"test only", 					NULL, 												0, 0),
    OPT_STRING   ('p', "path",   					&path,       		"path to read", 				NULL, 												0, 0),
    OPT_INTEGER  ('i', "int",    					&int_num,    		"selected integer", 			NULL, 												0, 0),
    OPT_FLOAT    ('s', "float",  					&flt_num,    		"selected float", 				NULL, 												0, 0),

    OPT_GROUP  ("Debug testing configuration"),
    OPT_BIT      ( 0 , "read",   					&perms,      		"read perm", 					NULL, 												PERM_READ, OPT_NONEG),
    OPT_BIT      ( 0 , "write",  					&perms,      		"write perm", 					NULL, 												PERM_WRITE, 0),
    OPT_BIT      ( 0 , "exec",   					&perms,      		"exec perm", 					NULL, 												PERM_EXEC, 0),

    OPT_GROUP  ("Time configuration"),
    OPT_FLOAT    ('x', "float2", 					&flt_test2,  		"selected float2", 				NULL, 												0, 0),
    OPT_STRING   ('y', "path2",  					&path_test2, 		"path to read2", 				NULL, 												0, 0),
*/
    OPT_END(),
  };

  // init the arg-parse Fn
  p_entry_ret_msg = SCDEFn_at_ESP32_DeVICE_M->ArgParse_InitFn(&argparse
	,set_options
	,set_usages
	,0	// flags: ARGPARSE_STOP_AT_NON_OPTION ?
	,"\nTo SET the configuration of the ESP32 DeVICE"										// prolog usage
	,"\nUse command help ESP32_DeVICE to get the detailed description of the arguments.");	// epilog usage
  if (p_entry_ret_msg) goto err;	// got an error-msg text -> send it to the user!

  // call the parse Fn
  p_entry_ret_msg = SCDEFn_at_ESP32_DeVICE_M->ArgParse_ParseFn(&argparse, argc, argv);
  if (p_entry_ret_msg) goto err;	// got an error-msg text -> send it to the user!

  // wie genau?? mem leak?
  // get the resulting args-no from arg parse fn
  argc = argparse.cpidx + argparse.argc;


  if (argc < 0) printf("An error occured? argc: %d\n", argc);

// -------------------------------------------------------------------------------------------------

 // deep debug
  printf("\nDeep debugging, parsed args bf: %lld, listing parsed values:\n",
  	 argparse.parsed_args_bf);

 /* if (argparse.parsed_args_bf & (1 << DEFINE_ARG_IO_DEVICE))
        printf("IO_DEVICE: '%.*s'\n"
        ,p_entry_ESP32_DeVICE_definition->stage1_definition.len
	    ,p_entry_ESP32_DeVICE_definition->stage1_definition.p_char);
*/
// -------------------------------------------------------------------------------------------------
// 5. Step: analyze if the summary of parsed arguments matches our requirements ...
// -------------------------------------------------------------------------------------------------

/*
  // check if quantity of arguments meet requirements to setup I2C as master
  if ((argparse.parsed_args_bf & (1 << DEFINE_ARG_IO_DEVICE)) == (1 << DEFINE_ARG_IO_DEVICE )) {
      // match !                                                                         
  } 

  // quantity of arguments do NOT meet requirements to setup I2C       
  else {

	// alloc mem for ret_msg
	p_entry_ret_msg = malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Error! Quantity of arguments not matching the requirements to set up the BMP180.");

	goto err;                                                                       
 }                                                    
*/
// -------------------------------------------------------------------------------------------------
// 6. Step: do further logic with the parsed arguments
//          - parsed_args_bf (bitfield) contains information about parsed arguments
//          - affected_readings (bitfield) may contain information about readings to maintain
// -------------------------------------------------------------------------------------------------

/*
  // remember the readings affected by the parsing process
  // on value change can affect multipe readings -> bitfield
  uint32_t affected_readings = 0;


  if (argparse.parsed_args_bf) affectedReadings = 0xffffffff;
 // set device WSAP_Beacon_Interval=109
 */
// -------------------------------------------------------------------------------------------------
// 5. Step: - update the readings according to 'affected_readings' - if any ...
//          - finetune reading data, if necessary
// -------------------------------------------------------------------------------------------------

  printf ("affected_readings:%d", argparse.affected_readings_bf);

  // update the readings according to 'affected_readings' bitfield - if any ...
  if ( argparse.affected_readings_bf ) {

      // we have affected readings, indicate begin of readings update 
      SCDEFn_at_ESP32_DeVICE_M->readings_begin_update_fn((entry_common_definition_t *) p_entry_ESP32_DeVICE_definition);

// -------------------------------------------------------------------------------------------------

	  // Reading 'WSAP'
	  if ( argparse.affected_readings_bf & ESP32_DeVICE_R_WSAP ) {

          // maintain reading state
          reading2_t *p_WSAP =
              p_entry_ESP32_DeVICE_definition->p_WSAP;
         
/*        // update the raw data -> its text
          if (p_state_reading->raw_data.p_char) free(p_state_reading->raw_data.p_char); 
          p_state_reading->raw_data.len = asprintf((char **) &p_state_reading->raw_data.p_char
              ,"pressure: %d Pa, altitude: %.1f m, temperature: %.1f °C"
	          ,pressure
	          ,altitude
	          ,temperature);*/

          // announce update
          SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update_fn((entry_common_definition_t *) p_entry_ESP32_DeVICE_definition
	          ,p_WSAP
	          ,true, 0);
	  }

// ------------------------------------------------------------------------------------------------- 

	  // Reading 'WSAP_Beacon_Interval'
	  if ( argparse.affected_readings_bf & ESP32_DeVICE_R_WSAP_Beacon_Interval ) {

          // maintain reading state
          reading2_t *p_WSAP_Beacon_Interval =
              p_entry_ESP32_DeVICE_definition->p_WSAP_Beacon_Interval;
         
/*        // update the raw data -> its text
          if (p_state_reading->raw_data.p_char) free(p_state_reading->raw_data.p_char); 
          p_state_reading->raw_data.len = asprintf((char **) &p_state_reading->raw_data.p_char
              ,"pressure: %d Pa, altitude: %.1f m, temperature: %.1f °C"
	          ,pressure
	          ,altitude
	          ,temperature);*/

          // announce update
          SCDEFn_at_ESP32_DeVICE_M->readings_bulk_update_fn((entry_common_definition_t *) p_entry_ESP32_DeVICE_definition
	          ,p_WSAP_Beacon_Interval
	          ,true, 0);
	  }

// -------------------------------------------------------------------------------------------------

      // all readings done, finnish update
      SCDEFn_at_ESP32_DeVICE_M->readings_end_update_fn((entry_common_definition_t *) p_entry_ESP32_DeVICE_definition, true);
  }

// -------------------------------------------------------------------------------------------------
// 6. Step: Ok, done without error
// -------------------------------------------------------------------------------------------------

  return p_entry_ret_msg;

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

err:

  // alternative end in case of errors - free / destroy all allocated things and return with ret msg.

  return p_entry_ret_msg;
}



