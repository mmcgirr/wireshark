

/****************************************************************************************/
/*    Copyright 2006                                                                                                          */
/*    inc.jet.inc                                                                                                             */
/*    All Rights Reserved                                                                                                     */
/*                                                                                                                                  */
/*                                                                                                                                  */
/*    Author:           Roger Gallic                                                                                    */
/*                                                                                                                                  */
/*    Last Modified:    01/04/06    Roger Gallic      Added this header                                     */
/*                                                                                                                                  */
/*                                                                                                                                  */
/*                                                                                                                                  */
/*                                                                                                                                  */
/****************************************************************************************/



#ifdef WIN32
#include <stdafx.h>
#include "Winmisc_old.h"
#else

#endif

#include "BackGroundTask.h"

#include "Fpgaprgm.h"
#include "messages.h"
#include "main.h"
#include "Log.h"

/**Currently recognized UDP commands:    :OER: 4-1-03
  L: Set LEDs
  K: Set LEDs using new structure
  D: Read Dip switches
*/

//:OER: 4-1-03
struct SetLedColorUdp {
       DWORD  m_dwKeyValue;   //0x4255524E
       BYTE   m_bAction;      //'L'
       BYTE   m_bLedColor;    //0..4: off, on, slow, med, fast
       WORD   m_wOnTimer;     //0:Continuous, 1..65535:mSec of on time
       ConfigRecord m_cf;
};

/**Change to this: */
struct SetLedColor_UDP {
       DWORD  m_dwKeyValue;   //0x4255524E
       BYTE   m_bAction;      //'K'
       BYTE   mac[6];       //MAC of destination device address
       BYTE   m_bLedColor;    //0..4: off, on, slow, med, fast
       WORD   m_wOnTimer;     //0:Continuous, 1..65535:mSec of on time
};

//:OER: 4-1-03
struct SetIpValues {
       DWORD  m_dwKeyValue;   //0x4255524E
       BYTE   m_bAction;      //'I'
       BYTE   mac[6];     //MAC of destination device address
       BYTE   ip[4];      //IP address
       BYTE   mask[4];    //IP mask
       BYTE   gateway[4]; //Gateway
};

//:OER: 4-1-03
struct ReadIpValues {
       DWORD  m_dwKeyValue;   //0x4255524E
       BYTE   m_bAction;      //'J'
       BYTE   mac[6];     //MAC of destination device address
       BYTE   ip[4];      //IP address
       BYTE   mask[4];    //IP mask
       BYTE   gateway[4]; //Gateway
};

//:OER: 4-1-03 This is the response packet for a query of 'ReadIPValues'
struct ResponseIpValues {
       char*  keyValue;   //"IPDATA";
       BYTE   ip[4];      //IP address
       BYTE   mask[4];    //IP mask
       BYTE   gateway[4]; //Gateway
};

int macAddressMatches(ConfigRecord cr);
int compareToMacAddress(BYTE *mac);

//////////////////////////////////////////////


BackGroundTask::BackGroundTask()
{
      Led4Flag = LED_OFF;
      LedState = false;
      errorCode = 0;
      taskRun = 0;
      taskExit = 0;
}

BackGroundTask::~BackGroundTask()
{
      taskRun = 0;
      while(0 == taskExit){
            OSTimeDly(1);
      }
}


int BackGroundTask::Run()
{

      OS_FIFO_EL *fifoElement = 0;
      OSFifoInit(& MyFifo);
      RegisterUDPFifo(0x1100,& MyFifo);
      taskRun = 1;
      taskExit = 0;
      //
      // Do nothing by default.
      //
      do
      {
            if ((fifoElement = OSFifoPend(&MyFifo,(short unsigned int)(0.5*TICKS_PER_SECOND))) != NULL)
            {
                  if(0 != taskRun){
                        Log::logPtr->LogMsg("GOT UDP MESSAGE!!",Log::MISC);
                        UDPPacket pkt((PoolPtr)fifoElement);
                        unsigned char *pData = pkt.GetDataBuffer();
                        if ((*(PDWORD)pData)==0x4255524E)         // check key
                        {
                              pData+=4;

                              if (pData[0] == 'L')
                              {           //Set LED color //Added 4-1-03 :OER:
                                    handleLedRequest(pData);
                              }

                              else
                                    if (pData[0] == 'I')
                              {
                                    Log::logPtr->LogMsg("Replace IP Values...",Log::MISC);
                                    replaceIPValues(pData);
                              }
                              else
                                    if (pData[0] == 'J')
                              { 
                                    readIPValues(pkt);
                              }
                              else
                                    if (pData[0] == 'D')
                              { 
                                    handleDipSwitchReadRequest(pkt);
                              }
                              else
                              {
                                    Log::logPtr->LogMsg("Unrecognized UDP port request. Command = %C",Log::ERROR_F ,pData[0]);
                              }
                        }
                        else {
                              Log::logPtr->LogMsg("Bad UDP key?",Log::ERROR_F);
                        }
                  }
            }
            if(0 != taskRun){
                  stepLedStateMachine();
            }

      }while (taskRun);
      taskExit = 1;
      return 0;
}

int BackGroundTask::handleLedRequest(unsigned char *pData)
{
      Log::logPtr->LogMsg("\nRecognized action %c",Log::MISC, pData[0]); //DEBUG***
      pData++;
      BYTE ledColor = *pData++;
      BYTE bb = *pData++;
      WORD ledTimeout = (bb<<8) + *pData++;
      int len = sizeof(ConfigRecord);

      ConfigRecord cr;
      unsigned char *pCR = (unsigned char *)&cr;
      for (int i=0; i < len; i++)   *pCR++ = pData[i];

      /*DEBUG ***/
      Log::logPtr->LogMsg("\ngConfig=%x %x %x %x %x %x",Log::MISC, gConfigRec.mac_address[0],
            gConfigRec.mac_address[1], gConfigRec.mac_address[2],
            gConfigRec.mac_address[3], gConfigRec.mac_address[4],
            gConfigRec.mac_address[5]);

      Log::logPtr->LogMsg("\nConfig=%x %x %x %x %x %x",Log::MISC, cr.mac_address[0], cr.mac_address[1],
            cr.mac_address[2], cr.mac_address[3], cr.mac_address[4], cr.mac_address[5]);
      /* *** */

      if (macAddressMatches(cr))
      {
            //SET THE LED'S HERE using: ledColor and ledTimeout ***:OER:***
            Log::logPtr->LogMsg("\n*Led Blink A* (color=%d, time=%d)",Log::MISC, (int)ledColor, ledTimeout); // Debug***
            Led4Flag = (LedRequestStates)ledColor;
      }
      return 0;
}

int BackGroundTask::handleDipSwitchReadRequest(UDPPacket pkt)
{
      //Read Dip switches //Added 4-1-03 :OER:
      ///Log::logPtr->LogMsg("\nRecognized action %c",Log::TRACE_3, pData[0]); //DEBUG***
      int result;
      ConfigRecord cr;
      unsigned char *pCR = (unsigned char *)&cr;
      int len = sizeof(ConfigRecord);
      unsigned char *pData = pkt.GetDataBuffer();
      
      for (int i=0; i < len; i++)   *pCR++ = *pData++;
      if (macAddressMatches(cr))
      {
            pkt.SetSourcePort(UDP_NETBURNERID_PORT);
            pkt.SetDestinationPort(UDP_NETBURNERID_PORT);
            pkt.ResetData();
            pkt.AddData("DIPSW=");
            unsigned char swVals = getdipsw(); // ***Verify this is correct call
            pkt.AddDataByte(swVals);
            for (volatile int i=0; i< (50*(int)gConfigRec.mac_address[5]); i++)
#ifndef WIN32
            asm (" NOP"); //Put in some hold off
#endif
            result = 1;
      }
      return 0;
}

int BackGroundTask::replaceIPValues(unsigned char *pData)
{
      //Set IP values: Address, Mask, Gateway
      ///Log::logPtr->LogMsg("\nRecognized action %c",Log::TRACE_3, pData[0]); //DEBUG***
      pData++;

      //Compare the MAC Address:
      BYTE mac[6];
      unsigned char *p = (unsigned char *)&mac;
      for (int i=0; i < 6; i++) *p++ = *pData++;

      if (compareToMacAddress(mac))
      {     //ie: If is addressed to this device
            ///Log::logPtr->LogMsg("\nMAC recognized",Log::TRACE_3); //DEBUG***
            //Get current gConfigRecord values:
            ConfigRecord newCr;
            int cfgLen = sizeof(ConfigRecord);
            BYTE *pOrigCr = (BYTE *)&gConfigRec;
            BYTE *pNewCr = (BYTE *)&newCr;
            for (BYTE i=0; i < cfgLen; i++)
                  *pNewCr++ = *pOrigCr++;

            //Get the new settings:
            BYTE* p;
            p = (BYTE *)&(newCr.ip_Addr); //IP Address
            for (int i=0; i<4; ++i) *p++ = *pData++;

            p = (BYTE *)&(newCr.ip_Mask); //IP Mask
            for (int i=0; i<4; ++i) *p++ = *pData++;

            p = (BYTE *)&(newCr.ip_GateWay); //Gateway
            for (int i=0; i<4; ++i) *p++ = *pData++;

            char s[20];
            BYTE c[4];

            BYTE* p0= (BYTE *)&(newCr.ip_Addr); //IP Address
            for (BYTE i=0; i<4; ++i) c[i] =  *p0++;
            sprintf(s, "%d.%d.%d.%d",(int)c[0],(int)c[1],(int)c[2],(int)c[3]);
            Log::logPtr->LogMsg("\nIP=%s",Log::MISC, s); //DEBUG ***
            EthernetIP = AsciiToIp(s);

            p0= (BYTE *)&(newCr.ip_Mask);
            for (BYTE i=0; i<4; ++i) c[i] =  *p0++;
            sprintf(s, "%d.%d.%d.%d",(int)c[0],(int)c[1],(int)c[2],(int)c[3]);
            EthernetIpMask = AsciiToIp(s);
            Log::logPtr->LogMsg("\nMASK=%s",Log::MISC, s); //DEBUG ***

            p0= (BYTE *)&(newCr.ip_GateWay);
            for (BYTE i=0; i<4; ++i) c[i] =  *p0++;
            sprintf(s, "%d.%d.%d.%d",(int)c[0],(int)c[1],(int)c[2],(int)c[3]);
            EthernetIpGate = AsciiToIp(s);
            Log::logPtr->LogMsg("\nGATEWAY=%s",Log::MISC, s); //DEBUG ***

            //Update the record:
            UpdateConfigRecord(&newCr);
            //  ForceReboot();      //<--Necessary? (try both ways)
      }
      return 0;
}

int BackGroundTask::readIPValues(UDPPacket pkt)
{
      int result = 0;
      //Read IP values: Address, Mask, Gateway
      unsigned char *pData = pkt.GetDataBuffer();
      //Log::logPtr->LogMsg("\nRecognized action %c",Log::TRACE_3, pData[0]); //DEBUG***

      pData++;

      //Compare the MAC Address:
      BYTE mac[6];
      unsigned char *p = (unsigned char *)&mac;
      for (int i=0; i < 6; i++) *p++ = *pData++;

      if (compareToMacAddress(mac))
      { //ie: If is addressed to this device
            //Get current gConfigRecord values:
            pkt.SetSourcePort(UDP_NETBURNERID_PORT);
            pkt.SetDestinationPort(UDP_NETBURNERID_PORT);
            pkt.ResetData();
            pkt.AddData("IPDATA"); // ???

            //Copy info to output packet:
            BYTE *p;
            p = (BYTE *)&gConfigRec.ip_Addr;
            for (int i=0; i<4; ++i) pkt.AddDataByte(*p++);
            p = (BYTE *)&gConfigRec.ip_Mask;
            for (int i=0; i<4; ++i) pkt.AddDataByte(*p++);
            p = (BYTE *)&gConfigRec.ip_GateWay;
            for (int i=0; i<4; ++i) pkt.AddDataByte(*p++);

            for (volatile int i=0; i< (50*(int)gConfigRec.mac_address[5]); i++)
#ifndef WIN32
                  asm (" NOP"); //Put in some hold off
#endif
            result = 1;
      }
      return 0;
}



int BackGroundTask::stepLedStateMachine()
{
      //
      // If an error code has been registered, it takes priority over other
      // flashes.
      //
      if(0 != errorCode){

            LedState = (0 == LedState) ? 1:0;   // Flash Red
            FlashLed(LedState,0);

      } else if(Led4Flag == LED_OFF){

            //
            // Don't waste processor time if LED is off and is to remain off.
            //
            if(0 != LedState){
                  LedState = 0;     
                  FlashLed(LedState,0);
            }

      } else if(Led4Flag == LED_FLASH_RED){
            
            LedState = (0 == LedState) ? 1:0;         // Flash Red
            FlashLed(LedState,0);

      } else if(Led4Flag == LED_FLASH_RED_GREEN){     

            LedState = (1 == LedState) ? 2:1;         // Flash Red/Green
            FlashLed(LedState,0);
      
      } else if(Led4Flag == LED_FLASH_GREEN){

            LedState = (0 == LedState) ? 2:0;         // Flash Green
            FlashLed(LedState,0);

      } else if(Led4Flag == LED_SOLID_GREEN){         // Solid Green

            if(2 != LedState){
                  LedState = 2;     
                  FlashLed(LedState,0);
            }
      }
      return 0;
}


//:OER: 4-1-03
//**Returns true if mac address in given ConfigRecord matches the gConfigRecord*/
int macAddressMatches(ConfigRecord cr) {
  return(
    (cr.mac_address[0]==gConfigRec.mac_address[0]) &&
    (cr.mac_address[1]==gConfigRec.mac_address[1]) &&
    (cr.mac_address[2]==gConfigRec.mac_address[2]) &&
    (cr.mac_address[3]==gConfigRec.mac_address[3]) &&
    (cr.mac_address[4]==gConfigRec.mac_address[4]) &&
    (cr.mac_address[5]==gConfigRec.mac_address[5]));
}

//:OER: 4-1-03
// **Returns true if the given mac address matches the gConfigRecord mac address*/
int compareToMacAddress(BYTE *mac) {
  return(
    (mac[0]==gConfigRec.mac_address[0]) &&
    (mac[1]==gConfigRec.mac_address[1]) &&
    (mac[2]==gConfigRec.mac_address[2]) &&
    (mac[3]==gConfigRec.mac_address[3]) &&
    (mac[4]==gConfigRec.mac_address[4]) &&
    (mac[5]==gConfigRec.mac_address[5]));
}

void BackGroundTask::postError(int eCode)
{
      errorCode = eCode;
}

void BackGroundTask::clearError()
{
      errorCode = 0;
}

int BackGroundTask::postLedRequest(LedRequestStates ledRequest)
{
      // This should be done using te fifo. The fifo is currently
      // dedicated to passing UDP messages. Use this technique for
      // now.

      Led4Flag = ledRequest;

      return 0;
}




Roger Gallic
Senior Software Engineer
Norwix Marking Systems

