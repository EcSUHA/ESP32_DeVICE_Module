//##################################################################################################
//###  Function: Callback for Service Access-Point Configuration (tpl resource)
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include "ProjectConfig.h"
#include <esp8266.h>
#include "Platform.h"


/*
#include "c_types.h"
#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "SCDED.h"
//#include "CGI_StdUi.h"
#include "CGI_WiFi.h"
#include "espmissingincludes.h"
#include "SCDE.h"
//#include "CGI_TiStCfg.h"
*/

#include <SCDE_s.h>

#include "WebIf_Module_s.h"
//#include "WebIf_Module.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



#include "ServAPCfg_tpl.h"



/*
 *--------------------------------------------------------------------------------------------------
 *FName: ServAPCfg_tpl
 * Desc: Template %X% replacement Code for the ServAPCfg.htm Page
 * Info: WARNING: CHECK BUFFERSIZE
 * Para: WebIF_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *       char *token -> ptr to string which contains the token to replace
 *       void **arg -> ???
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
ServAPCfg_tpl(WebIf_HTTPDConnSlotData_t *conn, char *token, void **arg) 
{
  char buff[1200];	
  if (token == NULL) return;


  switch(token[0])
	{
/*
//--------------------------------------------------------------------------------------------------

	case '?':				// '?' -> Device Name requested!
	{
	// get Devicename from current Service AP settings
	struct softap_config softap_config;
	wifi_softap_get_config (&softap_config);

	os_strcpy(buff,
		(char*) &softap_config.ssid);
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'A':				// 'A*' -> Service Access Point Settings
	{
	// get curren AP settings
	struct softap_config softap_config;

  	wifi_softap_get_config (&softap_config);

	switch(token[1])
		{

		case '1':				// 'A1'  -> SSID MAX 32 char
		os_strcpy(buff,
			(char*) &softap_config.ssid);
		break;

  		case '2':				// 'A2'  -> Password MAX 15 char
		os_strcpy(buff,
			(char*) &softap_config.password);
		break;

  		case '3':				// 'A3'  -> WLAN Channel Selection -> "A3"
		SelectA((char*) &buff,
			Channel,
			softap_config.channel,
			"A3",
			NULL);
		break;

  		case '4':				// 'A4'  -> Max AP Connections Selection -> "A4"
		SelectA((char*) &buff,
			MaxConn,
			softap_config.max_connection,
			"A4",
			NULL);
		break;

  		case '5':				// 'A5'  -> AUTH_MODE Selection -> "A5"
		SelectA((char*) &buff,
			Auth_M,
			softap_config.authmode,
			"A5",
			NULL);
		break;

  		case '6':				// 'A6'  -> SSID Hidden/Visible Selection -> "A6"
		SelectA((char*) &buff,
			SSID_H,
			softap_config.ssid_hidden,
			"A6",
			NULL);
		break;

		case '7':				// 'A7'  -> Beacon Interval Selection
		os_sprintf(buff,
			"%05d",
			softap_config.beacon_interval);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'B':				// 'B*' -> SOFTAP_IF DHCP Start + End IP
	{
	struct dhcps_lease dhcps_lease;

  	wifi_softap_get_dhcps_lease(&dhcps_lease);

	switch(token[1])
		{

		case '1':				// 'B1'  -> DHCP Start IP Oktett 1
		os_sprintf(buff,
			"%03d",
			(dhcps_lease.start_ip.addr) & 0xff);
		break;

		case '2':				// 'B2'  -> DHCP Start IP Oktett 2
		os_sprintf(buff,
			"%03d",
			(dhcps_lease.start_ip.addr >> 8) & 0xff);
		break;

		case '3':				// 'B3'  -> DHCP Start IP Oktett 3
		os_sprintf(buff,
			"%03d",
			(dhcps_lease.start_ip.addr >> 16) & 0xff);
		break;

		case '4':				// 'B4'  -> DHCP Start IP Oktett 4
		os_sprintf(buff,
			"%03d",
			dhcps_lease.start_ip.addr >> 24);
		break;

		case '5':				// 'B5'  -> DHCP End IP Oktett 1
		os_sprintf(buff,
			"%03d",
			dhcps_lease.end_ip.addr & 0xff);
		break;

		case '6':				// 'B6'  -> DHCP End IP Oktett 2
		os_sprintf(buff,
			"%03d",
			(dhcps_lease.end_ip.addr >> 8) & 0xff);
		break;

		case '7':				// 'B7'  -> DHCP End IP Oktett 3
		os_sprintf(buff,
			"%03d",
			(dhcps_lease.end_ip.addr >> 16) & 0xff);
		break;

		case '8':				// 'B8'  -> DHCP End IP Oktett 4
		os_sprintf(buff,
			"%03d",
			dhcps_lease.end_ip.addr >> 24);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'C':				// 'C*' -> SOFTAP_IF -> IP Adress
	{
  	struct ip_info ip_info;

  	wifi_get_ip_info(SOFTAP_IF, &ip_info);

	switch(token[1])
		{

		case '1':				// 'C1'  -> IP-Adresse Oktett 1
		os_sprintf(buff,
			"%03d",
			ip_info.ip.addr & 0xff);
		break;

		case '2':				// 'C2'  -> IP-Adresse Oktett 2
		os_sprintf(buff,
			"%03d",
			(ip_info.ip.addr >> 8) & 0xff);
		break;

		case '3':				// 'C3'  -> IP-Adresse Oktett 3
		os_sprintf(buff,
			"%03d",
			(ip_info.ip.addr >> 16) & 0xff);
		break;

		case '4':				// 'C4'  -> IP-Adresse Oktett 4
		os_sprintf(buff,
			"%03d",
			ip_info.ip.addr >> 24);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'D':				// 'D*' -> SOFTAP_IF -> Netmask Adress
	{
  	struct ip_info ip_info;

  	wifi_get_ip_info(SOFTAP_IF, &ip_info);

	switch(token[1])
		{

		case '1':					// 'D1'  -> Netmask Oktett 1
		os_sprintf(buff,
			"%03d",
			ip_info.netmask.addr & 0xff);
		break;

		case '2':					// 'D2'  -> Netmask Oktett 2
		os_sprintf(buff,
			"%03d",
			(ip_info.netmask.addr >> 8) & 0xff);
		break;

		case '3':					// 'D3'  -> Netmask Oktett 3
		os_sprintf(buff,
			"%03d",
			(ip_info.netmask.addr >> 16) & 0xff);
		break;

		case '4':					// 'D4'  -> Netmask Oktett 4
		os_sprintf(buff,
			"%03d",
			ip_info.netmask.addr >> 24);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'E':				// 'E*' -> SOFTAP_IF -> Gateway Adress
	{
  	struct ip_info ip_info;

  	wifi_get_ip_info(SOFTAP_IF, &ip_info);

	switch(token[1])
		{

		case '1':				// 'E1'  -> Gateway Oktett 1
		os_sprintf(buff,
			"%03d",
			ip_info.gw.addr & 0xff);
		break;

		case '2':				// 'E2'  -> Gateway Oktett 2
		os_sprintf(buff,
			"%03d",
			(ip_info.gw.addr >> 8) & 0xff);
		break;

		case '3':				// 'E3'  -> Gateway Oktett 3
		os_sprintf(buff,
			"%03d",
			(ip_info.gw.addr >> 16) & 0xff);
		break;

		case '4':				// 'E4'  -> Gateway Oktett 4
		os_sprintf(buff,
			"%03d",
			ip_info.gw.addr >> 24);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'F':				// 'F*' -> SOFTAP_IF -> Mac Adress
	{
  	uint8 macaddr[8];
//	os_memcpy(&macaddr,&SysCfgRamFlashMirrored->Apmacaddr,8);// woher nehmen?

	wifi_get_macaddr(SOFTAP_IF, (uint8 *) &macaddr);

	switch(token[1])
		{

		case '1':				// 'F1'  -> MAC Oktett 1
		os_sprintf(buff,
			"%02X",
			macaddr[0]);
		break;
	
		case '2':				// 'F2'  -> MAC Oktett 2
		os_sprintf(buff,
			"%02X",
			macaddr[1]);
		break;

		case '3':				// 'F3'  -> MAC Oktett 3
		os_sprintf(buff,
			"%02X",
			macaddr[2]);
		break;

		case '4':				// 'F4'  -> MAC Oktett 4
		os_sprintf(buff,
			"%02X",
			macaddr[3]);
		break;

		case '5':				// 'F5'  -> MAC Oktett 5
		os_sprintf(buff,
			"%02X",
			macaddr[4]);
		break;

		case '6':				// 'F6'  -> MAC Oktett 6
		os_sprintf(buff,
			"%02X",
			macaddr[5]);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'G':				// 'G'  -> List Stations that are connected to the Soft AP
 	{
  	struct station_info *station = wifi_softap_get_station_info();

  	struct station_info *next_station;

	int len = 0;
	int StationsCnt = 0;

	// loop connected stations ...
  	while (station)
		{
		len += os_sprintf(buff+len,
			"<h2>Connected Station %d - BSSID: "
			MACSTR
			" - IP: "
			IPSTR
			"</h2>",
			++StationsCnt,
			MAC2STR(station->bssid),
			IP2STR(&station->ip));

		next_station = STAILQ_NEXT(station,next);
		os_free(station);
		station = next_station;
		}

	// In case there were NO stations connected -> special text
	if (StationsCnt == 0)
		{
		len += os_sprintf(buff+len,
			"<h2>Actually no Stations are connected!</h2>");
		}

	# if SCDE_WIFI_DBG >= 3
	os_printf("Fits SI %d in buff?\n",len);
	# endif
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'H':				// 'H'  -> Enable / Disable Soft AP selection -> "X2"
	RadioA((char*) &buff,
		DisEn,
		(wifi_get_opmode() >> 1), // Bit1 = SoftAP Mode
		"X2",
		"ServAPCfg.cgi");
	break;

//--------------------------------------------------------------------------------------------------

	case 'I':				// 'I'  -> Enable / Disable DHCP Server selection -> "X9"
	RadioA((char*) &buff,
		DisEn,
		wifi_softap_dhcps_status(),
		"X9",
		"ServAPCfg.cgi");
	break;

//--------------------------------------------------------------------------------------------------
*/
	default:				// Not found? Zero terminate!
	buff[0] = 0;
	break;

//--------------------------------------------------------------------------------------------------
	}

  HTTPD_Send_To_Send_Buffer(conn, buff, -1);
}



