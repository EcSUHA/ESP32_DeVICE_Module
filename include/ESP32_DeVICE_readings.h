#ifndef ESP32_DeVICE_READINGS_H
#define ESP32_DeVICE_READINGS_H



// -------------------------------------------------------------------------------------------------



// ESP32_DeVICE - embedded reading types
enum ESP32_DeVICE_READING_TYPES {	        // #XX desc for debugging
  ESP32_DeVICE_STATE = 0			        // #00 start
 ,ESP32_DeVICE_PRESSURE                     // #01 pressure pa, stored as float
 ,ESP32_DeVICE_ALTITUDE                     // #02 altitude m, stored as float
 ,ESP32_DeVICE_TEMPERATURE          	    // #03 temperature °C, stored as int16_t
};



// -------------------------------------------------------------------------------------------------



//
string_t ESP32_DeVICE_Get_Reading_State_As_Text(reading2_t *p_state_reading);

//
string_t ESP32_DeVICE_Get_Reading_WSAP_As_Text(reading2_t *p_WSAP_reading);

//
string_t ESP32_DeVICE_Get_Reading_WSAP_Beacon_Interval_As_Text(reading2_t *p_WSAP_Beacon_Interval_reading);



// -------------------------------------------------------------------------------------------------



//
bool ESP32_DeVICE_Set_Reading_WSAP_From_Text(reading2_t *p_WSAP_Beacon_Interval_reading, char *p_value_as_text);

//
bool ESP32_DeVICE_Set_Reading_WSAP_Beacon_Interval_From_Text(reading2_t *p_WSAP_Beacon_Interval_reading, char* p_value_as_text);



// -------------------------------------------------------------------------------------------------



// ESP32_DeVICE - embedded reading types
reading_type_t ESP32_DeVICE_reading_types[];



#endif // ESP32_DeVICE_READINGS_H
