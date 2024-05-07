// IdFindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "IdFindDlg.h"
#include "winsock2.h"
#include <ws2tcpip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma pack(1)



struct ConfigRecord
{
      DWORD m_Len;         // length of record
      BYTE ip_Addr[4];      /*The device IP Address */
      BYTE ip_Mask[4];      /*The IP Address Mask */
      BYTE ip_GateWay[4];   /*The address of the P gateway */
      BYTE ip_TftpServer[4];/*The address of the TFTP server to load data from for debugging */
      DWORD baud_rate;    /*The initial system Baud rate */
      unsigned char wait_seconds; /*The number of seconds to wait before booting */
      unsigned char bBoot_To_Application; /*True if we boot to the application, not the monitor */
      unsigned char bException_Action; /*What should we do when we have an exception? */
      unsigned char m_FileName[80]; /*The file name of the TFTP file to load */
      unsigned char mac_address[6]; /*The Ethernet MAC address */
      unsigned char ser_port;
      unsigned char ip_DNS[4];
      unsigned char core_mac_address[6];/*The Base unit MAC address */
      unsigned char typeof_if;
      unsigned char direct_tx;
      unsigned char m_dwIP_used[4];
      unsigned long m_dwMASK_used;
      unsigned long m_dwGate_used;
      unsigned long m_dwDNS_used;
      unsigned char m_bUnused[3];
      unsigned char m_q_boot; /* True to boot without messages */
      unsigned short checksum;       /*A Checksum for this structure */
};


#define CONFIG_IF_ID_ETHERNET (0)
#define CONFIG_IF_ID_WIFI  (1)
#define CONFIG_IF_ID_ETHERNET2 (2)

LPCSTR IfNames[3] =
{
  "Ethernet", "WiFi", "Ethernet2"
};



BOOL CRConnectedDirectly( ConfigRecord &cr, unsigned long received_from )
{
   unsigned long dwip;
   unsigned long cdwip;
   if ( cr.direct_tx != 0xAA )
   {
      return TRUE;
   }

   dwip = cr.m_dwIP_used[0];
   dwip = ( dwip << 8 ) | cr.m_dwIP_used[1];
   dwip = ( dwip << 8 ) | cr.m_dwIP_used[2];
   dwip = ( dwip << 8 ) | cr.m_dwIP_used[3];
   cdwip = dwip;
   dwip = htonl( dwip );
   if ( dwip == received_from )
   {
      return TRUE;
   }

   dwip = cr.ip_Addr[0];
   dwip = ( dwip << 8 ) | cr.ip_Addr[1];
   dwip = ( dwip << 8 ) | cr.ip_Addr[2];
   dwip = ( dwip << 8 ) | cr.ip_Addr[3];
   dwip = htonl( dwip );
   if ( dwip == received_from )
   {
      return TRUE;
   }


   return FALSE;
}


class NetBurnerDeviceTree
{
   public:
      NetBurnerDeviceTree *pNext;
      NetBurnerDeviceTree *pChild;
      NetBurnerDeviceTree *pParent;
      unsigned long received_from;
      unsigned char Root_MAC[6];
      CString Name;
      CString Application;
      CString DisplayTag;
      ConfigRecord cr;
      HTREEITEM htreel;

      NetBurnerDeviceTree( unsigned long rx_from, ConfigRecord *pcr, PCSTR name, PCSTR app );

      ~NetBurnerDeviceTree()
      {
         NetBurnerDeviceTree *pt = pChild;
         while ( pt != NULL )
         {
            NetBurnerDeviceTree *pd = pt;
            pt = pt->pNext;
            delete pd;
         }
      }


      void ReplaceContent( unsigned long rx_from, ConfigRecord *pcr, PCSTR name, PCSTR app );

      /* Was this received directly from the device? */

      BOOL ConnectedDirectly()
      {
         return CRConnectedDirectly( cr, received_from );
      }


      /* Is the DHCP both active and valid */
      BOOL DHCPValid()
      {
         if ( ConnectedDirectly() )
         {
            if ( received_from > 1 )
            {
               return TRUE;
            }
            return FALSE;
         }

         if ( ( cr.m_dwIP_used[0] ) ||
              ( cr.m_dwIP_used[1] ) ||
              ( cr.m_dwIP_used[2] ) ||
              ( cr.m_dwIP_used[3] ) )
         {
            return TRUE;
         }
         return FALSE;
      }


      /* Is the system trying to use DHCP? */
      BOOL UsingDHCP()
      {
         if ( ( cr.ip_Addr[0] ) ||
              ( cr.ip_Addr[1] ) ||
              ( cr.ip_Addr[2] ) ||
              ( cr.ip_Addr[3] ) )
         {
            return FALSE;
         }
         return TRUE;
      }



      /* What is the proper destination address for this interface */
      unsigned long GetDestinationAddress()
      {
         if ( ConnectedDirectly() )
         {
            return received_from;
         }

         PBYTE pb = NULL;

         if ( ( cr.ip_Addr[0] ) ||
              ( cr.ip_Addr[1] ) ||
              ( cr.ip_Addr[2] ) ||
              ( cr.ip_Addr[3] ) )
         {
            pb = cr.ip_Addr;
         }

         if ( DHCPValid() )
         {
            pb = cr.m_dwIP_used;
         }

         if ( pb == NULL )
         {
            return 0;
         }

         unsigned long dwip;
         dwip = pb[0];
         dwip = ( dwip << 8 ) | pb[1];
         dwip = ( dwip << 8 ) | pb[2];
         dwip = ( dwip << 8 ) | pb[3];

         return htonl( dwip );
      }

      CString GetLabelText()
      {
         CString sr;
         volatile DWORD dw = GetDestinationAddress();
         PBYTE pb = ( PBYTE ) & dw;
         if ( pChild )
         {
            sr.Format( "%s running %s", Name, Application );
         }
         else if ( pParent )
         {
            if ( UsingDHCP() )
            {
               sr.Format( "%s DHCP'd at %d.%d.%d.%d",
                          IfNames[cr.typeof_if % 3],
                          pb[0],
                          pb[1],
                          pb[2],
                          pb[3] );
            }
            else
            {
               sr.Format( "%s at %d.%d.%d.%d",
                          IfNames[cr.typeof_if % 3],
                          pb[0],
                          pb[1],
                          pb[2],
                          pb[3] );
            }
         }
         else if ( UsingDHCP() )
         {
            sr.Format( "%s DHCP'd at %d.%d.%d.%d running %s",
                       Name,
                       pb[0],
                       pb[1],
                       pb[2],
                       pb[3],
                       Application );
         }
         else
         {
            sr.Format( "%s  at %d.%d.%d.%d running %s",
                       Name,
                       pb[0],
                       pb[1],
                       pb[2],
                       pb[3],
                       Application );
         }
         return sr;
      }

      int Get_Icon_Index()
      {
         if ( pChild )
         {
            return 1;
         }

         int base = 2;

         if ( cr.typeof_if == CONFIG_IF_ID_WIFI )
         {
            base += 2;
         }
         if ( ConnectedDirectly() )
         {
            return base;
         }

         return base + 1;
      }
};


PBYTE GetCoreMacAddress( ConfigRecord *pcr )
{
   for ( int i = 0; i < 6; i++ )
   {
      if ( pcr->core_mac_address[i] != 0 )
      {
         return pcr->core_mac_address;
      }
   }

   return pcr->mac_address;
}


NetBurnerDeviceTree::NetBurnerDeviceTree( unsigned long rx_from,
                                          ConfigRecord *pcr,
                                          PCSTR name_in,
                                          PCSTR app_in )
{
   pNext = NULL;
   pChild = NULL;
   pParent = NULL;
   memcpy( &cr, pcr, sizeof( cr ) );
   Name = name_in;
   Application = app_in;
   received_from = rx_from;
   memcpy( Root_MAC, GetCoreMacAddress( pcr ), 6 );
}



void NetBurnerDeviceTree::ReplaceContent( unsigned long rx_from,
                                          ConfigRecord *pcr,
                                          PCSTR name_in,
                                          PCSTR app_in )
{
   memcpy( &cr, pcr, sizeof( cr ) );
   Name = name_in;
   Application = app_in;
   received_from = rx_from;
}






/* from system.h
unsigned long  recordsize;       //The stored size of the struct
unsigned long  ip_Addr;          //The device IP Address
unsigned long  ip_Mask;          //The IP Address Mask
unsigned long  ip_GateWay;       //The address of the P gateway
unsigned long  ip_TftpServer;    //The address of the TFTP server to load data from for debugging
unsigned long  baud_rate;        //The initial system Baud rate
unsigned char  wait_seconds;     //The number of seconds to wait before booting
unsigned char  bBoot_To_Application; //True if we boot to the application, not the monitor
unsigned char  bException_Action;   //What should we do when we have an exception?
unsigned char  m_FileName[80];      //The file name of the TFTP file to load
unsigned char  mac_address[6];      //The Ethernet MAC address
unsigned long  ip_DNS_server;
unsigned long  m_Unused[7];
unsigned short checksum;         //A Checksum for this structure
*/




struct ToFromPcStruct
{
      DWORD m_dwKeyValue;
      BYTE m_bAction;
      ConfigRecord m_cf;
};





class NetBurnFindSocket : public CAsyncSocket
{
      CIdFindDlg *m_pDlg;
      sockaddr_in m_BcastAddr;
      virtual void OnReceive( int nErrorCode );

   public:
      void SendRequest();
      void SendVerify( NetBurnerDeviceTree *pTree );
      NetBurnFindSocket( CIdFindDlg *ipdlg );
      NetBurnFindSocket( CIdFindDlg *ipdlg, LPCTSTR ifaceIP );
      ~NetBurnFindSocket();
};






#define VERIFY_FROM_PC_TO_NDK   (0x4255524E) //BURN
#define VERIFY_FROM_PC_TO_NDKV2 (0x42555232) //BUR2
#define VERIFY_FROM_NDK_TO_PC (0x4E455442) //NETB
#define UDP_NETBURNERID_PORT (0x4E42) /*NB*/


#define NBAUTO_READ     'R'
#define NBAUTO_WRITE 'W'
#define NBAUTO_VERIFY   'V'
#define NBAUTO_ERR      (3)
#define NBAUTO_OK    (4)


int CompMac( unsigned char *p1, unsigned char *p2 )
{
   for ( int i = 0; i < 6; i++ )
   {
      if ( p1[i] > p2[i] )
      {
         return 1;
      }
      if ( p1[i] < p2[i] )
      {
         return -1;
      }
   }
   return 0;
}




void CIdFindDlg::AddDeveiceTreeItem( LPCSTR name,
                                     LPCSTR app,
                                     DWORD fromaddr,
                                     ConfigRecord *pCR )
{
   if ( m_pDevices == NULL )
   {
      //* First device */
      m_pDevices = new NetBurnerDeviceTree( fromaddr, pCR, name, app );
      HTREEITEM htel = m_cTree.InsertItem( m_pDevices->GetLabelText() );
      m_cTree.SetItemState( htel,
                            INDEXTOSTATEIMAGEMASK( m_pDevices->Get_Icon_Index() ),
                            TVIS_STATEIMAGEMASK );
      m_cTree.SetItemData( htel, ( DWORD ) m_pDevices );
      m_pDevices->htreel = htel;
      m_cTree.SelectItem( htel );

      return;
   }
   else
   {
      /* Now to find out place in the world */
      PBYTE cmac = GetCoreMacAddress( pCR );
      NetBurnerDeviceTree *pp = m_pDevices;
      NetBurnerDeviceTree *lpp = NULL;

      while ( ( pp != NULL ) && ( CompMac( cmac, pp->Root_MAC ) < 0 ) )
      {
         lpp = pp;
         pp = pp->pNext;
      }

      if ( pp == NULL )
      {
         /* We need to add after the last */
         NetBurnerDeviceTree *pnew = new NetBurnerDeviceTree( fromaddr, pCR, name, app );
         pnew->pNext = NULL;
         lpp->pNext = pnew;
         HTREEITEM htel = m_cTree.InsertItem( pnew->GetLabelText() );
         m_cTree.SetItemState( htel,
                               INDEXTOSTATEIMAGEMASK( pnew->Get_Icon_Index() ),
                               TVIS_STATEIMAGEMASK );
         m_cTree.SetItemData( htel, ( DWORD ) pnew );
         pnew->htreel = htel;
         return;
      }

      if ( CompMac( cmac, pp->Root_MAC ) == 0 )
      {
         /* We need to do tree depth stuff */
         if ( pp->pChild )
         {
            NetBurnerDeviceTree *pscan = pp->pChild;
            do
            {
               if ( CompMac( pscan->cr.mac_address, pCR->mac_address ) == 0 )
               {
                  /* We have a duplicate ! */
                  if ( pscan->ConnectedDirectly() )
                  {
                     return;
                  }
                  if ( CRConnectedDirectly( *pCR, fromaddr ) )
                  {
                     pscan->ReplaceContent( fromaddr, pCR, name, app );
                     m_cTree.SetItemState( pscan->htreel,
                                           INDEXTOSTATEIMAGEMASK( pscan->Get_Icon_Index() ),
                                           TVIS_STATEIMAGEMASK );
                     m_cTree.SetItemText( pscan->htreel, pscan->GetLabelText() );
                     return;
                  }
               }
               pscan = pscan->pNext;
            }
            while ( pscan != NULL );
         }
         else
         {
            /* Possible duplicate */
            if ( CompMac( pp->cr.mac_address, pCR->mac_address ) == 0 )
            {
               return;
            }
         }


         NetBurnerDeviceTree *pnew = new NetBurnerDeviceTree( fromaddr, pCR, name, app );

         if ( pp->pChild )
         {
            pp->pChild->pNext = pnew;
         }
         else
         {
            /* We are the first child so we must change */
            pp->pChild = pnew;
            m_cTree.SetItemText( pp->htreel, pp->GetLabelText() );
            m_cTree.SetItemState( pp->htreel,
                                  INDEXTOSTATEIMAGEMASK( pp->Get_Icon_Index() ),
                                  TVIS_STATEIMAGEMASK );
            NetBurnerDeviceTree *pdup = new NetBurnerDeviceTree( pp->received_from,
                                                                 &pp->cr,
                                                                 pp->Name,
                                                                 pp->Application );
            pdup->pNext = pnew;
            pp->pChild = pdup;
            pdup->pParent = pp;
            HTREEITEM htel = m_cTree.InsertItem( pdup->GetLabelText(), pp->htreel );
            m_cTree.SetItemState( htel,
                                  INDEXTOSTATEIMAGEMASK( pdup->Get_Icon_Index() ),
                                  TVIS_STATEIMAGEMASK );
            m_cTree.SetItemData( htel, ( DWORD ) pdup );
            pdup->htreel = htel;
         }
         pnew->pParent = pp;
         HTREEITEM htel = m_cTree.InsertItem( pnew->GetLabelText(), pp->htreel );
         m_cTree.SetItemState( htel,
                               INDEXTOSTATEIMAGEMASK( pnew->Get_Icon_Index() ),
                               TVIS_STATEIMAGEMASK );
         m_cTree.SetItemData( htel, ( DWORD ) pnew );
         pnew->htreel = htel;

         return;
      }


      NetBurnerDeviceTree *pnew = new NetBurnerDeviceTree( fromaddr, pCR, name, app );
      pnew->pNext = pp;
      if ( lpp == NULL )
      {
         m_pDevices = pnew;
      }
      else
      {
         lpp->pNext = pnew;
      }
      HTREEITEM htel = m_cTree.InsertItem( pnew->GetLabelText() );
      m_cTree.SetItemState( htel,
                            INDEXTOSTATEIMAGEMASK( pnew->Get_Icon_Index() ),
                            TVIS_STATEIMAGEMASK );
      m_cTree.SetItemData( htel, ( DWORD ) pnew );
      pnew->htreel = htel;
      return;
   }
}


void CIdFindDlg::ProcessPacket( LPBYTE data, DWORD len, sockaddr_in &sa )
{
   char Device_Name[80];
   char Device_App[200];
   Device_Name[0] = 0;

   Device_App[0] = 0;
   if ( len > sizeof( ToFromPcStruct ) )
   {
      int clen = len - sizeof( ToFromPcStruct );
      if ( clen < 0 )
      {
         clen = 0;
      }
      if ( clen > 79 )
      {
         clen = 79;
      }
      strncpy( Device_Name, ( char * ) ( data + sizeof( ToFromPcStruct ) ), clen );
      unsigned char * cp = data + sizeof( ToFromPcStruct );
      while ( *cp )
      {
         cp++;
      }
      if ( ( data + len ) > cp + 2 )
      {
         cp++;
         strncpy( Device_App + 1, ( char * ) cp, 199 );
         Device_App[0] = ':';
         if ( strcmp( ":Unknown", Device_App ) == 0 )
         {
            Device_App[0] = 0;
         };
      }
      else
      {
         Device_App[0] = 0;
      }
   }

   ToFromPcStruct *tfpc = ( ToFromPcStruct * ) data;

   if ( ( tfpc->m_dwKeyValue == htonl( VERIFY_FROM_NDK_TO_PC ) ) )
   {
      // Listbox text
      char buffer[256];
      DWORD dwip = 0;

      dwip = tfpc->m_cf.ip_Addr[0];
      dwip = ( dwip << 8 ) | tfpc->m_cf.ip_Addr[1];
      dwip = ( dwip << 8 ) | tfpc->m_cf.ip_Addr[2];
      dwip = ( dwip << 8 ) | tfpc->m_cf.ip_Addr[3];

      if ( sa.sin_addr.S_un.S_addr != htonl( dwip ) )
      {
         _snprintf( buffer, 256,
                  "MAC:[%02x-%02X-%02X-%02X-%02X-%02X] IP:%d.%d.%d.%d DHCP:%d.%d.%d.%d %s%s",
                  tfpc->m_cf.mac_address[0],
                  tfpc->m_cf.mac_address[1],
                  tfpc->m_cf.mac_address[2],
                  tfpc->m_cf.mac_address[3],
                  tfpc->m_cf.mac_address[4],
                  tfpc->m_cf.mac_address[5],
                  tfpc->m_cf.ip_Addr[0],
                  tfpc->m_cf.ip_Addr[1],
                  tfpc->m_cf.ip_Addr[2],
                  tfpc->m_cf.ip_Addr[3],
                  sa.sin_addr.S_un.S_un_b.s_b1,
                  sa.sin_addr.S_un.S_un_b.s_b2,
                  sa.sin_addr.S_un.S_un_b.s_b3,
                  sa.sin_addr.S_un.S_un_b.s_b4,
                  Device_Name,
                  Device_App );
      }
      else
      {
         _snprintf( buffer, 256,
                  "MAC: [%02x-%02X-%02X-%02X-%02X-%02X]    IP:%d.%d.%d.%d %s%s",
                  tfpc->m_cf.mac_address[0],
                  tfpc->m_cf.mac_address[1],
                  tfpc->m_cf.mac_address[2],
                  tfpc->m_cf.mac_address[3],
                  tfpc->m_cf.mac_address[4],
                  tfpc->m_cf.mac_address[5],
                  tfpc->m_cf.ip_Addr[0],
                  tfpc->m_cf.ip_Addr[1],
                  tfpc->m_cf.ip_Addr[2],
                  tfpc->m_cf.ip_Addr[3],
                  Device_Name,
                  Device_App );
      }

      // Allocate and store NetBurner data



      ConfigRecord *pCR = ( ConfigRecord * ) malloc( htonl( tfpc->m_cf.m_Len ) );

      memcpy( pCR, &( tfpc->m_cf ), htonl( tfpc->m_cf.m_Len ) );

      dwip = htonl( dwip );
      AddDeveiceTreeItem( Device_Name, Device_App, sa.sin_addr.S_un.S_addr, pCR );

      free( pCR );
   }
}







void NetBurnFindSocket::OnReceive( int nErrorCode )
{
   BYTE buffer[2048];
   sockaddr_in SockAddr;
   int SockAddrLen = sizeof( SockAddr );
   int rv = ReceiveFrom( ( void * ) buffer, 1024, ( SOCKADDR * ) &SockAddr, &SockAddrLen );
   if ( rv > 0 )
   {
      if ( SockAddr.sin_addr.S_un.S_addr == htonl( INADDR_LOOPBACK ) )
      {
         SockAddr.sin_addr.S_un.S_addr = ( ( unsigned long * ) ( buffer + ( rv - 4 ) ) )[0];
         m_pDlg->ProcessPacket( buffer, rv - 4, SockAddr );
      }
      else
      {
         m_pDlg->ProcessPacket( buffer, rv, SockAddr );
      }
   }
}





void NetBurnFindSocket::SendRequest()
{
   ToFromPcStruct tfpc;
   tfpc.m_dwKeyValue = htonl( VERIFY_FROM_PC_TO_NDK );
   tfpc.m_bAction = NBAUTO_READ;

   SendTo( ( void * ) &tfpc,
           sizeof( tfpc ),
           ( SOCKADDR * ) &m_BcastAddr,
           sizeof( m_BcastAddr ) );
}


void NetBurnFindSocket::SendVerify( NetBurnerDeviceTree *pTree )
{
   NetBurnerDeviceTree *ptc;
   NetBurnerDeviceTree *ptr = pTree;

   unsigned char buffer[1500];
   ToFromPcStruct *ptfpc;

   ptfpc = ( ToFromPcStruct * ) buffer;
   ptfpc->m_dwKeyValue = htonl( VERIFY_FROM_PC_TO_NDKV2 );
   ptfpc->m_bAction = 'V';
   unsigned char * p = buffer + 5;

   while ( ptr )
   {
      if ( ptr->pChild )
      {
         ptc = ptr->pChild;

         while ( ptc )
         {
            if ( ptc->ConnectedDirectly() )
            {
               *p++ = ptc->cr.mac_address[4];
            }
            else
            {
               *p++ = ptc->cr.mac_address[4] ^ 0x80;
            }

            *p++ = ptc->cr.mac_address[5];
            ptc = ptc->pNext;
         }
      }
      else
      {
         if ( ptr->ConnectedDirectly() )
         {
            *p++ = ptr->cr.mac_address[4];
         }
         else
         {
            *p++ = ptr->cr.mac_address[4] ^ 0x80;
         }
         *p++ = ptr->cr.mac_address[5];
      }
      ptr = ptr->pNext;
   }

   *p++ = 0;
   *p++ = 0;
   *p++ = 0;
   *p++ = 0;
   int len = ( p + 1 ) - buffer;

   SendTo( ( void * ) buffer, len, ( SOCKADDR * ) &m_BcastAddr, sizeof( m_BcastAddr ) );
}




NetBurnFindSocket::NetBurnFindSocket( CIdFindDlg *ipdlg )
{
   m_pDlg = ipdlg;
   int i = 0;

   memset( &m_BcastAddr, 0, sizeof( m_BcastAddr ) );
   m_BcastAddr.sin_family = AF_INET;
   m_BcastAddr.sin_port = htons( UDP_NETBURNERID_PORT );
   m_BcastAddr.sin_addr.S_un.S_addr = INADDR_BROADCAST;

   try
   {
      BOOL bVal = 1;
      if ( Create( UDP_NETBURNERID_PORT, SOCK_DGRAM, FD_READ ) == 0 )
      {
         if ( Create( UDP_NETBURNERID_PORT + 1, SOCK_DGRAM, FD_READ ) == 0 )
         {
            throw( i );
         }
      }

      if ( Bind( UDP_NETBURNERID_PORT ) != 0 )
      {
         AfxMessageBox( "Trying again" );
         if ( Bind( UDP_NETBURNERID_PORT + 1 ) != 0 )
         {
            throw( i );
         }
      }
      if ( SetSockOpt( SO_BROADCAST, &bVal, sizeof( BOOL ) ) == 0 )
      {
         throw( i );
      };
   }
   catch ( int iv )
   {
      DWORD err = GetLastError();
      CString fmt;
      fmt.Format( "Unable to Bind socket, is AutoUpdate already open?", err, iv );
      AfxMessageBox( fmt );
   }
}

NetBurnFindSocket::NetBurnFindSocket( CIdFindDlg *ipdlg, LPCTSTR ifaceIP)
{
   m_pDlg = ipdlg;
   int i = 0;



   memset( &m_BcastAddr, 0, sizeof( m_BcastAddr ) );
   m_BcastAddr.sin_family = AF_INET;
   m_BcastAddr.sin_port = htons( UDP_NETBURNERID_PORT );
   m_BcastAddr.sin_addr.S_un.S_addr = INADDR_BROADCAST;



   try
   {
      BOOL bVal = 1;
      if ( Create( UDP_NETBURNERID_PORT, SOCK_DGRAM, FD_READ, ifaceIP ) == 0 )
      {
         if ( Create( UDP_NETBURNERID_PORT + 1, SOCK_DGRAM, FD_READ ) == 0 )
         {
            throw( i );
         }
      }

      if ( Bind( UDP_NETBURNERID_PORT ) != 0 )
      {
         AfxMessageBox( "Trying again" );
         if ( Bind( UDP_NETBURNERID_PORT + 1 ) != 0 )
         {
            throw( i );
         }
      }
      if ( SetSockOpt( SO_BROADCAST, &bVal, sizeof( BOOL ) ) == 0 )
      {
         throw( i );
      };
   }
   catch ( int iv )
   {
      DWORD err = GetLastError();
      CString fmt;
      fmt.Format( "Unable to Bind socket, is AutoUpdate already open?", err, iv );
      AfxMessageBox( fmt );
   }
}

NetBurnFindSocket::~NetBurnFindSocket()
{
}



/////////////////////////////////////////////////////////////////////////////
// CIdFindDlg dialog


CIdFindDlg::CIdFindDlg( CWnd *pParent /*=NULL*/ ) : CDialog( CIdFindDlg::IDD, pParent )
{
   //{{AFX_DATA_INIT(CIdFindDlg)
           // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   m_pSocket = NULL;
    nNumInterfaces = 0;
    for (int i=0;i<nNumInterfaces;++i) {
        socks[i] = NULL;
    }
}


void CIdFindDlg::DoDataExchange( CDataExchange *pDX )
{
   CDialog::DoDataExchange( pDX );
   //{{AFX_DATA_MAP(CIdFindDlg)
   DDX_Control(pDX, IDC_TREE_DISPLAY, m_cTree);
   DDX_Control(pDX, IDC_FIND_AGAIN, m_cFindButtom);
   //}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CIdFindDlg, CDialog)
        //{{AFX_MSG_MAP(CIdFindDlg)
        ON_BN_CLICKED(IDC_FIND_AGAIN, OnFindAgain)
       ON_WM_TIMER()

        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdFindDlg message handlers





void CIdFindDlg::OnFindAgain()
{
   m_cTree.DeleteAllItems();
   NetBurnerDeviceTree *pt = m_pDevices;
   while ( pt != NULL )
   {
      NetBurnerDeviceTree *pd = pt;
      pt = pt->pNext;
      delete pd;
   }
   m_pDevices = NULL;
   if ( m_uTimerId )
   {
      KillTimer( m_uTimerId );
   }
   m_uTimerId = SetTimer( 1, 1500, NULL );
   m_nTimerCycle = 0;
   m_cFindButtom.EnableWindow( false );
   //m_pSocket->SendRequest();

    for (int i=0;i<nNumInterfaces;++i) {
        socks[i]->SendRequest();
    }
}



void CIdFindDlg::OnOK()
{
   UpdateData( true );
   HTREEITEM htel = m_cTree.GetSelectedItem();

   if ( htel == NULL )
   {
      AfxMessageBox( "You must first select a device in the\nSelect a Unit in the tree." );
   }
   else
   {
      NetBurnerDeviceTree *pData = ( NetBurnerDeviceTree * ) m_cTree.GetItemData( htel );
      m_dwIPAddr = htonl( pData->GetDestinationAddress() );
      CDialog::OnOK();
   }
}

BOOL CIdFindDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   //m_pSocket = new NetBurnFindSocket( this );
   m_pDevices = NULL;
   m_iList.Create( IDB_ICON_BITMAP, 16, 4, 16 );
   m_cTree.SetImageList( &m_iList, TVSIL_STATE );
   m_uTimerId = 0;
    // Getting all network interfaces.
    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0,0,0,0);


    WSAIoctl(sd,SIO_GET_INTERFACE_LIST,0,0,&ifacelist,sizeof(ifacelist), &nBytesReturned,0,0);
    nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);

    for (int i=0;i<nNumInterfaces;++i) {
        sockaddr_in *pAddress;
        pAddress = (sockaddr_in *) & (ifacelist[i].iiAddress);
        LPCTSTR ifaceIP = inet_ntoa(pAddress->sin_addr);
        socks[i] = new NetBurnFindSocket(this,ifaceIP);
        socks[i]->SendRequest();
   if ( m_uTimerId )
   {
      KillTimer( m_uTimerId );
   }
   m_uTimerId = SetTimer( 1, 1500, NULL );
   m_nTimerCycle = 0;
    }


   m_cFindButtom.EnableWindow( false );
   //m_pSocket->SendRequest();

    for (int i=0;i<nNumInterfaces;++i) {
        socks[i]->SendRequest();
    }


   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


CIdFindDlg::~CIdFindDlg()
{
   if ( m_pSocket )
   {
      delete m_pSocket;
   }

    for (int i=0;i<nNumInterfaces;++i) {
        if (socks[i])
            delete socks[i];
    }
   m_pSocket = NULL;
}






void CIdFindDlg::OnTimer( UINT nIDEvent )
{
   m_nTimerCycle++;

   //m_pSocket->SendVerify( m_pDevices );

    for (int i=0;i<nNumInterfaces;++i) {
        socks[i]->SendVerify( m_pDevices );
    }

   if ( m_nTimerCycle == 3 )
   {
      m_cFindButtom.EnableWindow( true );
      KillTimer( m_uTimerId );
      m_uTimerId = 0;
   }
   CDialog::OnTimer( nIDEvent );
}

