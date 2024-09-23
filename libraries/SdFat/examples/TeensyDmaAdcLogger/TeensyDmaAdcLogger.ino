// Test of Teensy exFAT DMA ADC logger.
// This is mainly to test use of RingBuf in an ISR.
// You should modify it for serious use as a data logger.
//
#include "DMAChannel.h"
#include "SdFat.h"
#include "FreeStack.h"
#include "RingBuf.h"

// 400 sector RingBuf - could be larger on Teensy 4.1.
const size_t RING_BUF_SIZE = 400*512;

// Preallocate 8GiB file.
const uint64_t PRE_ALLOCATE_SIZE = 8ULL << 30;

// Use FIFO SDIO.
#define SD_CONFIG SdioConfig(FIFO_SDIO)

DMAChannel dma(true);

SdFs sd;

FsFile file;
//------------------------------------------------------------------------------
// Ping-pong DMA buffer.
DMAMEM static uint16_t __attribute__((aligned(32))) dmaBuf[2][256];
size_t dmaCount;

// RingBuf for 512 byte sectors.
RingBuf<FsFile, RING_BUF_SIZE> rb;

// Shared between ISR and background.
volatile size_t maxBytesUsed;

volatile bool overrun;
//------------------------------------------------------------------------------
//ISR.
static void isr() {
  if (rb.bytesFreeIsr() >= 512 && !overrun) {
    rb.memcpyIn(dmaBuf[dmaCount & 1], 512);
    dmaCount++;
    if (rb.bytesUsed() > maxBytesUsed) {
      maxBytesUsed = rb.bytesUsed();
    }
  } else {
    overrun = true;
  }
  dma.clearComplete();
  dma.clearInterrupt();
#if defined(__IMXRT1062__)
  // Handle clear interrupt glitch in Teensy 4.x!
  asm("DSB");
#endif  // defined(__IMXRT1062__)
}
//------------------------------------------------------------------------------
// Over-clocking will degrade quality - use only for stress testing.
void overclock() {
#if defined(__IMXRT1062__) // Teensy 4.0
  ADC1_CFG  =
    // High Speed Configuration
    ADC_CFG_ADHSC |
    // Sample period 3 clocks
    ADC_CFG_ADSTS(0) |
    // Input clock
    ADC_CFG_ADIV(0) |
    // Not selected - Long Sample Time Configuration
    // ADC_CFG_ADLSMP |
    // 12-bit
    ADC_CFG_MODE(2) |
    // Asynchronous clock
    ADC_CFG_ADICLK(3);
#else // defined(__IMXRT1062__)
  // Set 12 bit mode and max over-clock
  ADC0_CFG1 =
    // Clock divide select, 0=direct, 1=div2, 2=div4, 3=div8
    ADC_CFG1_ADIV(0) |
    // Sample time configuration, 0=Short, 1=Long
    // ADC_CFG1_ADLSMP |
    // Conversion mode, 0=8 bit, 1=12 bit, 2=10 bit, 3=16 bit
    ADC_CFG1_MODE(1) |
    // Input clock, 0=bus, 1=bus/2, 2=OSCERCLK, 3=async
    ADC_CFG1_ADICLK(0);

  ADC0_CFG2 = ADC_CFG2_MUXSEL | ADC_CFG2_ADLSTS(3);
#endif  // defined(__IMXRT1062__)
}
//------------------------------------------------------------------------------
#if defined(__IMXRT1062__) // Teensy 4.0
#define SOURCE_SADDR ADC1_R0
#define SOURCE_EVENT DMAMUX_SOURCE_ADC1
#else
#define SOURCE_SADDR ADC0_RA
#define SOURCE_EVENT DMAMUX_SOURCE_ADC0
#endif
//------------------------------------------------------------------------------
// Should replace ADC stuff with calls to Teensy ADC library.
// https://github.com/pedvide/ADC
static void init(uint8_t pin) {
  uint32_t adch;
	uint32_t i, sum = 0;
	// Actually, do many normal reads, to start with a nice DC level
	for (i=0; i < 1024; i++) {
		sum += analogRead(pin);
	}
#if defined(__IMXRT1062__) // Teensy 4.0
  // save channel
  adch = ADC1_HC0 & 0x1F;
  // Continuous conversion , DMA enable
  ADC1_GC = ADC_GC_ADCO | ADC_GC_DMAEN;
  // start conversion
  ADC1_HC0 = adch;
#else  // defined(__IMXRT1062__) // Teensy 4.0
  // save channel
  adch = ADC0_SC1A & 0x1F;
  // DMA enable
  ADC0_SC2 |= ADC_SC2_DMAEN;
  // Continuous conversion enable
  ADC0_SC3 = ADC_SC3_ADCO;
  // Start ADC
  ADC0_SC1A = adch;
 #endif  // defined(__IMXRT1062__) // Teensy 4.0
	// set up a DMA channel to store the ADC data
 	dma.attachInterrupt(isr);
	dma.begin();
  dma.source((volatile const signed short &)SOURCE_SADDR);
  dma.destinationBuffer((volatile uint16_t*)dmaBuf, sizeof(dmaBuf));
  dma.interruptAtHalf();
  dma.interruptAtCompletion();
	dma.triggerAtHardwareEvent(SOURCE_EVENT);
	dma.enable();
}
//------------------------------------------------------------------------------
void stopDma() {
#if defined(__IMXRT1062__) // Teensy 4.0
  ADC1_GC = 0;
#else  // defined(__IMXRT1062__)
  ADC0_SC3 = 0;
#endif  // defined(__IM{"net":{"http_server_properties":{"servers":[{"isolation":[],"server":"https://s0.2mdn.net","supports_spdy":true},{"isolation":[],"network_stats":{"srtt":22966},"server":"https://gcdn.2mdn.net","supports_spdy":true},{"isolation":[],"server":"https://pagead2.googlesyndication.com","supports_spdy":true},{"isolation":[],"server":"https://googleads4.g.doubleclick.net","supports_spdy":true},{"isolation":[],"server":"https://sync.tidaltv.com","supports_spdy":true},{"isolation":[],"server":"https://sync-tm.everesttech.net","supports_spdy":true},{"isolation":[],"server":"https://ws.rqtrk.eu","supports_spdy":true},{"isolation":[],"server":"https://insight.adsrvr.org","supports_spdy":true},{"isolation":[],"network_stats":{"srtt":23165},"server":"https://cm.g.doubleclick.net","supports_spdy":true},{"isolation":[],"network_stats":{"srtt":22400},"server":"https://pixel.tapad.com","supports_spdy":true},{"isolation":[],"network_stats":{"srtt":22342},"server":"https://odr.mookie1.com","supports_spdy":true},{"isolation":[],"server":"https://track.leanlab.co","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319448021836175","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":28409},"server":"https://googleads.g.doubleclick.net","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319448021853226","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":22848},"server":"https://ad.doubleclick.net","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13319448081974899","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":26603},"server":"https://ade.googlesyndication.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13320248554491698","port":443,"protocol_str":"quic"}],"isolation":[],"server":"https://clienttoken.spotify.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13320641398220447","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":23049},"server":"https://bloodhound.spotify.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13321165489416675","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":23189},"server":"https://apresolve.spotify.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13321255472697600","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":23962},"server":"https://securepubads.g.doubleclick.net","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13321255503746285","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":22590},"server":"https://pubads.g.doubleclick.net","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"]