/**
 * Copyright (c) 2011-2022 Bill Greiman
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
#ifndef ostream_h
#define ostream_h
/**
 * \file
 * \brief \ref ostream class
 */
#include "ios.h"
//==============================================================================
/**
 * \class ostream
 * \brief Output Stream
 */
class ostream : public virtual ios {
 public:
  ostream() {}

  /** call manipulator
   * \param[in] pf function to call
   * \return the stream
   */
  ostream& operator<< (ostream& (*pf)(ostream& str)) {
    return pf(*this);
  }
  /** call manipulator
   * \param[in] pf function to call
   * \return the stream
   */
  ostream& operator<< (ios_base& (*pf)(ios_base& str)) {
    pf(*this);
    return *this;
  }
  /** Output bool
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (bool arg) {
    putBool(arg);
    return *this;
  }
  /** Output string
   * \param[in] arg string to output
   * \return the stream
   */
  ostream &operator<< (const char *arg) {
    putStr(arg);
    return *this;
  }
  /** Output string
   * \param[in] arg string to output
   * \return the stream
   */
  ostream &operator<< (const signed char *arg) {
    putStr((const char*)arg);
    return *this;
  }
  /** Output string
   * \param[in] arg string to output
   * \return the stream
   */
  ostream &operator<< (const unsigned char *arg) {
    putStr((const char*)arg);
    return *this;
  }
#if ENABLE_ARDUINO_STRING
  /** Output string
   * \param[in] arg string to output
   * \return the stream
   */
  ostream &operator<< (const String& arg) {
    putStr(arg.c_str());
    return *this;
  }
#endif  // ENABLE_ARDUINO_STRING
  /** Output character
   * \param[in] arg character to output
   * \return the stream
   */
  ostream &operator<< (char arg) {
    putChar(arg);
    return *this;
  }
  /** Output character
   * \param[in] arg character to output
   * \return the stream
   */
  ostream &operator<< (signed char arg) {
    putChar(static_cast<char>(arg));
    return *this;
  }
  /** Output character
   * \param[in] arg character to output
   * \return the stream
   */
  ostream &operator<< (unsigned char arg) {
    putChar(static_cast<char>(arg));
    return *this;
  }
  /** Output double
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (double arg) {
    putDouble(arg);
    return *this;
  }
  /** Output float
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (float arg) {
    putDouble(arg);
    return *this;
  }
  /** Output signed short
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (short arg) {  // NOLINT
    putNum((int32_t)arg);
    return *this;
  }
  /** Output unsigned short
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (unsigned short arg) {  // NOLINT
    putNum((uint32_t)arg);
    return *this;
  }
  /** Output signed int
   * \par{"expect_ct":[],"sts":[{"expiry":1676940674.264425,"host":"EYHE410YE+QLRiG5eVa37sRP76lJYGunuxQ++4jMfzo=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348674.264427},{"expiry":1705884692.561298,"host":"KG2CnBR10hfRQyMyo7pYGD3mF6oRCaYMYkjznZzYXwM=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348692.5613},{"expiry":1705877466.510288,"host":"LrVF2wAcXHZTCqn15adJ2OSVd42DkPw4lABvyR42Vqk=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674341466.510292},{"expiry":1705884691.86235,"host":"OuKlWsMW1dkkbI1X/oi6o0Y95ZNSWnSoeaIXAEYPlv4=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674348691.862353},{"expiry":1705884678.062648,"host":"P7URR8ssSoX7XJQ2seyUw7Etu+do9RxYqDzxjV/i5p0=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674348678.062658},{"expiry":1705884670.506051,"host":"Q7rGViBtltYYKhE4KgXqrgjVuPzAAznX677M9eGKRRg=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674348670.506053},{"expiry":1705884673.329959,"host":"oubsJ7Rvy9mrPhMBlOXr5n0WKssh8l/xCL0Wc6gK4HI=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348673.329961},{"expiry":1700874261.346757,"host":"rUX7HhAjUgygcOV13LA6463As5CQOfX5O1EnqngN6aE=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1669338261.34676},{"expiry":1676933469.988893,"host":"3rdtKlKEXL3VOG/n9roq5yc71B6vVvV5+NTx3Ps+o4s=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674341469.988896},{"expiry":1705877470.278825,"host":"31j3kaACZEpi0s5N/jPzPL4U7l0PqibJx386MrAzVIg=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674341470.278828},{"expiry":1705877470.483762,"host":"9PEpT6mdAJ8S0VRDY3666QF03uUuIc1TEeu3E3ZE6PM=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674341470.483765},{"expiry":1705877468.208885,"host":"/eT7udH/j8xfYVz6vfgfnKxbd1MSDYCcqc2XsbsCvYY=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674341468.208889}],"version":2}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {"expect_ct":[],"sts":[{"expiry":1704718954.491781,"host":"C47LSS67LOPR/nzUq66AFtkSkYv/QndegwCOfDtIPDU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673182954.491787},{"expiry":1702647756.231993,"host":"Hi4bEdMq563Qsqn4sVyUls/uVk7U80IxMa3wyWVUqWU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1671111756.232},{"expiry":1705884671.460205,"host":"OuKlWsMW1dkkbI1X/oi6o0Y95ZNSWnSoeaIXAEYPlv4=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674348671.460208},{"expiry":1705111798.221726,"host":"Q4G6aIdezq0rDi89KmjqFSp7cn3C9gwZwsLa7+P5K38=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673575798.221728},{"expiry":1705884697.286154,"host":"o2oGFfZkps1NfT6asVPXqcBtIJRMNNqoO1jKhddjlmY=","mode":"force-https","