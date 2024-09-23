#ifndef _UBXGPS_H_
#define _UBXGPS_H_

//  Copyright (C) 2014-2017, SlashDevin
//
//  This file is part of NeoGPS
//
//  NeoGPS is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  NeoGPS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.

#include "NMEAGPS_cfg.h"
// Disable the entire file if derived types are not allowed.
#ifdef NMEAGPS_DERIVED_TYPES

#include "ublox/ubxNMEA.h"
#include "ublox/ubxmsg.h"
#include "GPSTime.h"
#include "ublox/ubx_cfg.h"

#if !defined(UBLOX_PARSE_STATUS)  & !defined(UBLOX_PARSE_TIMEGPS) & \
    !defined(UBLOX_PARSE_TIMEUTC) & !defined(UBLOX_PARSE_POSLLH)  & \
    !defined(UBLOX_PARSE_DOP)     & !defined(UBLOX_PARSE_PVT)     & \
    !defined(UBLOX_PARSE_VELNED)  & !defined(UBLOX_PARSE_SVINFO)  & \
    !defined(UBLOX_PARSE_HNR_PVT)

  #warning No UBX binary messages enabled: ubloxGPS class not defined.

#else

  #include <Stream.h>
  #include <stddef.h>

  // NOTE: millis() is used for ACK timing


class ubloxGPS : public UBLOXGPS_BASE
{
    ubloxGPS & operator =( const ubloxGPS & );
    ubloxGPS( const ubloxGPS & );
    ubloxGPS();

public:

    // Constructor needs to know the device to handle the UBX binary protocol
    ubloxGPS( Stream *device )
      :
        storage( (ublox::msg_t *) NULL ),
        reply( (ublox::msg_t *) NULL ),
        reply_expected( false ),
        ack_expected( false ),
        m_device( device )
      {};

    // ublox binary UBX message type.
    enum ubx_msg_t {
        UBX_MSG = UBLOXGPS_BASE_LAST_MSG+1
    };
    static const nmea_msg_t UBX_FIRST_MSG = (nmea_msg_t) UBX_MSG;
    static const nmea_msg_t UBX_LAST_MSG  = (nmea_msg_t) UBX_MSG;


    //................................................................
    // Process one character of ublox message.  The internal state 
    // machine tracks what part of the sentence has been received.  As the
    // tracks what part of the sentence has been received so far.  As the
    // sentence is received, members of the /fix/ structure are updated.  
    // @return DECODE_COMPLETED when a sentence has been completely received.

    decode_t decode( char c );

    //................................................................
    // Received message header.  Payload is only stored if /storage/ is 
    // overridden for that message type.  This is the UBX-specific
    // version of "nmeaMessage".

    ublox::msg_t & rx() { return m_rx_msg; }

    //................................................................

    bool enable_msg( ublox::msg_class_t msg_class, ublox::msg_id_t msg_id )
    {
      return send( ublox::cfg_msg_t( msg_class, msg_id, 1 ) );
    }

    bool disable_msg( ublox::msg_class_t msg_class, ublox::msg_id_t msg_id )
    {
      return send( ublox::cfg_msg_t( msg_class, msg_id, 0 ) );
    }
    
    //................................................................
    // Send a message (non-blocking).
    //    Although multiple /send_request/s can be issued,
    //    all replies will be handled identically.

    bool send_request( const ublox::msg_t & msg )
      {
        write( msg );
        return true;
      }

    bool send_request_P( const ublox::msg_t & msg )
      {
        write_P( msg );
        return true;
      }

    //................................................................
    // Send a message and wait for a reply (blocking).
    //    No event will be generated for the reply.
    //    If /msg/ is a UBX_CFG, this will wait for a UBX_CFG_ACK/NAK
    //      and return true if ACKed.
    //    If /msg/ is a poll, this will wait for the reply.
    //    If /msg/ is neither, this will return true immediately.
    //    If /msg/ is both, this will wait for both the reply and the ACK/NAK.
    //    If /storage_for/ is implemented, those messages will continue
    //      to be saved while waiting for this reply.

    bool send( const ublox::msg_t & msg, ublox::msg_t *reply_msg = (ublox::msg_t *) NULL );
    bool send_P( const ublox::msg_t & msg, ublox::msg_t *reply_msg = (ublox::msg_t *) NULL );
    using NMEAGPS::send_P;

    //................................................................
    //  Ask for a specific message (non-blocking).
    //     The message will receive be received later.
    //  See also /send_request/.

    bool poll_request( const ublox::msg_t & msg )
      {
        ublox::msg_t poll_msg( msg.msg_class, msg.msg_id, 0 );
        return send_request( poll_msg );
      }

    bool poll_request_P( const ublox::msg_t & msg )
      {
        ublox::msg_t poll_msg( (ublox::msg_class_t) pgm_read_byte( &msg.msg_class ),
                        (ublox::msg_id_t) pgm_read_byte( &msg.msg_id ), 0 );
        return send_request( poll_msg );
      }

    //................................................................
    //  Ask for a specific message (blocking).
    //    See also /send/.
    bool poll( ublox::msg_t & msg )
      {
        ublox::msg_t poll_msg( msg.msg_class, msg.msg_id, 0 );
        return send( poll_msg, &msg );
      }

    bool poll_P( const ublox::msg_t & msg, ublox::msg_t *reply_msg = (ublox::msg_t *) NULL )
      {
        ublox::msg_t poll_msg( (ublox::msg_class_t) pgm_read_byte( &msg.msg_class ),
                        (ublox::msg_id_t) pgm_read_byte( &msg.msg_id ), 0 );
        return send( poll_msg, reply_msg );
      }

    //................................................................
    //  Return the Stream that was passed into the constructor.

    Stream *Device() const { return (Stream *)m_device; };

protected:

    /*
     * Some UBX messages can be larger than 256 bytes, so
     * hide the 8-bit NMEAGPS::chrCount with this 16-bit version.
     */
    uint16_t chrCount;

    bool parseField( char chr );

    enum ubxState_t {
        UBX_IDLE  = NMEA_IDLE,
        UBX_SYNC2 = NMEA_LAST_STATE+1,
        UBX_HEAD,
        UBX_RECEIVING_DATA,
        UBX_CRC_A,
        UBX_CRC_B
    };
    static const ubxState_t UBX_FIRST_STATE = UBX_SYNC2;
    static const ubxState_t UBX_LAST_STATE  = UBX_CRC_B;

    inline void write
      ( uint8_t c, uint8_t & crc_a, uint8_t & crc_b ) const
    {
        m_device->print( (char) c );
        crc_a += c;
        crc_b += crc_a;
    };
    void write( const ublox::msg_t & msg );
    void write_P( const ublox::msg_t & msg );

    //................................................................
    // When the processing style is polling (not interrupt), this
    //   should be called frequently by any internal methods that block
    //   to make sure received chars continue to be processed.

    virtual void run()
      {
        if (processing_style == PS_POLLING)
          while (Device()->available())
            handle( Device()->read() );
        // else
        //   handled by interrupts
      }

    void wait_for_idle();
    bool wait_for_ack();
      //  NOTE: /run/ is called from these blocking functions 


    bool waiting() const
      {
        return (ack_expected && (!ack_received && !nak_received)) ||
               (reply_expected && !reply_received);
      }

    bool receiving() const
      {
        return (rxState != (rxState_t)UBX_IDLE) || (m_device && m_device->available());
      }

    // Override this if the contents of a particular message need to be saved.
    // This may execute in an interrupt context, so be quick!
    //  NOTE: the ublox::msg_t.length will get stepped on, so you may need to
    //  set it every time if you are using a union for your storage.

    virtual ublox::msg_t *storage_for( const ublox::msg_t & rx_msg )
      { return (ublox::msg_t *) NULL; }

    virtual bool intervalCompleted() const
      {
        return ((nmeaMessage     0\rßm˚¸   ∫  √f    _keyhttps://www.gstatic.com/_/mss/boq-search/_/js/k=boq-search.VisualFrontendUi.en.Wzyd1KwjEcU.es5.O/ck=boq-search.VisualFrontendUi.zkB1VsaoynY.L.B1.O/am=AB0AwP__8BgAAP-O_48QBAQAAAB0AAGCCADYCA6AUAEAGHgCUH-XAhIgAjhIAAFgJLoyAADYBcrGAAAQGAZgAAAAAAAU3nnAQAAAAAAAAAAAAMKKAQAAAAAAAIAAoEkAAAAAAAQ/d=1/exm=A7fCU,AdPnOb,B2u0ld,BVgquf,BXWsfc,COQbmf,ClUoee,DTy7fe,EEDORb,EFQ78c,GihOkd,HU2IR,I46Hvd,IZT63,JNoxi,KG2eXe,KUM7Z,Kg1rBc,Ko78Df,L1AAkb,LEikZe,Lurqoe,MI6k7c,Mlhmy,MpJwZc,NwH0H,O1Gjze,O6y8ed,OTA3Ae,ObWLec,OmgaI,PrPYRd,QIhFr,RMhBfe,Rr5NOe,S1avQ,SRsBqc,SdcwHb,SpsfSb,U0aPgd,UUJqVe,Uas9Hd,Ulmmrd,V3dDOb,VwDzFe,WO9ee,Wf0Cmd,Wq6lxf,XVMNvd,ZfAoz,ZwDk9d,_b,_r,_tp,aIe7ef,aQQdBd,aW3pY,aurFic,btdpvd,byfTOb,fKUV3e,fM7wyf,fkGYQb,gJjRYc,gychg,hKSk3e,hc6Ubd,kWgXee,kdOQLe,kjKdXe,lazG7b,lsjVmc,lwddkf,mI3LFb,mdR7q,n73qwf,nQze3d,nsJP5e,oR20R,oSegn,ovKuLd,pjICDe,pw70Gc,s39S4,sVEevc,sbC4bb,vSdyTd,w9hDv,ws9Tlc,xQtZb,xUdipf,xhIfAc,yDVVkb,zbML3c,zr1jrb/excm=_b,_r,_tp,unifiedviewerview/ed=1/wt=2/rs=AH7-fg7ZxoO9B3oJcoH50mi-3tmoaLWtcw/ee=cEt90b:ws9Tlc;QGR0gd:Mlhmy;uY49fb:COQbmf;yxTchf:KUM7Z;qddgKe:xQtZb;dIoSBb:SpsfSb;EmZ2Bf:zr1jrb;EVNhjf:pw70Gc;NSEoX:lazG7b;qaS3gd:yiLg6e;zOsCQe:Ko78Df;sTsDMc:kHVSUb;KcokUb:KiuZBf;WCEKNd:I46Hvd;xMUn6e:e0kzxe;fWLTFc:TVBJbf;flqRgb:ox2Q7c;pXdRYb:oR20R;JsbNhc:Xd8iUd;oGtAuc:sOXFj;rQSrae:C6D5Fc;kCQyJ:ueyPK;EABSZ:MXZt9d;qavrXe:zQzcXe;TxfV6d:YORN0b;dtl0hd:lLQWFe;UDrY1c:eps46d;wQlYve:aLUfP;GleZL:J1A7Od;g8nkx:U4MzKc;JXS8fb:Qj0suc;w3bZCb:ZPGaIb;VGRfx:VFqbr;aAJE9c:WHW6Ef;imqimf:jKGL2e;BgS6mb:fidj5d;UVmjEd:EesRsb;z97YGf:oug9te;CxXAWb:YyRLvc;VN6jIc:ddQyuf;F9mqte:UoRcbe;SLtqO:Kh1xYe;tosKvd:ZCqP3;WDGyFe:jcVOxd;VxQ32b:k0XsBb;DULqB:RKfG5c;Np8Qkd:Dpx6qc;bcPXSc:gSZLJb;aZ61od:arTwJ;cFTWae:gT8qnd;gaub4:TN6bMe;DpcR3d:zL72xf;hjRo6e:F62sG;BjwMce:cXX2Wb;Pjplud:EEDORb;io8t5d:yDVVkb;Oj465e:KG2eXe;ul9GGd:VDovNc;sP4Vbe:VwDzFe;kMFpHd:OTA3Ae;NPKaK:SdcwHb;nAFL3:s39S4;iFQyKf:QIhFr;zxnPse:SP0dJe;tGdRVe:oRqHk;yWysfe:IGrxNe;F774Sb:sVEevc;eBAeSb:sVEevc;VoYp5d:BXWsfc;xqxLcb:Whqy4b;Ti4hX:Y1W8Ad;vGrMZ:Y1W8Ad;h3MYod:wV5Pjc;zaIgPb:ovuoid;kbAm9d:MkHyGd;okUaUd:Kg1rBc;tH4IIe:NoECLb;wV5Pjc:nQze3d;eHDfl:ofjVkb;SNUn3:ZwDk9d;LBgRLc:SdcwHb;wR5FRb:O1Gjze;vfVwPd:TWOpEe;w9w86d:aIe7ef;KQzWid:mB4wNe;pNsl2d:j9Yuyc;Nyt6ic:jn2sGd/m=iaRXBb 
https://google.com/ÿAóEo˙Ù               \ØÅeQ/        äò#c˜ä–    ﬂŸiîùP©±∆èJ}|Œßª:lö”Ï˙√{e˝©°ÿAóEo˙Ù   Ÿp≈                                                                                                                                                                                                                                                                                                                                                                                    