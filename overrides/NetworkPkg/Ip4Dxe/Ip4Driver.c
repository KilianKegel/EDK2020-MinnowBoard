/** @file
  The driver binding and service binding protocol for IP4 driver.

Copyright (c) 2005 - 2019, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2015 Hewlett-Packard Development Company, L.P.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Ip4Impl.h"
#include <stdio.h>      // include if required
#include <stdlib.h>     // include if required
#include <string.h>     // include if required
#include <ctype.h>      // include if required
#include <wctype.h>     // include if required
#include <CDE.h>        // include to have CDE base definitions, e.g. CDEMOFINE()
extern char *strefierror(EFI_STATUS errcode);   // declare ANSI C strerror() counterpart for UEFI strefierror()

enum { HDADDRSIZE64, HDADDRSIZE32, HDADDRSIZE16, HDADDRSIZE8, HDADDRSIZE0 };
enum { HDELMSIZE1, HDELMSIZE2, HDELMSIZEx0 = 0, HDELMSIZE4 = 3, HDELMSIZEx1 = 0, HDELMSIZEx2 = 0, HDELMSIZEx3 = 0, HDELMSIZE64 = 7 };

typedef union _UNIDUMPPARM {
    unsigned reg;
    struct {
        unsigned elmsizemin1 : 3;   /*!<element size minus one, only 0,1,3,7                */
        unsigned nAddrSize : 3; /*!<0,1,2,3,4 with:
                                0 == "%016llX: "
                                1 == "%08llX: "
                                2 == "%04llX: "
                                3 == "%02llX: "
                                4 == ""                                                     */
        unsigned nBytesPerLine : 7;/*!< nBytesPerLine minus one bytes per line - 0 == 16,   */
        unsigned fNoAscii : 1;  /*!<append NO ASCII characters                              */
        unsigned fBaseOfs : 1;  /*!<base and offset, offset only otherwise                  */
        unsigned fNoDash : 1;  /*!<print dash "-" in between                               */
        unsigned pitch : 8;     /*!<pitch between two consecutive elements fo size elmsize  */
    }bit;
}UNIDUMPPARM;

static const char szTwoSpaces[] = { 0x20,'\0' };
static const char szOneSpace[] = { 0x20,'\0' };

/*!
    @fn static int PrintAscii(char *pBuffer, unsigned elmsize, unsigned cToPrint,unsigned char *pTextLineBuf)

    @brief prints the ASCII interpretation binary byte/word/dword/qword

    @details Features:

    @param[in] *pBuffer : pointer to binary buffer
    @param[in] elmsize : element size
    @param[in] cToPrint : count to print
    @param[in] *pTextLineBuf : pointer to text buffer

    @return 0
*/
static int PrintAscii(char* pBuffer, unsigned elmsize, unsigned cToPrint, unsigned char* pTextLineBuf) {
    unsigned char* pb = (unsigned char*)&pBuffer[0];
    unsigned short* pw = (unsigned short*)&pBuffer[0];
    unsigned int* pdw = (unsigned int*)&pBuffer[0];
    unsigned long long* pqw = (unsigned long long*) & pBuffer[0];
    unsigned long long qwLit2Big;/*!< little endian to big endian translation buffer*/
    unsigned char* pLit2Big = (unsigned char*)&qwLit2Big;
    unsigned j;

#define PRINTREVERSE for (k = elmsize - 1 ; k != (unsigned)-1 ; k--){\
                             sprintf(&pTextLineBuf[strlen(pTextLineBuf)],"%c", isalnum(pLit2Big[k]) ? 0xFF & pLit2Big[k] : '.'); \
                         }\
                         if (elmsize - 1){/*!< add space between ASCII char, except in 8-bit format*/\
                             sprintf(&pTextLineBuf[strlen(pTextLineBuf)]," ");\
                         }// END PRINTREVERSE

    switch (elmsize) {
        unsigned k;
        case sizeof(char) :
            for (j = 0; j < cToPrint; j += elmsize) {
                *((unsigned char*)pLit2Big) = 0xFF & pb[j / elmsize];
                PRINTREVERSE;
            }
        break;

        case sizeof(short) :
            for (j = 0; j < cToPrint; j += elmsize) {
                *((unsigned short*)pLit2Big) = 0xFFFF & pw[j / elmsize];
                PRINTREVERSE;
            }
        break;

        case sizeof(int) :
            for (j = 0; j < cToPrint; j += elmsize) {
                *((unsigned int*)pLit2Big) = 0xFFFFFFFF & pdw[j / elmsize];
                PRINTREVERSE;
            }
        break;

        case sizeof(long long) :
            for (j = 0; j < cToPrint; j += elmsize) {
                *((unsigned long long*)pLit2Big) = 0xFFFFFFFFFFFFFFFFLL & pqw[j / elmsize];
                PRINTREVERSE;
            }
        break;
    }
    return 0;
}

/*!
    @fn int UniDump(UNIDUMPPARM ctrl, unsigned elmcount, unsigned long long startaddr, unsigned long long(*pfnGetElm)(unsigned long long qwAddr),unsigned (*pfnWriteStr)(char *szLine))

    @brief dump an addressrange, highly configurable

    @details Features:
        1. w/ or w/o appended ASCII translation -> UNIDUMPPARM.bit.fNoAscii
        2. byte/word/dword and qword support    -> UNIDUMPPARM.bit.elmsize
        3. configurable/enabe/disable address size printed at begin of each line -> UNIDUMPPARM.bit.nAddrSize
        4. configurable bytes per lane + 1, until 128 -> UNIDUMPPARM.bit.nBytesPerLine
        5. configurable base and offset only print    -> UNIDUMPPARM.bit.fBaseOfs
        6. configurable dash character "-" at half of the line -> UNIDUMPPARM.bit.fNoDash
        7. configurable pitch to next character -> UNIDUMPPARM.bit.pitch

    @param[in] ctrl : control word
    @param[in] elmcount : element count
    @param[in] startaddr : start address
    @param[in] pfnGetElm : get element routine
    @param[in] pfnWriteStr : call back routine to print a string

    @return 0
*/
int UniDump(UNIDUMPPARM ctrl, unsigned elmcount, unsigned long long startaddr, unsigned long long(*pfnGetElm)(unsigned long long qwAddr), unsigned (*pfnWriteStr)(char* szLine))
{

    unsigned elmsize = 1 + ctrl.bit.elmsizemin1;
    unsigned u/*<unsigned index*/;
    unsigned nLineLength = ctrl.bit.nBytesPerLine ? 1 + ctrl.bit.nBytesPerLine : 16;
    unsigned nLineLengthHalf = nLineLength / 2;
    unsigned char* pBuffer = malloc(128);
    unsigned char* pTextLineBuf = malloc(18/*max. AddressSize*/ + 128/*max. characters*/ * 4 + 5/*Dash + szTwoSpaces*/);
    unsigned char* pb = (unsigned char*)&pBuffer[0];
    unsigned short* pw = (unsigned short*)&pBuffer[0];
    unsigned int* pdw = (unsigned int*)&pBuffer[0];
    unsigned long long* pqw = (unsigned long long*) & pBuffer[0];
    char* rgszAddrXX[] = { { "%016llX: " },{ "%08llX: " },{ "%04llX: " },{ "%02llX: " },{ "" } };/*<address field size strings*/
    char* pAddrXX = rgszAddrXX[ctrl.bit.nAddrSize];

    pTextLineBuf[0] = '\0';

    for (u = 0; u < elmcount; u += elmsize) {

        if (0 == ctrl.bit.fNoDash)
            if (0 == ((u) % nLineLengthHalf) && 0 != ((u) % nLineLength))
                sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "- ");

        if (0 == (u % nLineLength))
            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], pAddrXX, u + (1 == ctrl.bit.fBaseOfs ? startaddr : 0LL));

        switch (elmsize) {
        case 1: pb[(u % nLineLength) / 1] = (unsigned char)(*pfnGetElm)(startaddr + u);
            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "%02X ", 0xFF & pb[(u % nLineLength) / 1]);
            break;
        case 2: pw[(u % nLineLength) / 2] = (unsigned short)(*pfnGetElm)(startaddr + u);
            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "%04X ", 0xFFFF & pw[(u % nLineLength) / 2]);
            break;
        case 4: pdw[(u % nLineLength) / 4] = (unsigned int)(*pfnGetElm)(startaddr + u);
            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "%08X ", 0xFFFFFFFF & pdw[(u % nLineLength) / 4]);
            break;
        case 8: pqw[(u % nLineLength) / 8] = (unsigned long long)(*pfnGetElm)(startaddr + u);
            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "%016llX ", 0xFFFFFFFFFFFFFFFFLL & pqw[(u % nLineLength) / 8]);
            break;
        }

        if (0 == ((u + elmsize) % nLineLength)) {
            //
            // print ascii values
            //
            if (0 == ctrl.bit.fNoAscii) {
                sprintf(&pTextLineBuf[strlen(pTextLineBuf)], szTwoSpaces);
                PrintAscii(&pBuffer[0], elmsize, nLineLength, &pTextLineBuf[strlen(pTextLineBuf)]);
            }
            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "\n");
            pfnWriteStr(pTextLineBuf);
            pTextLineBuf[0] = '\0';
        }
    }
    //
    // after glow - print remaining ASCII chars
    //
    if (1) {
        unsigned rem = u % nLineLength/*remining hex numbers (filled w/ space)*/;
        unsigned asc = nLineLength - rem/*ASCII characters not yet printed*/;
        unsigned hex = asc / elmsize/*HEX numbers not yet printed == asc / elmsize*/;
        unsigned cSpaces = hex * elmsize * 2 + hex/*count spaces between the single numbers, depending on printsize*/;
        unsigned cDashSpace = ctrl.bit.fNoDash ? 0 : (rem > nLineLengthHalf ? 0 : 2);/*count dash and space 0 / 2, depending on HEXDUMPPARMS.bit.fNoDash*/


        if (0 != rem && 1 == ctrl.bit.fNoAscii)
            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "\n");

        if (0 == ctrl.bit.fNoAscii) {
            if (rem) {
                unsigned x;
                for (x = 0; x < cSpaces + cDashSpace; x++) {
                    sprintf(&pTextLineBuf[strlen(pTextLineBuf)], szOneSpace);
                }
                sprintf(&pTextLineBuf[strlen(pTextLineBuf)], szTwoSpaces);
            }

            PrintAscii(&pBuffer[0], elmsize, rem, &pTextLineBuf[strlen(pTextLineBuf)]);

            sprintf(&pTextLineBuf[strlen(pTextLineBuf)], "\n");
            pfnWriteStr(pTextLineBuf);
            pTextLineBuf[0] = '\0';
        }
    }

    free(pTextLineBuf);
    free(pBuffer);

    return 0;
}

/*!
    @fn static unsigned WriteString(IN char *pszLineOfText)

    @brief print a text line

    @details

    @param[in] *pszLineOfText : pointer to ZERO terminated text line

    @return 0
*/
unsigned WriteString(char* pszLineOfText)
{

    printf(pszLineOfText);

    return 0;
};

/*!
    @fn unsigned long long GetMem32(void *pAddr)

    @brief read a DWORD from a given memory location

    @details

    @param[in] *pAddr

    @return dword read
*/
unsigned long long GetMem32(unsigned long long pAddr)
{
    unsigned int  nRet, * p = (unsigned int*)pAddr;
    nRet = 0xFFFFFFFF & *p;
    return nRet;

}

EFI_DRIVER_BINDING_PROTOCOL gIp4DriverBinding = {
  Ip4DriverBindingSupported,
  Ip4DriverBindingStart,
  Ip4DriverBindingStop,
  0xa,
  NULL,
  NULL
};

BOOLEAN  mIpSec2Installed = FALSE;

/**
   Callback function for IpSec2 Protocol install.

   @param[in] Event           Event whose notification function is being invoked
   @param[in] Context         Pointer to the notification function's context

**/
VOID
EFIAPI
IpSec2InstalledCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS    Status;
  //
  // Test if protocol was even found.
  // Notification function will be called at least once.
  //
  Status = gBS->LocateProtocol (&gEfiIpSec2ProtocolGuid, NULL, (VOID **)&mIpSec);
  if (Status == EFI_SUCCESS && mIpSec != NULL) {
    //
    // Close the event so it does not get called again.
    //
    gBS->CloseEvent (Event);

    mIpSec2Installed = TRUE;
  }
}

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  The entry point for IP4 driver which install the driver
  binding and component name protocol on its image.

  @param[in]  ImageHandle           The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.

**/
//EFI_STATUS
//EFIAPI
//Ip4DriverEntryPoint (
//  IN EFI_HANDLE             ImageHandle,
//  IN EFI_SYSTEM_TABLE       *SystemTable
//  )
INT32 main(INT32 Argc, CHAR8 **Argv)
{
  VOID            *Registration;
  EFI_HANDLE        ImageHandle = (void*)Argv[-2];
  EFI_SYSTEM_TABLE* SystemTable = (void*)Argv[-1];

  /*

        From now on all ANSI C functions listed in https://github.com/tianocore/edk2-staging/blob/CdePkg/implemented.md#validation-status
        are available to the driver, additionally to the ordinary UEFI API and library interface

  */
  CDEMOFINE/*MOduleFIleliNE*/((MFNINF(1) "##################################################################\n"));
  CDEMOFINE/*MOduleFIleliNE*/((MFNINF(1) "########################## Welcome to the CdePkg Library for UEFI POST drivers\n"));
  CDEMOFINE/*MOduleFIleliNE*/((MFNINF(1) "##################################################################\n"));
  

  EfiCreateProtocolNotifyEvent (
    &gEfiIpSec2ProtocolGuid,
    TPL_CALLBACK,
    IpSec2InstalledCallback,
    NULL,
    &Registration
    );

  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gIp4DriverBinding,
           ImageHandle,
           &gIp4ComponentName,
           &gIp4ComponentName2
           );
}

/**
  Test to see if this driver supports ControllerHandle. This service
  is called by the EFI boot service ConnectController(). In
  order to make drivers as small as possible, there are a few calling
  restrictions for this service. ConnectController() must
  follow these calling restrictions. If any other agent wishes to call
  Supported() it must also follow these calling restrictions.

  @param[in]  This                Protocol instance pointer.
  @param[in]  ControllerHandle    Handle of device to test
  @param[in]  RemainingDevicePath Optional parameter use to pick a specific child
                                  device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

**/
EFI_STATUS
EFIAPI
Ip4DriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  * This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     * RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS                Status;

  //
  // Test for the MNP service binding Protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiManagedNetworkServiceBindingProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Test for the Arp service binding Protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiArpServiceBindingProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  return Status;
}

/**
  Clean up a IP4 service binding instance. It will release all
  the resource allocated by the instance. The instance may be
  partly initialized, or partly destroyed. If a resource is
  destroyed, it is marked as that in case the destroy failed and
  being called again later.

  @param[in]  IpSb               The IP4 service binding instance to clean up

  @retval EFI_SUCCESS            The resource used by the instance are cleaned up
  @retval other                  Failed to clean up some of the resources.

**/
EFI_STATUS
Ip4CleanService (
  IN IP4_SERVICE            *IpSb
  );


/**
  Create a new IP4 driver service binding private instance.

  @param  Controller         The controller that has MNP service binding
                             installed
  @param  ImageHandle        The IP4 driver's image handle
  @param  Service            The variable to receive the newly created IP4
                             service.

  @retval EFI_OUT_OF_RESOURCES   Failed to allocate some resource
  @retval EFI_SUCCESS            A new IP4 service binding private is created.
  @retval other                  Other error occurs.

**/
EFI_STATUS
Ip4CreateService (
  IN  EFI_HANDLE            Controller,
  IN  EFI_HANDLE            ImageHandle,
  OUT IP4_SERVICE           **Service
  )
{
  IP4_SERVICE               *IpSb;
  EFI_STATUS                Status;

  ASSERT (Service != NULL);

  *Service = NULL;

  //
  // allocate a service private data then initialize all the filed to
  // empty resources, so if any thing goes wrong when allocating
  // resources, Ip4CleanService can be called to clean it up.
  //
  IpSb = AllocateZeroPool (sizeof (IP4_SERVICE));

  if (IpSb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  IpSb->Signature                   = IP4_SERVICE_SIGNATURE;
  IpSb->ServiceBinding.CreateChild  = Ip4ServiceBindingCreateChild;
  IpSb->ServiceBinding.DestroyChild = Ip4ServiceBindingDestroyChild;
  IpSb->State                       = IP4_SERVICE_UNSTARTED;

  IpSb->NumChildren                 = 0;
  InitializeListHead (&IpSb->Children);

  InitializeListHead (&IpSb->Interfaces);
  IpSb->DefaultInterface            = NULL;
  IpSb->DefaultRouteTable           = NULL;

  Ip4InitAssembleTable (&IpSb->Assemble);

  IpSb->IgmpCtrl.Igmpv1QuerySeen    = 0;
  InitializeListHead (&IpSb->IgmpCtrl.Groups);

  IpSb->Image                       = ImageHandle;
  IpSb->Controller                  = Controller;

  IpSb->MnpChildHandle              = NULL;
  IpSb->Mnp                         = NULL;

  IpSb->MnpConfigData.ReceivedQueueTimeoutValue = 0;
  IpSb->MnpConfigData.TransmitQueueTimeoutValue = 0;
  IpSb->MnpConfigData.ProtocolTypeFilter        = IP4_ETHER_PROTO;
  IpSb->MnpConfigData.EnableUnicastReceive      = TRUE;
  IpSb->MnpConfigData.EnableMulticastReceive    = TRUE;
  IpSb->MnpConfigData.EnableBroadcastReceive    = TRUE;
  IpSb->MnpConfigData.EnablePromiscuousReceive  = FALSE;
  IpSb->MnpConfigData.FlushQueuesOnReset        = TRUE;
  IpSb->MnpConfigData.EnableReceiveTimestamps   = FALSE;
  IpSb->MnpConfigData.DisableBackgroundPolling  = FALSE;

  ZeroMem (&IpSb->SnpMode, sizeof (EFI_SIMPLE_NETWORK_MODE));

  IpSb->Timer = NULL;
  IpSb->ReconfigCheckTimer = NULL;

  IpSb->ReconfigEvent = NULL;

  IpSb->Reconfig = FALSE;

  IpSb->MediaPresent = TRUE;

  //
  // Create various resources. First create the route table, timer
  // event, ReconfigEvent and MNP child. IGMP, interface's initialization depend
  // on the MNP child.
  //
  IpSb->DefaultRouteTable = Ip4CreateRouteTable ();

  if (IpSb->DefaultRouteTable == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL | EVT_TIMER,
                  TPL_CALLBACK,
                  Ip4TimerTicking,
                  IpSb,
                  &IpSb->Timer
                  );

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL | EVT_TIMER,
                  TPL_CALLBACK,
                  Ip4TimerReconfigChecking,
                  IpSb,
                  &IpSb->ReconfigCheckTimer
                  );

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  Ip4AutoReconfigCallBack,
                  IpSb,
                  &IpSb->ReconfigEvent
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = NetLibCreateServiceChild (
             Controller,
             ImageHandle,
             &gEfiManagedNetworkServiceBindingProtocolGuid,
             &IpSb->MnpChildHandle
             );

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = gBS->OpenProtocol (
                  IpSb->MnpChildHandle,
                  &gEfiManagedNetworkProtocolGuid,
                  (VOID **) &IpSb->Mnp,
                  ImageHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = Ip4ServiceConfigMnp (IpSb, TRUE);

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = IpSb->Mnp->GetModeData (IpSb->Mnp, NULL, &IpSb->SnpMode);

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = Ip4InitIgmp (IpSb);

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  IpSb->MacString = NULL;
  Status = NetLibGetMacString (IpSb->Controller, IpSb->Image, &IpSb->MacString);

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  IpSb->DefaultInterface = Ip4CreateInterface (IpSb->Mnp, Controller, ImageHandle);

  if (IpSb->DefaultInterface == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  InsertHeadList (&IpSb->Interfaces, &IpSb->DefaultInterface->Link);

  ZeroMem (&IpSb->Ip4Config2Instance, sizeof (IP4_CONFIG2_INSTANCE));

  Status = Ip4Config2InitInstance (&IpSb->Ip4Config2Instance);

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  IpSb->MaxPacketSize = IpSb->SnpMode.MaxPacketSize - sizeof (IP4_HEAD);
  if (NetLibGetVlanId (IpSb->Controller) != 0) {
    //
    // This is a VLAN device, reduce MTU by VLAN tag length
    //
    IpSb->MaxPacketSize -= NET_VLAN_TAG_LEN;
  }
  IpSb->OldMaxPacketSize = IpSb->MaxPacketSize;
  *Service = IpSb;

  return EFI_SUCCESS;

ON_ERROR:
  Ip4CleanService (IpSb);
  FreePool (IpSb);

  return Status;
}


/**
  Clean up a IP4 service binding instance. It will release all
  the resource allocated by the instance. The instance may be
  partly initialized, or partly destroyed. If a resource is
  destroyed, it is marked as that in case the destroy failed and
  being called again later.

  @param[in]  IpSb               The IP4 service binding instance to clean up

  @retval EFI_SUCCESS            The resource used by the instance are cleaned up
  @retval other                  Failed to clean up some of the resources.

**/
EFI_STATUS
Ip4CleanService (
  IN IP4_SERVICE            *IpSb
  )
{
  EFI_STATUS                Status;

  IpSb->State     = IP4_SERVICE_DESTROY;

  if (IpSb->Timer != NULL) {
    gBS->SetTimer (IpSb->Timer, TimerCancel, 0);
    gBS->CloseEvent (IpSb->Timer);

    IpSb->Timer = NULL;
  }

  if (IpSb->ReconfigCheckTimer != NULL) {
    gBS->SetTimer (IpSb->ReconfigCheckTimer, TimerCancel, 0);
    gBS->CloseEvent (IpSb->ReconfigCheckTimer);

    IpSb->ReconfigCheckTimer = NULL;
  }

  if (IpSb->DefaultInterface != NULL) {
    Status = Ip4FreeInterface (IpSb->DefaultInterface, NULL);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    IpSb->DefaultInterface = NULL;
  }

  if (IpSb->DefaultRouteTable != NULL) {
    Ip4FreeRouteTable (IpSb->DefaultRouteTable);
    IpSb->DefaultRouteTable = NULL;
  }

  Ip4CleanAssembleTable (&IpSb->Assemble);

  if (IpSb->MnpChildHandle != NULL) {
    if (IpSb->Mnp != NULL) {
      gBS->CloseProtocol (
             IpSb->MnpChildHandle,
             &gEfiManagedNetworkProtocolGuid,
             IpSb->Image,
             IpSb->Controller
             );

      IpSb->Mnp = NULL;
    }

    NetLibDestroyServiceChild (
      IpSb->Controller,
      IpSb->Image,
      &gEfiManagedNetworkServiceBindingProtocolGuid,
      IpSb->MnpChildHandle
      );

    IpSb->MnpChildHandle = NULL;
  }

  if (IpSb->ReconfigEvent != NULL) {
    gBS->CloseEvent (IpSb->ReconfigEvent);

    IpSb->ReconfigEvent = NULL;
  }

  IpSb->Reconfig = FALSE;

  if (IpSb->MacString != NULL) {
    FreePool (IpSb->MacString);
  }

  Ip4Config2CleanInstance (&IpSb->Ip4Config2Instance);

  return EFI_SUCCESS;
}

/**
  Callback function which provided by user to remove one node in NetDestroyLinkList process.

  @param[in]    Entry           The entry to be removed.
  @param[in]    Context         Pointer to the callback context corresponds to the Context in NetDestroyLinkList.

  @retval EFI_SUCCESS           The entry has been removed successfully.
  @retval Others                Fail to remove the entry.

**/
EFI_STATUS
EFIAPI
Ip4DestroyChildEntryInHandleBuffer (
  IN LIST_ENTRY         *Entry,
  IN VOID               *Context
  )
{
  IP4_PROTOCOL                  *IpInstance;
  EFI_SERVICE_BINDING_PROTOCOL  *ServiceBinding;
  UINTN                         NumberOfChildren;
  EFI_HANDLE                    *ChildHandleBuffer;

  if (Entry == NULL || Context == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  IpInstance = NET_LIST_USER_STRUCT_S (Entry, IP4_PROTOCOL, Link, IP4_PROTOCOL_SIGNATURE);
  ServiceBinding    = ((IP4_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT *) Context)->ServiceBinding;
  NumberOfChildren  = ((IP4_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT *) Context)->NumberOfChildren;
  ChildHandleBuffer = ((IP4_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT *) Context)->ChildHandleBuffer;

  if (!NetIsInHandleBuffer (IpInstance->Handle, NumberOfChildren, ChildHandleBuffer)) {
    return EFI_SUCCESS;
  }

  return ServiceBinding->DestroyChild (ServiceBinding, IpInstance->Handle);
}

/**
  Start this driver on ControllerHandle. This service is called by the
  EFI boot service ConnectController(). In order to make
  drivers as small as possible, there are a few calling restrictions for
  this service. ConnectController() must follow these
  calling restrictions. If any other agent wishes to call Start() it
  must also follow these calling restrictions.

  @param[in]  This                 Protocol instance pointer.
  @param[in]  ControllerHandle     Handle of device to bind driver to
  @param[in]  RemainingDevicePath  Optional parameter use to pick a specific child
                                   device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
Ip4DriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS                    Status;
  IP4_SERVICE                   *IpSb;
  EFI_IP4_CONFIG2_PROTOCOL      *Ip4Cfg2;
  UINTN                         Index;
  IP4_CONFIG2_DATA_ITEM         *DataItem;

  IpSb     = NULL;
  Ip4Cfg2  = NULL;
  DataItem = NULL;
  CDEMOFINE((MFNINF(1) "Hello world\n"));
  printf("Welcome, to BDS phase...\n");
  if (1) {

      UNIDUMPPARM hexparms = { 
          .reg = 0, 
          .bit.elmsizemin1 = 0,
          .bit.fBaseOfs = 0,
          .bit.fNoDash = 0,
          .bit.fNoAscii = 0,

      };

      UniDump(
          hexparms, 
          0x1000, 
          0x200000, 
          /*(unsigned long long(*)(unsigned long long))*/&GetMem32,
          WriteString
      );

    }

  //
  // Test for the Ip4 service binding protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiIp4ServiceBindingProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  CDEMOFINE((MFNINF(1) "Status == %016X, %s\n", Status,strefierror(Status)));

  if (Status == EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

  Status = Ip4CreateService (ControllerHandle, This->DriverBindingHandle, &IpSb);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (IpSb != NULL);

  Ip4Cfg2  = &IpSb->Ip4Config2Instance.Ip4Config2;

  //
  // Install the Ip4ServiceBinding Protocol onto ControlerHandle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ControllerHandle,
                  &gEfiIp4ServiceBindingProtocolGuid,
                  &IpSb->ServiceBinding,
                  &gEfiIp4Config2ProtocolGuid,
                  Ip4Cfg2,
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    goto FREE_SERVICE;
  }

  //
  // Read the config data from NV variable again.
  // The default data can be changed by other drivers.
  //
  Status = Ip4Config2ReadConfigData (IpSb->MacString, &IpSb->Ip4Config2Instance);
  if (EFI_ERROR (Status)) {
    goto UNINSTALL_PROTOCOL;
  }

  //
  // Consume the installed EFI_IP4_CONFIG2_PROTOCOL to set the default data items.
  //
  for (Index = Ip4Config2DataTypePolicy; Index < Ip4Config2DataTypeMaximum; Index++) {
    DataItem = &IpSb->Ip4Config2Instance.DataItem[Index];
    if (DataItem->Data.Ptr != NULL) {
      Status = Ip4Cfg2->SetData (
                          Ip4Cfg2,
                          Index,
                          DataItem->DataSize,
                          DataItem->Data.Ptr
                          );
      if (EFI_ERROR(Status)) {
        goto UNINSTALL_PROTOCOL;
      }

      if (Index == Ip4Config2DataTypePolicy && (*(DataItem->Data.Policy) == Ip4Config2PolicyDhcp)) {
        break;
      }
    }
  }

  //
  // Ready to go: start the receiving and timer.
  // Ip4Config2SetPolicy maybe call Ip4ReceiveFrame() to set the default interface's RecvRequest first after
  // Ip4Config2 instance is initialized. So, EFI_ALREADY_STARTED is the allowed return status.
  //
  Status = Ip4ReceiveFrame (IpSb->DefaultInterface, NULL, Ip4AccpetFrame, IpSb);

  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    goto UNINSTALL_PROTOCOL;
  }

  Status = gBS->SetTimer (IpSb->Timer, TimerPeriodic, TICKS_PER_SECOND);

  if (EFI_ERROR (Status)) {
    goto UNINSTALL_PROTOCOL;
  }

  Status = gBS->SetTimer (IpSb->ReconfigCheckTimer, TimerPeriodic, 500 * TICKS_PER_MS);

  if (EFI_ERROR (Status)) {
    goto UNINSTALL_PROTOCOL;
  }

  //
  // Initialize the IP4 ID
  //
  mIp4Id = (UINT16)NET_RANDOM (NetRandomInitSeed ());

  return Status;

UNINSTALL_PROTOCOL:
  gBS->UninstallMultipleProtocolInterfaces (
         ControllerHandle,
         &gEfiIp4ServiceBindingProtocolGuid,
         &IpSb->ServiceBinding,
         &gEfiIp4Config2ProtocolGuid,
         Ip4Cfg2,
         NULL
         );

FREE_SERVICE:
  Ip4CleanService (IpSb);
  FreePool (IpSb);
  return Status;
}


/**
  Stop this driver on ControllerHandle. This service is called by the
  EFI boot service DisconnectController(). In order to
  make drivers as small as possible, there are a few calling
  restrictions for this service. DisconnectController()
  must follow these calling restrictions. If any other agent wishes
  to call Stop() it must also follow these calling restrictions.

  @param[in]  This              Protocol instance pointer.
  @param[in]  ControllerHandle  Handle of device to stop driver on
  @param[in]  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number
                                of children is zero stop the entire bus driver.
  @param[in]  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS           This driver is removed ControllerHandle
  @retval other                 This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
Ip4DriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  )
{
  EFI_SERVICE_BINDING_PROTOCOL             *ServiceBinding;
  IP4_SERVICE                              *IpSb;
  EFI_HANDLE                               NicHandle;
  EFI_STATUS                               Status;
  INTN                                     State;
  LIST_ENTRY                               *List;
  IP4_DESTROY_CHILD_IN_HANDLE_BUF_CONTEXT  Context;
  IP4_INTERFACE                            *IpIf;
  IP4_ROUTE_TABLE                          *RouteTable;

  BOOLEAN                                  IsDhcp4;

  IsDhcp4   = FALSE;

  NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiManagedNetworkProtocolGuid);
  if (NicHandle == NULL) {
    NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiArpProtocolGuid);
    if (NicHandle == NULL) {
      NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiDhcp4ProtocolGuid);
      if (NicHandle != NULL) {
        IsDhcp4 = TRUE;
      } else {
        return EFI_SUCCESS;
      }
    }
  }

  Status = gBS->OpenProtocol (
                  NicHandle,
                  &gEfiIp4ServiceBindingProtocolGuid,
                  (VOID **) &ServiceBinding,
                  This->DriverBindingHandle,
                  NicHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  IpSb = IP4_SERVICE_FROM_PROTOCOL (ServiceBinding);

  if (IsDhcp4) {
    Status = Ip4Config2DestroyDhcp4 (&IpSb->Ip4Config2Instance);
    gBS->CloseEvent (IpSb->Ip4Config2Instance.Dhcp4Event);
    IpSb->Ip4Config2Instance.Dhcp4Event = NULL;
  } else if (NumberOfChildren != 0) {
    List = &IpSb->Children;
    Context.ServiceBinding    = ServiceBinding;
    Context.NumberOfChildren  = NumberOfChildren;
    Context.ChildHandleBuffer = ChildHandleBuffer;
    Status = NetDestroyLinkList (
               List,
               Ip4DestroyChildEntryInHandleBuffer,
               &Context,
               NULL
               );
  } else if (IpSb->DefaultInterface->ArpHandle == ControllerHandle) {

    //
    // The ARP protocol for the default interface is being uninstalled and all
    // its IP child handles should have been destroyed before. So, release the
    // default interface and route table, create a new one and mark it as not started.
    //
    Ip4CancelReceive (IpSb->DefaultInterface);
    Ip4FreeInterface (IpSb->DefaultInterface, NULL);
    Ip4FreeRouteTable (IpSb->DefaultRouteTable);

    IpIf = Ip4CreateInterface (IpSb->Mnp, IpSb->Controller, IpSb->Image);
    if (IpIf == NULL) {
      goto ON_ERROR;
    }
    RouteTable = Ip4CreateRouteTable ();
    if (RouteTable == NULL) {
      Ip4FreeInterface (IpIf, NULL);
      goto ON_ERROR;;
    }

    IpSb->DefaultInterface  = IpIf;
    InsertHeadList (&IpSb->Interfaces, &IpIf->Link);
    IpSb->DefaultRouteTable = RouteTable;
    Ip4ReceiveFrame (IpIf, NULL, Ip4AccpetFrame, IpSb);

    IpSb->State = IP4_SERVICE_UNSTARTED;

  } else if (IsListEmpty (&IpSb->Children)) {
    State           = IpSb->State;
    //
    // OK, clean other resources then uninstall the service binding protocol.
    //
    Status = Ip4CleanService (IpSb);
    if (EFI_ERROR (Status)) {
      IpSb->State = State;
      goto ON_ERROR;
    }

    gBS->UninstallMultipleProtocolInterfaces (
           NicHandle,
           &gEfiIp4ServiceBindingProtocolGuid,
           ServiceBinding,
           &gEfiIp4Config2ProtocolGuid,
           &IpSb->Ip4Config2Instance.Ip4Config2,
           NULL
           );

    if (gIp4ControllerNameTable != NULL) {
      FreeUnicodeStringTable (gIp4ControllerNameTable);
      gIp4ControllerNameTable = NULL;
    }
    FreePool (IpSb);
  }

ON_ERROR:
  return Status;
}


/**
  Creates a child handle and installs a protocol.

  The CreateChild() function installs a protocol on ChildHandle.
  If ChildHandle is a pointer to NULL, then a new handle is created and returned in ChildHandle.
  If ChildHandle is not a pointer to NULL, then the protocol installs on the existing ChildHandle.

  @param  This        Pointer to the EFI_SERVICE_BINDING_PROTOCOL instance.
  @param  ChildHandle Pointer to the handle of the child to create. If it is NULL,
                      then a new handle is created. If it is a pointer to an existing UEFI handle,
                      then the protocol is added to the existing UEFI handle.

  @retval EFI_SUCCES            The protocol was added to ChildHandle.
  @retval EFI_INVALID_PARAMETER ChildHandle is NULL.
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources available to create
                                the child
  @retval other                 The child handle was not created

**/
EFI_STATUS
EFIAPI
Ip4ServiceBindingCreateChild (
  IN EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                *ChildHandle
  )
{
  IP4_SERVICE               *IpSb;
  IP4_PROTOCOL              *IpInstance;
  EFI_TPL                   OldTpl;
  EFI_STATUS                Status;
  VOID                      *Mnp;

  if ((This == NULL) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  IpSb       = IP4_SERVICE_FROM_PROTOCOL (This);
  IpInstance = AllocatePool (sizeof (IP4_PROTOCOL));

  if (IpInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ip4InitProtocol (IpSb, IpInstance);

  //
  // Install Ip4 onto ChildHandle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  ChildHandle,
                  &gEfiIp4ProtocolGuid,
                  &IpInstance->Ip4Proto,
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  IpInstance->Handle = *ChildHandle;

  //
  // Open the Managed Network protocol BY_CHILD.
  //
  Status = gBS->OpenProtocol (
                  IpSb->MnpChildHandle,
                  &gEfiManagedNetworkProtocolGuid,
                  (VOID **) &Mnp,
                  gIp4DriverBinding.DriverBindingHandle,
                  IpInstance->Handle,
                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                  );
  if (EFI_ERROR (Status)) {
    gBS->UninstallMultipleProtocolInterfaces (
           *ChildHandle,
           &gEfiIp4ProtocolGuid,
           &IpInstance->Ip4Proto,
           NULL
           );

    goto ON_ERROR;
  }

  //
  // Insert it into the service binding instance.
  //
  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  InsertTailList (&IpSb->Children, &IpInstance->Link);
  IpSb->NumChildren++;

  gBS->RestoreTPL (OldTpl);

ON_ERROR:

  if (EFI_ERROR (Status)) {

    Ip4CleanProtocol (IpInstance);

    FreePool (IpInstance);
  }

  return Status;
}


/**
  Destroys a child handle with a protocol installed on it.

  The DestroyChild() function does the opposite of CreateChild(). It removes a protocol
  that was installed by CreateChild() from ChildHandle. If the removed protocol is the
  last protocol on ChildHandle, then ChildHandle is destroyed.

  @param  This        Pointer to the EFI_SERVICE_BINDING_PROTOCOL instance.
  @param  ChildHandle Handle of the child to destroy

  @retval EFI_SUCCES            The protocol was removed from ChildHandle.
  @retval EFI_UNSUPPORTED       ChildHandle does not support the protocol that is being removed.
  @retval EFI_INVALID_PARAMETER Child handle is NULL.
  @retval EFI_ACCESS_DENIED     The protocol could not be removed from the ChildHandle
                                because its services are being used.
  @retval other                 The child handle was not destroyed

**/
EFI_STATUS
EFIAPI
Ip4ServiceBindingDestroyChild (
  IN EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                    ChildHandle
  )
{
  EFI_STATUS                Status;
  IP4_SERVICE               *IpSb;
  IP4_PROTOCOL              *IpInstance;
  EFI_IP4_PROTOCOL          *Ip4;
  EFI_TPL                   OldTpl;

  if ((This == NULL) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Retrieve the private context data structures
  //
  IpSb   = IP4_SERVICE_FROM_PROTOCOL (This);

  Status = gBS->OpenProtocol (
                  ChildHandle,
                  &gEfiIp4ProtocolGuid,
                  (VOID **) &Ip4,
                  gIp4DriverBinding.DriverBindingHandle,
                  ChildHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  IpInstance = IP4_INSTANCE_FROM_PROTOCOL (Ip4);

  if (IpInstance->Service != IpSb) {
    return EFI_INVALID_PARAMETER;
  }

  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  //
  // A child can be destroyed more than once. For example,
  // Ip4DriverBindingStop will destroy all of its children.
  // when UDP driver is being stopped, it will destroy all
  // the IP child it opens.
  //
  if (IpInstance->InDestroy) {
    gBS->RestoreTPL (OldTpl);
    return EFI_SUCCESS;
  }

  IpInstance->InDestroy = TRUE;

  //
  // Close the Managed Network protocol.
  //
  gBS->CloseProtocol (
         IpSb->MnpChildHandle,
         &gEfiManagedNetworkProtocolGuid,
         gIp4DriverBinding.DriverBindingHandle,
         ChildHandle
         );

  if (IpInstance->Interface != NULL && IpInstance->Interface->Arp != NULL) {
    gBS->CloseProtocol (
           IpInstance->Interface->ArpHandle,
           &gEfiArpProtocolGuid,
           gIp4DriverBinding.DriverBindingHandle,
           ChildHandle
           );
  }

  //
  // Uninstall the IP4 protocol first. Many thing happens during
  // this:
  // 1. The consumer of the IP4 protocol will be stopped if it
  // opens the protocol BY_DRIVER. For eaxmple, if MNP driver is
  // stopped, IP driver's stop function will be called, and uninstall
  // EFI_IP4_PROTOCOL will trigger the UDP's stop function. This
  // makes it possible to create the network stack bottom up, and
  // stop it top down.
  // 2. the upper layer will recycle the received packet. The recycle
  // event's TPL is higher than this function. The recycle events
  // will be called back before preceeding. If any packets not recycled,
  // that means there is a resource leak.
  //
  gBS->RestoreTPL (OldTpl);
  Status = gBS->UninstallProtocolInterface (
                  ChildHandle,
                  &gEfiIp4ProtocolGuid,
                  &IpInstance->Ip4Proto
                  );
  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);
  if (EFI_ERROR (Status)) {
    IpInstance->InDestroy = FALSE;
    goto ON_ERROR;
  }

  Status = Ip4CleanProtocol (IpInstance);
  if (EFI_ERROR (Status)) {
    gBS->InstallMultipleProtocolInterfaces (
           &ChildHandle,
           &gEfiIp4ProtocolGuid,
           Ip4,
           NULL
           );

    goto ON_ERROR;
  }

  RemoveEntryList (&IpInstance->Link);
  IpSb->NumChildren--;

  gBS->RestoreTPL (OldTpl);

  FreePool (IpInstance);
  return EFI_SUCCESS;

ON_ERROR:
  gBS->RestoreTPL (OldTpl);

  return Status;
}
