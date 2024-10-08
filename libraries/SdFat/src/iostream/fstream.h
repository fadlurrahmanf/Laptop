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
/**
 * \file
 * \brief iostreams for files.
 */
#ifndef fstream_h
#define fstream_h
#include "iostream.h"
//------------------------------------------------------------------------------
/**
 * \class StreamBaseClass
 * \brief base type for FAT and exFAT streams
 */
class StreamBaseClass : protected StreamBaseFile, virtual public ios {
 protected:
  void clearWriteError() {
    StreamBaseFile::clearWriteError();
  }
  /* Internal do not use
   * \return mode
   */
  int16_t getch();
  bool getWriteError() {
    return StreamBaseFile::getWriteError();
  }
  void open(const char* path, ios::openmode mode);
  /** Internal do not use
   * \return mode
   */
  ios::openmode getmode() {
    return m_mode;
  }
  void putch(char c);
  void putstr(const char *str);
  bool seekoff(off_type off, seekdir way);
  /** Internal do not use
   * \param[in] pos
   */
  bool seekpos(pos_type pos) {
    return StreamBaseFile::seekSet(pos);
  }
  /** Internal do not use
   * \param[in] mode
   */
  void setmode(ios::openmode mode) {
    m_mode = mode;
  }
  int write(const void* buf, size_t n);
  void write(char c);

 private:
  ios::openmode m_mode;
};
//==============================================================================
/**
 * \class fstream
 * \brief file input/output stream.
 */
class fstream : public iostream, StreamBaseClass  {
 public:
  using iostream::peek;
  fstream() {}
  /** Constructor with open
   * \param[in] path file to open
   * \param[in] mode open mode
   */
  explicit fstream(const char* path, openmode mode = in | out) {
    open(path, mode);
  }
#if DESTRUCTOR_CLOSES_FILE
  ~fstream() {}
#endif  // DESTRUCTOR_CLOSES_FILE
  /** Clear state and writeError
   * \param[in] state new state for stream
   */
  void clear(iostate state = goodbit) {
    ios::clear(state);
    StreamBaseClass::clearWriteError();
  }
  /**  Close a file and force cached data and directory information
   *  to be written to the storage device.
   */
  void close() {
    StreamBaseClass::close();
  }
  /** Open a fstream
   * \param[in] path path to open
   * \param[in] mode open mode
   *
   * Valid open modes are (at end, ios::ate, and/or ios::binary may be added):
   *
   * ios::in - Open file for reading.
   *
   * ios::out or ios::out | ios::trunc - Truncate to 0 length, if existent,
   * or create a file for writing only.
   *
   * ios::app or ios::out | ios::app - Append; open or create file for
   * writing at end-of-file.
   *
   * ios::in | ios::out - Open file for update (reading and writing).
   *
   * ios::in | ios::out | ios::trunc - Truncate to zero length, if existent,
   * or create file for update.
   *
   * ios::in | ios::app or ios::in | ios::out | ios::app - Append; open or
   * create text file for update, writing at end of file.
   */
  void open(const char* path, openmode mode = in | out) {
    StreamBaseClass::open(path, mode);
  }
 {"expect_ct":[],"sts":[{"expiry":1704718954.491781,"host":"C47LSS67LOPR/nzUq66AFtkSkYv/QndegwCOfDtIPDU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673182954.491787},{"expiry":1702647756.231993,"host":"Hi4bEdMq563Qsqn4sVyUls/uVk7U80IxMa3wyWVUqWU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1671111756.232},{"expiry":1705884671.460205,"host":"OuKlWsMW1dkkbI1X/oi6o0Y95ZNSWnSoeaIXAEYPlv4=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674348671.460208},{"expiry":1705111798.221726,"host":"Q4G6aIdezq0rDi89KmjqFSp7cn3C9gwZwsLa7+P5K38=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673575798.221728},{"expiry":1705884716.319204,"host":"o2oGFfZkps1NfT6asVPXqcBtIJRMNNqoO1jKhddjlmY=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348716.319212},{"expiry":1705884662.303077,"host":"6Turz/dyrfpEyob9ip5MOi6TNRgdxLvniY94TmP8nX8=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348662.303084},{"expiry":1705725903.841684,"host":"+RF8qPV/GSCVzxCzVhnVv/EK1dVO9QxZSHrOr6TxzzM=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674189903.841688}],"version":2}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {"expect_ct":[],"sts":[{"expiry":1676940657.8231,"host":"EYHE410YE+QLRiG5eVa37sRP76lJYGunuxQ++4jMfzo=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348657.823101},{"expiry":1704186046.788495,"host":"E4NcNYCn7Ptof4WJR58R22onvjouhFi5jiUxkcFFLHw=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1672650046.788498},{"expiry":1689915128.289945,"host":"GM2OF9XwumvzsJzVyjvHSHaoFID6tLsBqvQdP+69iKk=","mode