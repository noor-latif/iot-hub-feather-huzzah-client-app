#ifndef MQTT_PROTOCOL_H
#define MQTT_PROTOCOL_H

/* 
 * MQTT Protocol setup
 */
#include "iothub_transport_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif
    extern const TRANSPORT_PROVIDER* MQTT_Protocol(void);

#ifdef __cplusplus
}
#endif

#endif /*MQTT_PROTOCOL_H*/
