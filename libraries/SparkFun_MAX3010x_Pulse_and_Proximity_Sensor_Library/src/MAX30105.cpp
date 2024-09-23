removeEventListener?window.removeEventListener("message",a,!1):window.detachEvent&&window.detachEvent("onmessage",a)}
var P=function(){function a(){b=!1}if(!window.postMessage)return!1;var b=!0;N(a);window.postMessage("","*");O(a);return b}(),M={};"async-defers"in A||z("async-defers",M);M=A["async-defers"];P&&("async-listener"in A&&O(A["async-listener"]),N(K),z("async-listener",K));var Q={};"ps-s"in A||z("ps-s",Q);Q=A["ps-s"];function R(a){var b=document.createElement("a");b.href=a;b.href=b.href;a={href:b.href,protocol:b.protocol,host:b.host,hostname:b.hostname,port:b.port,pathname:b.pathname,search:b.search,hash:b.hash,username:b.username,password:b.password};a.origin=a.protocol+"//"+a.host;a.pathname&&"/"==a.pathname[0]||(a.pathname="/"+a.pathname);return a}function S(a){a=R(a);return J(a.href,"#")[0]};/*

 SPF
 (c) 2012-2017 Google Inc.
 https://ajax.googleapis.com/ajax/libs/spf/2.4.0/LICENSE
*/
var T={},U={},ha={};"rsrc-s"in A||z("rsrc-s",T);T=A["rsrc-s"];"rsrc-n"in A||z("rsrc-n",U);U=A["rsrc-n"];"rsrc-u"in A||z("rsrc-u",ha);ha=A["rsrc-u"];var ia={};"js-d"in A||z("js-d",ia);ia=A["js-d"];var ja={};"js-u"in A||z("js-u",ja);ja=A["js-u"];function ka(a,b,c){if(b){b=[];var d=0;c&&(a+="\r\n");var e=a.indexOf("[\r\n",d);for(-1<e&&(d=e+3);-1<(e=a.indexOf(",\r\n",d));){var f=I(a.substring(d,e));d=e+3;f&&b.push(JSON.parse(f))}e=a.indexOf("]\r\n",d);-1<e&&(f=I(a.substring(d,e)),d=e+3,f&&b.push(JSON.parse(f)));f="";a.length>d&&(f=a.substring(d),c&&H(f,"\r\n")&&(f=f.substring(0,f.length-2)));b=V(b);return{m:b,g:f}}a=JSON.parse(a);b=V(y(a));return{m:b,g:""}}
function V(a){var b=y(a);w(b,function(c){if(c){c.head&&(c.head=W(c.head));if(c.body)for(var d in c.body)c.body[d]=W(c.body[d]);c.foot&&(c.foot=W(c.foot))}});return a}
function W(a){var b=new la;if(!a)return b;if("[object String]"!=Object.prototype.toString.call(a))return a.scripts&&w(a.scripts,function(c){b.scripts.push({url:c.url||"",text:c.text||"",name:c.name||"",async:c.async||!1})}),a.styles&&w(a.styles,function(c){b.styles.push({url:c.url||"",text:c.text||"",name:c.name||""})}),a.links&&w(a.links,function(c){"spf-preconnect"==c.rel&&b.links.push({url:c.url||"",rel:c.rel||""})}),b.html=a.html||"",b;a=a.replace(ma,function(c,d,e,f){if("script"==d){d=(d=e.match(X))?
d[1]:"";var h=e.match(na);h=h?h[1]:"";var k=oa.test(e);e=pa.exec(e);return(e=!e||-1!=e[1].indexOf("/javascript")||-1!=e[1].indexOf("/x-javascript")||-1!=e[1].indexOf("/ecmascript"))?(b.scripts.push({url:h,text:f,name:d,async:k}),""):c}return"style"==d&&(d=(d=e.match(X))?d[1]:"",e=pa.exec(e),e=!e||-1!=e[1].indexOf("text/css"))?(b.styles.push({url:"",text:f,name:d}),""):c});a=a.replace(qa,function(c,d){var e=d.match(ra);e=e?e[1]:"";return"stylesheet"==e?(e=(e=d.match(X))?e[1]:"",d=(d=d.match(sa))?d[1]:
"",b.styles.push({url:d,text:"",name:e}),""):"spf-preconnect"==e?(d=(d=d.match(sa))?d[1]:"",b.links.push({url:d,rel:e}),""):c});b.html=a;return b}function la(){this.html="";this.scripts=[];this.styles=[];this.links=[]}(function(){var a=document.createElement("div");return"transition"in a.style?!0:x(["webkit","Moz","Ms","O","Khtml"],function(b){return b+"Transition"in a.style})})();
var qa=/\x3clink([\s\S]*?)\x3e/ig,ma=/\x3c(script|style)([\s\S]*?)\x3e([\s\S]*?)\x3c\/\1\x3e/ig,oa=/(?:\s|^)async(?:\s|=|$)/i,sa=/(?:\s|^)href\s*=\s*["']?([^\s"']+)/i,X=/(?:\s|^)name\s*=\s*["']?([^\s"']+)/i,ra=/(?:\s|^)rel\s*=\s*["']?([^\s"']+)/i,na=/(?:\s|^)src\s*=\s*["']?([^\s"']+)/i,pa=/(?:\s|^)type\s*=\s*["']([^"']+)["']/i;function ta(a,b,c,d){var e=d||{},f=!1,h=0,k,g=new XMLHttpRequest;g.open(a,b,!0);g.timing={};var m=g.abort;g.abort=function(){clearTimeout(k);g.onreadystatechange=null;m.call(g)};g.onreadystatechange=function(){var q=g.timing;if(2==g.readyState){q.responseStart=q.responseStart||v();if("json"==g.responseType)f=!1;else if(C["assume-all-json-requests-chunked"]||-1<(g.getResponseHeader("Transfer-Encoding")||"").toLowerCase().indexOf("chunked"))f=!0;else{q=g.getResponseHeader("X-Firefox-Spdy");var B=window.chrome&&
chrome.loadTimes&&chrome.loadTimes();B=B&&B.wasFetchedViaSpdy;f=!(!q&&!B)}e.u&&e.u(g)}else 3==g.readyState?f&&e.l&&(q=g.responseText.substring(h),h=g.responseText.length,e.l(g,q)):4==g.readyState&&(q.responseEnd=q.responseEnd||v(),window.performance&&window.performance.getEntriesByName&&(g.resourceTiming=window.performance.getEntriesByName(b).pop()),f&&e.l&&g.responseText.length>h&&(q=g.responseText.substring(h),h=g.responseText.length,e.l(g,q)),clearTimeout(k),e.s&&e.s(g))};"responseType"in g&&"json"==
e.responseType&&(g.responseType="json");e.withCredentials&&(g.withCredentials=e.withCredentials);d="FormData"in window&&c instanceof FormData;a="POST"==a&&!d;if(e.headers)for(var t in e.headers)g.setRequestHeader(t,e.headers[t]),"content-type"==t.toLowerCase()&&(a=!1);a&&g.setRequestHeader("Content-Type","application/x-www-form-urlencoded");0<e.C&&(k=setTimeout(function(){g.abort();e.A&&e.A(g)},e.C));g.timing.fetchStart=v();g.send(c);return g};function ua(a,b,c,d,e){var f=!1;c.responseStart=c.responseEnd=v();b.type&&0==b.type.lastIndexOf("navigate",0)&&(c.navigationStart=c.startTime,C["cache-unified"]||(D(d),f=!0));b.j&&"multipart"==e.type&&w(e.parts,function(h){h.timing||(h.timing={});h.timing.spfCached=!!c.spfCached;h.timing.spfPrefetched=!!c.spfPrefetched;b.j(a,h)});va(a,b,c,e,f)}function wa(a,b,c){a=c.getResponseHeader("X-SPF-Response-Type")||"";b.o=-1!=a.toLowerCase().indexOf("multipart")}
function xa(a,b,c,d,e,f,h){if(d.o){f=d.g+f;try{var k=ka(f,!0,h)}catch(g){e.abort();b.i&&b.i(a,g,e);return}b.j&&w(k.m,function(g){g.timing||(g.timing={});g.timing.spfCached=!!c.spfCached;g.timing.spfPrefetched=!!c.spfPrefetched;b.j(a,g)});d.h=d.h.concat(k.m);d.g=k.g}}
function ya(a,b,c,d,e){if(e.timing)for(var f in e.timing)c[f]=e.timing[f];if(e.resourceTiming)if("load"==b.type)for(var h in e.resourceTiming)c[h]=e.resourceTiming[h];else if(window.performance&&window.performance.timing&&(f=window.performance.timing.navigationStart,f+e.resourceTiming.startTime>=c.startTime))for(var k in e.resourceTiming)h=e.resourceTiming[k],void 0!==h&&(H(k,"Start")||H(k,"End")||"startTime"==k)&&(c[k]=f+Math.round(h));"load"!=b.type&&(c.navigationStart=c.startTime);d.h.length&&
(d.g=I(d.g),d.g&&xa(a,b,c,d,e,"",!0));if("json"==e.responseType){if(!e.response){b.i&&b.i(a,Error("JSON response parsing failed"),e);return}var g=V(y(e.response))}else try{g=ka(e.responseText).m}catch(t){b.i&&b.i(a,t,e);return}if(b.j&&1<g.length)for(d=d.h.length;d<g.length;d++)e=g[d],e.timing||(e.timing={}),e.timing.spfCached=!!c.spfCached,e.timing.spfPrefetched=!!c.spfPrefetched,b.j(a,e);if(1<g.length){var m;w(g,function(t){t.cacheType&&(m=t.cacheType)});g={parts:g,type:"multipart"};m&&(g.cacheType=
m)}else g=1==g.length?g[0]:{};va(a,b,c,g,!0)}function va(a,b,c,d,e){if(e&&"POST"!=b.method&&(e=za(a,b.current,d.cacheType,b.type,!0))){d.cacheKey=e;var f={response:d,type:b.type||""},h=parseInt(C["cache-lifetime"],10),k=parseInt(C["cache-max"],10);0>=h||0>=k||(k=E(),f={data:f,life:h,time:v(),count:0},G(f),k[e]=f,setTimeout(fa,1E3))}d.timing=c;b.v&&b.v(a,d)}
function za(a,b,c,d,e){a=S(a);var f;C["cache-unified"]?f=a:"navigate-back"==d||"navigate-forward"==d?f="history "+a:"navigate"==d?f=(e?"history ":"prefetch ")+a:"prefetch"==d&&(f=e?"prefetch "+a:"");b&&"url"==c?f+=" previous "+b:b&&"path"==c&&(f+=" previous "+R(b).pathname);return f||""}
function Aa(a,b){var c=[];b&&(c.push(a+" previous "+b),c.push(a+" previous "+R(b).pathname));c.push(a);var d=null;x(c,function(e){a:{var f=E();if(e in f){f=f[e];if(F(f)){G(f);f=f.data;break a}D(e)}f=void 0}f&&(d={key:e,response:f.response,type:f.type});return!!f});return d}function Ba(){this.o=!1;this.g="";this.h=[]};function Y(a,b){if(a){var c=Array.prototype.slice.call(arguments);c[0]=a;c=ea.apply(null,c)}return!1!==c};function Ca(a,b,c,d){Y((a||{}).onError,{url:b,err:c,xhr:d})}function Da(a,b,c){Y((a||{}).onPartProcess,{url:b,part:c})&&Y((a||{}).onPartDone,{url:b,part:c})}function Ea(a,b,c){var d;(d="multipart"==c.type)||(d=Y((a||{}).onProcess,{url:b,response:c}));d&&Y((a||{}).onDone,{url:b,response:c})}
var Fa={request:function(a,b){b=b||{};b={method:b.method,headers:b.experimental_headers,j:u(Da,null,b),i:u(Ca,null,b),v:u(Ea,null,b),D:b.postData,type:"",current:window.location.href,B:window.location.href};b.method=((b.method||"GET")+"").toUpperCase();b.type=b.type||"request";var c=a,d=C["url-identifier"]||"";if(d){d=d.replace("__type__",b.type||"");var e=J(c,"#"),f=J(e[0],"?");c=f[0];var h=f[1];f=f[2];var k=e[1];e=e[2];if(0==d.lastIndexOf("?",0))h&&(d=d.replace("?","&")),f+=d;else{if(0==d.lastIndexOf(".",
0))if(H(c,"/"))d="index"+d;else{var g=c.lastIndexOf(".");-1<g&&(c=c.substring(0,g))}else H(c,"/")&&0==d.lastIndexOf("/",0)&&(d=d.substring(1));c+=d}c=c+h+f+k+e}d=S(c);c={};c.spfUrl=d;c.startTime=v();c.fetchStart=c.startTime;h=za(a,b.current,null,b.type,!1);h=Aa(h,b.current);c.spfPrefetched=!!h&&"prefetch"==h.type;c.spfCached=!!h;if(h){a=u(ua,null,a,b,c,h.key,h.response);b=window._spf_state=window._spf_state||{};var m=parseInt(b.uid,10)||0;m++;b=b.uid=m;M[b]=a;P?window.postMessage("spf:"+b,"*"):window.setTimeout(u(L,
null,b),0);a=null}else{h={};if(f=C["request-headers"])for(m in f)k=f[m],h[m]=null==k?"":String(k);if(b.headers)for(m in b.headers)k=b.headers[m],h[m]=null==k?"":String(k);null!=b.B&&(h["X-SPF-Referer"]=b.B);null!=b.current&&(h["X-SPF-Previous"]=b.current);if(m=C["advanced-header-identifier"])h["X-SPF-Request"]=m.replace("__type__",b.type),h.Accept="application/json";m=new Ba;f=u(ya,null,a,b,c,m);a={headers:h,C:C["request-timeout"],u:u(wa,null,a,m),l:u(xa,null,a,b,c,m),s:f,A:f};b.withCredentials&&
(a.withCredentials=b.withCredentials);C["advanced-response-type-json"]&&(a.responseType="json");a="POST"==b.method?ta("POST",d,b.D,a):ta("GET",d,null,a)}return a}},n=this;n.spf=n.spf||{};var Ga=n.spf,Z;for(Z in Fa)Ga[Z]=Fa[Z];}).call(this);
ØA—Eoúô   ­LÍÛþ5      

GET¹È  "
accept-rangesbytes"
age119656"®
alt-svc¢h3=":443"; ma=2592000,h3-29=":443"; ma=2592000,h3-Q050=":443"; ma=2592000,h3-Q046=":443"; ma=2592000,h3-Q043=":443"; ma=2592000,quic=":443"; ma=2592000; v="46,43"")
cache-controlpublic, max-age=31536000"
content-encodingbr"
content-length5201"
content-typetext/javascript"J
&cross-origin-opener-policy-report-only same-origin; report-to="youtube"",
cross-origin-resource-policycross-origin"%
dateFri, 20 Jan 2023 15:04:11 GMT"(
expiresSat, 20 Jan 2024 15:04:11 GMT".
last-modifiedFri, 20 Jan 2023 08:12:53 GMT"{
	report-ton{"group":"youtube","max_age":2592000,"endpoints":[{"url":"https://csp.withgoogle.com/csp/report-to/youtube"}]}"
serversffe"
varyAccept-Encoding, Origin"!
x-content-type-optionsnosniff"
x-xss-protection00˜ûˆŠØ¬ÔBJhttps://www.youtube.com/s/desktop/82a4cf4f/jsbin/network.vflset/network.jsP(Zh3`jtext/javascriptrGETx € ˆµ¿‰ŠØ¬Ô	jv~j/í®«r2¥~Ä7±ÄÆzò©£¹6º©Å=]ÍØA—Eoúô   Y×{iÌ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          NFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_ENABLE);
}

//Disable roll over if FIFO over flows
void MAX30105::disableFIFORollover(void) {
  bitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_DISABLE);
}

//Set number of samples to trigger the almost full interrupt (Page 18)
//Power on default is 32 samples
//Note it is reverse: 0x00 is 32 samples, 0x0F is 17 samples
void MAX30105::setFIFOAlmostFull(uint8_t numberOfSamples) {
  bitMask(MAX30105_FIFOCONFIG, MAX30105_A_FULL_MASK, numberOfSamples);
}

//Read the FIFO Write Pointer
uint8_t MAX30105::getWritePointer(void) {
  return (readRegister8(_i2caddr, MAX30105_FIFOWRITEPTR));
}

//Read the FIFO Read Pointer
uint8_t MAX30105::getReadPointer(void) {
  return (readRegister8(_i2caddr, MAX30105_FIFOREADPTR));
}


// Die Temperature
// Returns temp in C
float MAX30105::readTemperature() {
	
  //DIE_TEMP_RDY interrupt must be enabled
  //See issue 19: https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library/issues/19
  
  // Step 1: Config die temperature register to take 1 temperature sample
  writeRegister8(_i2caddr, MAX30105_DIETEMPCONFIG, 0x01);

  // Poll for bit to clear, reading is then complete
  // Timeout after 100ms
  unsigned long startTime = millis();
  while (millis() - startTime < 100)
  {
    //uint8_t response = readRegister8(_i2caddr, MAX30105_DIETEMPCONFIG); //Original way
    //if ((response & 0x01) == 0) break; //We're done!
    
	//Check to see if DIE_TEMP_RDY interrupt is set
	uint8_t response = readRegister8(_i2caddr, MAX30105_INTSTAT2);
    if ((response & MAX30105_INT_DIE_TEMP_RDY_ENABLE) > 0) break; //We're done!
    delay(1); //Let's not over burden the I2C bus
  }
  //TODO How do we want to fail? With what type of error?
  //? if(millis() - startTime >= 100) return(-999.0);

  // Step 2: Read die temperature register (integer)
  int8_t tempInt = readRegister8(_i2caddr, MAX30105_DIETEMPINT);
  uint8_t tempFrac = readRegister8(_i2caddr, MAX30105_DIETEMPFRAC); //Causes the clearing of the DIE_TEMP_RDY interrupt

  // Step 3: Calculate temperature (datasheet pg. 23)
  return (float)tempInt + ((float)tempFrac * 0.0625);
}

// Returns die temp in F
float MAX30105::readTemperatureF() {
  float temp = readTemperature();

  if (temp != -999.0) temp = temp * 1.8 + 32.0;

  return (temp);
}

// Set the PROX_INT_THRESHold
void MAX30105::setPROXINTTHRESH(uint8_t val) {
  writeRegister8(_i2caddr, MAX30105_PROXINTTHRESH, val);
}


//
// Device ID and Revision
//
uint8_t MAX30105::readPartID() {
  return readRegister8(_i2caddr, MAX30105_PARTID);
}

void MAX30105::readRevisionID() {
  revisionID = readRegister8(_i2caddr, MAX30105_REVISIONID);
}

uint8_t MAX30105::getRevisionID() {
  return revisionID;
}


//Setup the sensor
//The MAX30105 has many settings. By default we select:
// Sample Average = 4
// Mode = MultiLED
// ADC Range = 16384 (62.5pA per LSB)
// Sample rate = 50
//Use the default setup if you are just getting started with the MAX30105 sensor
void MAX30105::setup(byte powerLevel, byte sampleAverage, byte ledMode, int sampleRate, int pulseWidth, int adcRange) {
  softReset(); //Reset all configuration, threshold, and data registers to POR values

  //FIFO Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //The chip will average multiple samples of same type together if you wish
  if (sampleAverage == 1) setFIFOAverage(MAX30105_SAMPLEAVG_1); //No averaging per FIFO record
  else if (sampleAverage == 2) setFIFOAverage(MAX30105_SAMPLEAVG_2);
  else if (sampleAverage == 4) setFIFOAverage(MAX30105_SAMPLEAVG_4);
  else if (sampleAverage == 8) setFIFOAverage(MAX30105_SAMPLEAVG_8);
  else if (sampleAverage == 16) setFIFOAverage(MAX30105_SAMPLEAVG_16);
  else if (sampleAverage == 32) setFIFOAverage(MAX30105_SAMPLEAVG_32);
  else setFIFOAverage(MAX30105_SAMPLEAVG_4);

  //setFIFOAlmostFull(2); //Set to 30 samples to trigger an 'Almost Full' interrupt
  enableFIFORollover(); //Allow FIFO to wrap/roll over
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Mode Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  if (ledMode == 3) setLEDMode(MAX30105_MODE_MULTILED); //Watch all three LED channels
  else if (ledMode == 2) setLEDMode(MAX30105_MODE_REDIRONLY); //Red and IR
  else setLEDMode(MAX30105_MODE_REDONLY); //Red only
  activeLEDs = ledMode; //Used to control how many bytes to read from FIFO buffer
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Particle Sensing Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  if(adcRange < 4096) setADCRange(MAX30105_ADCRANGE_2048); //7.81pA per LSB
  else if(adcRange < 8192) setADCRange(MAX30105_ADCRANGE_4096); //15.63pA per LSB
  else if(adcRange < 16384) setADCRange(MAX30105_ADCRANGE_8192); //31.25pA per LSB
  else if(adcRange == 16384) setADCRange(MAX30105_ADCRANGE_16384); //62.5pA per LSB
  else setADCRange(MAX30105_ADCRANGE_2048);

  if (sampleRate < 100) setSampleRate(MAX30105_SAMPLERATE_50); //Take 50 samples per second
  else if (sampleRate < 200) setSampleRate(MAX30105_SAMPLERATE_100);
  else if (sampleRate < 400) setSampleRate(MAX30105_SAMPLERATE_200);
  else if (sampleRate < 800) setSampleRate(MAX30105_SAMPLERATE_400);
  else if (sampleRate < 1000) setSampleRate(MAX30105_SAMPLERATE_800);
  else if (sampleRate < 1600) setSampleRate(MAX30105_SAMPLERATE_1000);
  else if (sampleRate < 3200) setSampleRate(MAX30105_SAMPLERATE_1600);
  else if (sampleRate == 3200) setSampleRate(MAX30105_SAMPLERATE_3200);
  else setSampleRate(MAX30105_SAMPLERATE_50);

  //The longer the pulse width the longer range of detection you'll have
  //At 69us and 0.4mA it's about 2 inches
  //At 411us and 0.4mA it's about 6 inches
  if (pulseWidth < 118) setPulseWidth(MAX30105_PULSEWIDTH_69); //Page 26, Gets us 15 bit resolution
  else if (pulseWidth < 215) setPulseWidth(MAX30105_PULSEWIDTH_118); //16 bit resolution
  else if (pulseWidth < 411) setPulseWidth(MAX30105_PULSEWIDTH_215); //17 bit resolution
  else if (pulseWidth == 411) setPulseWidth(MAX30105_PULSEWIDTH_411); //18 bit resolution
  else setPulseWidth(MAX30105_PULSEWIDTH_69);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //LED Pulse Amplitude Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //Default is 0x1F which gets us 6.4mA
  //powerLevel = 0x02, 0.4mA - Presence detection of ~4 inch
  //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
  //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
  //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch

  setPulseAmplitudeRed(powerLevel);
  setPulseAmplitudeIR(powerLevel);
  setPulseAmplitudeGreen(powerLevel);
  setPulseAmplitudeProximity(powerLevel);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Multi-LED Mode Configuration, Enable the reading of the three LEDs
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  enableSlot(1, SLOT_RED_LED);
  if (ledMode > 1) enableSlot(2, SLOT_IR_LED);
  if (ledMode > 2) enableSlot(3, SLOT_GREEN_LED);
  //enableSlot(1, SLOT_RED_PILOT);
  //enableSlot(2, SLOT_IR_PILOT);
  //enableSlot(3, SLOT_GREEN_PILOT);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  clearFIFO(); //Reset the FIFO before we begin checking the sensor
}

//
// Data Collection
//

//Tell caller how many samples are available
uint8_t MAX30105::available(void)
{
  int8_t numberOfSamples = sense.head - sense.tail;
  if (numberOfSamples < 0) numberOfSamples += STORAGE_SIZE;

  return (numberOfSamples);
}

//Report the most recent red value
uint32_t MAX30105::getRed(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.red[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

//Report the most recent IR value
uint32_t MAX30105::getIR(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.IR[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

//Report the most recent Green value
uint32_t MAX30105::getGreen(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.green[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

//Report the next Red value in the FIFO
uint32_t MAX30105::getFIFORed(void)
{
  return (sense.red[sense.tail]);
}

//Report the next IR value in the FIFO
uint32_t MAX30105::getFIFOIR(void)
{
  return (sense.IR[sense.tail]);
}

//Report the next Green value in the FIFO
uint32_t MAX30105::getFIFOGreen(void)
{
  return (sense.green[sense.tail]);
}

//Advance the tail
void MAX30105::nextSample(void)
{
  if(available()) //Only advance the tail if new data is available
  {
    sense.tail++;
    sense.tail %= STORAGE_SIZE; //Wrap condition
  }
}

//Polls the sensor for new data
//Call regularly
//If new data is available, it updates the head and tail in the main struct
//Returns number of new samples obtained
uint16_t MAX30105::check(void)
{
  //Read register FIDO_DATA in (3-byte * number of active LED) chunks
  //Until FIFO_RD_PTR = FIFO_WR_PTR

  byte readPointer = getReadPointer();
  byte writePointer = getWritePointer();

  int numberOfSamples = 0;

  //Do we have new data?
  if (readPointer != writePointer)
  {
    //Calculate the number of readings we need to get from sensor
    numberOfSamples = writePointer - readPointer;
    if (numberOfSamples < 0) numberOfSamples += 32; //Wrap condition

    //We now have the number of readings, now calc bytes to read
    //For this example we are just doing Red and IR (3 bytes each)
    int bytesLeftToRead = numberOfSamples * activeLEDs * 3;

    //Get ready to read a burst of data from the FIFO register
    _i2cPort->beginTransmission(MAX30105_ADDRESS);
    _i2cPort->write(MAX30105_FIFODATA);
    _i2cPort->endTransmission();

    //We may need to read as many as 288 bytes so we read in blocks no larger than I2C_BUFFER_LENGTH
    //I2C_BUFFER_LENGTH changes based on the platform. 64 bytes for SAMD21, 32 bytes for Uno.
    //Wire.requestFrom() is limited to BUFFER_LENGTH which is 32 on the Uno
    while (bytesLeftToRead > 0)
    {
      int toGet = bytesLeftToRead;
      if (toGet > I2C_BUFFER_LENGTH)
      {
        //If toGet is 32 this is bad because we read 6 bytes (Red+IR * 3 = 6) at a time
        //32 % 6 = 2 left over. We don't want to request 32 bytes, we want to request 30.
        //32 % 9 (Red+IR+GREEN) = 5 left over. We want to request 27.

        toGet = I2C_BUFFER_LENGTH - (I2C_BUFFER_LENGTH % (activeLEDs * 3)); //Trim toGet to be a multiple of the samples we need to read
      }

      bytesLeftToRead -= toGet;

      //Request toGet number of bytes from sensor
      _i2cPort->requestFrom(MAX30105_ADDRESS, toGet);
      
      while (toGet > 0)
      {
        sense.head++; //Advance the head of the storage struct
        sense.head %= STORAGE_SIZE; //Wrap condition

        byte temp[sizeof(uint32_t)]; //Array of 4 bytes that we will convert into long
        uint32_t tempLong;

        //Burst read three bytes - RED
        temp[3] = 0;
        temp[2] = _i2cPort->read();
        temp[1] = _i2cPort->read();
        temp[0] = _i2cPort->read();

        //Convert array to long
        memcpy(&tempLong, temp, sizeof(tempLong));
		
		tempLong &= 0x3FFFF; //Zero out all but 18 bits

        sense.red[sense.head] = tempLong; //Store this reading into the sense array

        if (activeLEDs > 1)
        {
          //Burst read three more bytes - IR
          temp[3] = 0;
          temp[2] = _i2cPort->read();
          temp[1] = _i2cPort->read();
          temp[0] = _i2cPort->read();

          //Convert array to long
          memcpy(&tempLong, temp, sizeof(tempLong));

		  tempLong &= 0x3FFFF; //Zero out all but 18 bits
          
		  sense.IR[sense.head] = tempLong;
        }

        if (activeLEDs > 2)
        {
          //Burst read three more bytes - Green
          temp[3] = 0;
          temp[2] = _i2cPort->read();
          temp[1] = _i2cPort->read();
          temp[0] = _i2cPort->read();

          //Convert array to long
          memcpy(&tempLong, temp, sizeof(tempLong));

		  tempLong &= 0x3FFFF; //Zero out all but 18 bits

          sense.green[sense.head] = tempLong;
        }

        toGet -= activeLEDs * 3;
      }

    } //End while (bytesLeftToRead > 0)

  } //End readPtr != writePtr

  return (numberOfSamples); //Let the world know how much new data we found
}

//Check for new data but give up after a certain amount of time
//Returns true if new data was found
//Returns false if new data was not found
bool MAX30105::safeCheck(uint8_t maxTimeToCheck)
{
  uint32_t markTime = millis();
  
  while(1)
  {
	if(millis() - markTime > maxTimeToCheck) return(false);

	if(check() == true) //We found new data!
	  return(true);

	delay(1);
  }
}

//Given a register, read it, mask it, and then set the thing
void MAX30105::bitMask(uint8_t reg, uint8_t mask, uint8_t thing)
{
  // Grab current register context
  uint8_t originalContents = readRegister8(_i2caddr, reg);

  // Zero-out the portions of the register we're interested in
  originalContents = originalContents & mask;

  // Change contents
  writeRegister8(_i2caddr, reg, originalContents | thing);
}

//
// Low-level I2C Communication
//
uint8_t MAX30105::readRegister8(uint8_t address, uint8_t reg) {
  _i2cPort->beginTransmission(address);
  _i2cPort->write(reg);
  _i2cPort->endTransmission(false);

  _i2cPort->requestFrom((uint8_t)address, (uint8_t)1); // Request 1 byte
  if (_i2cPort->available())
  {
    return(_i2cPort->read());
  }

  return (0); //Fail

}

void MAX30105::writeRegister8(uint8_t address, uint8_t reg, uint8_t value) {
  _i2cPort->beginTransmission(address);
  _i2cPort->write(reg);
  _i2cPort->write(value);
  _i2cPort->endTransmission();
}
