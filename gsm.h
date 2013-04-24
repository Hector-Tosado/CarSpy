/*
 * gsm.h
 *
 *  Created on: 03/26/2013
 *      Author: Hector Tosado
 */

#ifndef GSM_H_
#define GSM_H_

void checkConnection();
void contextActivation();
void TCPConnect();
void SendData(const unsigned char* data_to_be_sent);
void makeCall();
void sendTextMessage();

extern void UARTSend(unsigned long device, const unsigned char *pucBuffer, unsigned long ulCount);
#endif /* GSM_H_ */

