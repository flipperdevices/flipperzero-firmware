//*****************************************************************************
//! \file mqtt_interface.h
//! \brief Paho MQTT to WIZnet Chip interface Header file.
//! \details The process of porting an interface to use paho MQTT.
//! \version 1.0.0
//! \date 2016/12/06
//! \par  Revision history
//!       <2016/12/06> 1st Release
//!
//! \author Peter Bang & Justin Kim
//! \copyright
//!
//! Copyright (c)  2016, WIZnet Co., LTD.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

/* MQTT subscribe Example.... W5500 + STM32F103(IoT board)
//Include: Board configuration
#include "IoTEVB.h"

//Include: MCU peripheral Library
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"

//Include: W5500 iolibrary
#include "w5500.h"
#include "wizchip_conf.h"
#include "misc.h"

//Include: Internet iolibrary
#include "MQTTClient.h"

//Include: MCU Specific W5500 driver
#include "W5500HardwareDriver.h"

//Include: Standard IO Library
#include <stdio.h>

//Socket number defines
#define TCP_SOCKET	0

//Receive Buffer Size define
#define BUFFER_SIZE	2048

//Global variables
unsigned char targetIP[4] = {}; // mqtt server IP
unsigned int targetPort = 1883; // mqtt server port
uint8_t mac_address[6] = {};
wiz_NetInfo gWIZNETINFO = { .mac = {}, //user MAC
							.ip = {}, //user IP
							.sn = {},
							.gw = {},
							.dns = {},
							.dhcp = NETINFO_STATIC};

unsigned char tempBuffer[BUFFER_SIZE] = {};

struct opts_struct
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	enum QoS qos;
	char* username;
	char* password;
	char* host;
	int port;
	int showtopics;
} opts ={ (char*)"stdout-subscriber", 0, (char*)"\n", QOS0, NULL, NULL, targetIP, targetPort, 0 };


// @brief messageArrived callback function
void messageArrived(MessageData* md)
{
	unsigned char testbuffer[100];
	MQTTMessage* message = md->message;

	if (opts.showtopics)
	{
		memcpy(testbuffer,(char*)message->payload,(int)message->payloadlen);
		*(testbuffer + (int)message->payloadlen + 1) = "\n";
		printf("%s\r\n",testbuffer);
	}

	if (opts.nodelimiter)
		printf("%.*s", (int)message->payloadlen, (char*)message->payload);
	else
		printf("%.*s%s", (int)message->payloadlen, (char*)message->payload, opts.delimiter);
}


// @brief 1 millisecond Tick Timer setting
void NVIC_configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(72000);
	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // Highest priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// @brief 1 millisecond Tick Timer Handler setting
void SysTick_Handler(void)
{
	MilliTimer_Handler();
}

int main(void)
{
	led_ctrl led1,led2;
	int i;
	int rc = 0;
	unsigned char buf[100];
	//Usart initialization for Debug.
	USART1Initialze();
		printf("USART initialized.\n\r");

	I2C1Initialize();
		printf("I2C initialized.\n\r");

	MACEEP_Read(mac_address,0xfa,6);

	printf("Mac address\n\r");
	for(i = 0 ; i < 6 ; i++)
	{
		printf("%02x ",mac_address[i]);
	}
	printf("\n\r");

	//LED initialization.
	led_initialize();
	led1 = led2 = ON;

	led2Ctrl(led2);
	led1Ctrl(led1);

	//W5500 initialization.
	W5500HardwareInitilize();
		printf("W5500 hardware interface initialized.\n\r");

	W5500Initialze();
		printf("W5500 IC initialized.\n\r");

	//Set network informations
	wizchip_setnetinfo(&gWIZNETINFO);

	setSHAR(mac_address);

	print_network_information();

	Network n;
	MQTTClient c;

	NewNetwork(&n, TCP_SOCKET);
	ConnectNetwork(&n, targetIP, targetPort);
	MQTTClientInit(&c,&n,1000,buf,100,tempBuffer,2048);

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = opts.clientid;
	data.username.cstring = opts.username;
	data.password.cstring = opts.password;

	data.keepAliveInterval = 60;
	data.cleansession = 1;

	rc = MQTTConnect(&c, &data);
	printf("Connected %d\r\n", rc);
	opts.showtopics = 1;

	printf("Subscribing to %s\r\n", "hello/wiznet");
	rc = MQTTSubscribe(&c, "hello/wiznet", opts.qos, messageArrived);
	printf("Subscribed %d\r\n", rc);

    while(1)
    {
    	MQTTYield(&c, data.keepAliveInterval);
    }
}
*/

#ifndef __MQTT_INTERFACE_H_
#define __MQTT_INTERFACE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * @brief MQTT MilliTimer handler
 * @note MUST BE register to your system 1m Tick timer handler
 */
void MilliTimer_Handler(void);

/*
 * @brief Timer structure
 */
typedef struct Timer Timer;
struct Timer {
	unsigned long systick_period;
	unsigned long end_time;
};

/*
 * @brief Network structure
 */
typedef struct Network Network;
struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, long);
	int (*mqttwrite) (Network*, unsigned char*, int, long);
	void (*disconnect) (Network*);
};

/*
 * @brief Timer function
 */
void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

/*
 * @brief Network interface porting
 */
int w5x00_read(Network*, unsigned char*, int, long);
int w5x00_write(Network*, unsigned char*, int, long);
void w5x00_disconnect(Network*);
void NewNetwork(Network* n, int sn);
int ConnectNetwork(Network* n, uint8_t* ip, uint16_t port);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_INTERFACE_H_
