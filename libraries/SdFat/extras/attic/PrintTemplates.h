/**
 * Copyright (c) 2011-2020 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef PrintTemplates_h
#define PrintTemplates_h
/**
 * \file
 * \brief templates for printf
 */
#include <stdarg.h>
#include "FmtNumber.h"
/** test for digit */
#define isDigit(d) ('0' <= (d) && (d) <= '9')
/** control for supported floating formats */
#define PRINTF_USE_FLOAT 2
//-----------------------------------------------------------------------------
/** Formatted print.
 *
 * \param[in] file destination file or device.
 * \param[in] fmt format string.
 * \param[in] ap argument list.
 *
 * \return number of character printed for success else a negative value.
 */
template<typename F>
int vfprintf(F* file, const char *fmt, va_list ap) {
#if PRINTF_USE_FLOAT
  char buf[30];
  double f;
#else  // PRINTF_USE_FLOAT
   char buf[15];
#endif  // PRINTF_USE_FLOAT
  char prefix[3];
  unsigned base;
  int n;
  int nc = 0;
  int nf;
  int nz;
  int prec;
  int width;
  size_t np;
  size_t ns;
  size_t nw;
  long ln;
  char c;
  char plusSign;
  char* ptr;  // end of string
  char* str;  // start of string
  bool altForm;
  bool leftAdjust;
  bool isLong;
  bool zeroPad;

  while (true) {
    const char* bgn = fmt;
    while ((c = *fmt++) && c!= '%') {}
    nw = fmt - bgn - 1;
    if (nw) {
      nc += nw;
      if (nw != file->write(bgn, nw)) {
        goto fail;
      }
    }
    if (!c) break;
    altForm = false;
    leftAdjust = false;
    np = 0;
    nz = 0;
    zeroPad = false;
    plusSign = 0;
    c = *fmt++;

    while (true) {
      if (c == '-') {
        leftAdjust = true;
      } else if (c == '+') {
        plusSign = '+';
      } else if (c == ' ') {
        if (plusSign == 0) {
          plusSign = ' ';
        }
      } else if (c == '0') {
        zeroPad = true;
      } else if (c == '#') {
        altForm = true;
      } else {
        break;
      }
      c = *fmt++;
    }

    width = 0;
	  if (isDigit(c)) {
		  while(isDigit(c)) {
		    width = 10 * width + c - '0';
		    c = *fmt++;
		  }
	  } else if (c == '*') {
		  width = va_arg(ap, int);
		  c = *fmt++;
		  if (width < 0) {
		    leftAdjust = true;
		    width = -width;
		  }
	  }
    if (leftAdjust) {
      zeroPad = false;
    }

    prec = -1;
	  if (c == '.') {
      zeroPad = false;
		  prec = 0;
		  c = *fmt++;
		  if (isDigit(c)) {
		    while(isDigit(c)) {
			    prec = 10 * prec + c - '0';
			    c = *fmt++;
		    }
		  } else if (c == '*') {
		    prec = va_arg(ap, int);
		    c = *fmt++;
		  }
	  }

    isLong = false;
    if (c == 'l' || c =='L') {
      isLong = true;
      c = *fmt++;
    }

    if (!c) break;

    str = buf + sizeof(buf);
    ptr = str;
    switch(c) {
      case 'c':
        *--str = va_arg(ap, int);
        break;

      case 's':
        str = va_arg(ap, char *);
        if (!str) {
          str = (char*)"(null)";
        }
        ns = strlen(str);
        ptr = str + (prec >= 0 && (size_t)prec < ns ? prec : ns);
        break;
{"net":{"http_server_properties":{"broken_alternative_services":[{"anonymization":[],"broken_count":1,"host":"trace.mediago.io","port":443,"protocol_str":"quic"},{"anonymization":[],"broken_count":1,"host":"trace.popin.cc","port":443,"protocol_str":"quic"},{"anonymization":[],"broken_count":153,"host":"www.bing.com","port":443,"protocol_str":"quic"}],"servers":[{"anonymization":[],"server":"https://zem.outbrainimg.com","supports_spdy":true},{"anonymization":[],"server":"https://www.google.com","supports_spdy":true},{"anonymization":[],"server":"https://accounts.google.com","supports_spdy":true},{"anonymization":[],"server":"https://www.gstatic.com","supports_spdy":true},{"anonymization":[],"server":"https://ssl.gstatic.com","supports_spdy":true},{"anonymization":[],"server":"https://contacts.google.com","supports_spdy":true},{"anonymization":[],"server":"https://apis.google.com","supports_spdy":true},{"anonymization":[],"server":"https://fonts.gstatic.com","supports_spdy":true},{"anonymization":[],"network_stats":{"srtt":26592},"server":"https://lh4.googleusercontent.com","supports_spdy":true},{"anonymization":[],"network_stats":{"srtt":26592},"server":"https://lh6.googleusercontent.com","supports_spdy":true},{"anonymization":[],"network_stats":{"srtt":25763},"server":"https://lh3.googleusercontent.com","supports_spdy":true},{"anonymization":[],"server":"https://lh5.googleusercontent.com","supports_spdy":true},{"anonymization":[],"server":"https://play.google.com","supports_spdy":true},{"anonymization":[],"server":"https://docs.google.com","supports_spdy":true},{"anonymization":[],"server":"https://loki.delve.office.com","supports_spdy":true},{"anonymization":[],"server":"https://api.msn.com","supports_spdy":true},{"anonymization":[],"server":"https://arc.msn.com","supports_spdy":true},{"anonymization":[],"server":"https://sb.scorecardresearch.com","supports_spdy":true},{"anonymization":[],"server":"https://c.msn.com","supports_spdy":true},{"anonymization":[],"server":"https://srtb.msn.com","supports_spdy":true},{"anonymization":[],"server":"https://px.ads.linkedin.com","supports_spdy":true},{"anonymization":[],"server":"https://trc.taboola.com","supports_spdy":true},{"anonymization":[],"server":"https://ent-api.msn.com","supports_spdy":true},{"anonymization":[],"server":"https://hbx.media.net","supports_spdy":true},{"anonymization":[],"server":"https://cm.mgid.com","supports_spdy":true},{"anonymization":[],"server":"https://pr-bh.ybp.yahoo.com","supports_spdy":true},{"anonymization":[],"server":"https://creativecdn.com","supports_spdy":true},{"anonymization":[],"server":"https://eb2.3lift.com","supports_spdy":true},{"anonymization":[],"server":"https://code.yengo.com","supports_spdy":true},{"anonymization":[],"server":"https://visitor.omnitagjs.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879367098006","port":443,"protocol_str":"quic"}],"anonymization":[],"server":"https://trace.mediago.io","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879367230371","port":443,"protocol_str":"quic"}],"anonymization":[],"server":"https://trace.popin.cc","supports_spdy":true},{"anonymization":[],"server":"https://sync.inmobi.com","supports_spdy":true},{"anonymization":[],"server":"https://webshell.suite.office.com","supports_spdy":true},{"anonymization":[],"server":"https://southeastasia1-mediap.svc.ms","supports_spdy":true},{"anonymization":[],"server":"https://img-s-msn-com.akamaized.net","supports_spdy":true},{"anonymization":[],"server":"https://services.bingapis.com","supports_spdy":true},{"anonymization":[],"server":"https://assets.msn.com","supports_spdy":true},{"anonymization":[],"server":"https://www.gravatar.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879380617520","port":443,"protocol_str":"quic"}],"anonymization":[],"server":"https://www.googletagmanager.com","supports_spdy":true},{"anonymization":[],"server":"https://i.stack.imgur.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879380955225","port":443,"protocol_str":"quic"}],"anonymization":[],"server":"https://7a11fb69c29e1b5edd56e5c4a2c94625.safeframe.googlesyndication.com","supports_spdy":true},{"anonymization":[],"server":"https://stackoverflow.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879380159255","port":443,"protocol_str":"quic"}],"anonymization":[],"network_stats":{"srtt":26018},"server":"https://ajax.googleapis.com","supports_spdy":true},{"anonymization":[],"network_stats":{"srtt":22058},"server":"https://substrate.office.com","supports_spdy":true},{"anonymization":[],"server":"https://ntp.msn.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879384441718","port":443,"protocol_str":"quic"}],"anonymization":[],"network_stats":{"srtt":32247},"server":"https://tpc.googlesyndication.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879385027502","port":443,"protocol_str":"quic"}],"anonymization":[],"network_stats":{"srtt":22905},"server":"https://pagead2.googlesyndication.com","supports_spdy":true},{"anonymization":[],"server":"https://wcpstatic.microsoft.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319879420736121","port":443,"protocol_str":"quic"}],"anonymization":[],"network_stats":{"srtt":30362},"server":"https://www.google-analytics.com","supports_spdy":true},{"anonymization":[],"server":"https://msedgeextensions.sf.tlu.dl.delivery.mp.microsoft.com","supports_spdy":true},{"anonymization":[],"server":"https://microsoftedgewelcome.microsoft.com","supports_spdy":true},{"anonymization":[],"server":"https://js.monitor.azure.com","supports_spdy":true},{"anonymization":[],"server":"https://www.clarity.ms","supports_spdy":true},{"anonymization":[],"server":"https://c.bing.com","supports_spdy":true},{"anonymization":[],"server":"https://c.clarity.ms","supports_spdy":true},{"anonymization":[],"server":"https://edgefrecdn.azureedge.net","supports_spdy":true},{"anonymization":[],"server":"https://edgeassetservice.azureedge.net","supports_spdy":true},{"anonymization":[],"server":"https://h.clarity.ms","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13321410648951571","port":443,"protocol_str":"quic"}],"anonymization":[],"network_stats":{"srtt":24868},"server":"https://update.googleapis.com","supports_spdy":true},{"anonymization":[],"server":"https://www.bing.com","supports_spdy":true},{"anonymization":[],"server":"https://edge.microsoft.com","supports_spdy":true},{"anonymization":[],"server":"https://business.bing.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13321414250162804","port":443,"protocol_str":"quic"}],"anonymization":[],"network_stats":{"srtt":28451},"server":"https://dns.google","supports_spdy":true}],"supports_quic":{"address":"192.168.10.126","used_quic":true},"version":5},"network_qualities":{"CAASABiAgICA+P////8B":"4G","CAESABiAgICA+P////8B":"4G","CAISABiAgICA+P////8B":"4G","CAYSABiAgICA+P////8B":"Offline"}}}                                                                                                                                                        