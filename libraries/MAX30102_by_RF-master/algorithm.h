/** \file algorithm.h ******************************************************
*
* Project: MAXREFDES117#
* Filename: algorithm.h
* Description: This module is the heart rate/SpO2 calculation algorithm header file
*
* Revision History:
*\n 1-18-2016 Rev 01.00 SK Initial release.
*\n
*
* --------------------------------------------------------------------
*
* This code follows the following naming conventions:
*
*\n char              ch_pmod_value
*\n char (array)      s_pmod_s_string[16]
*\n float             f_pmod_value
*\n int32_t           n_pmod_value
*\n int32_t (array)   an_pmod_value[16]
*\n int16_t           w_pmod_value
*\n int16_t (array)   aw_pmod_value[16]
*\n uint16_t          uw_pmod_value
*\n uint16_t (array)  auw_pmod_value[16]
*\n uint8_t           uch_pmod_value
*\n uint8_t (array)   auch_pmod_buffer[16]
*\n uint32_t          un_pmod_value
*\n int32_t *         pn_pmod_value
*
* ------------------------------------------------------------------------- */
/*******************************************************************************
* Copyright (C) 2015 Maxim Integrated Products, Inc., All Rights Reserved.
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
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/
#ifndef ALGORITHM_H_
#define ALGORITHM_H_
#include <arduino.h>

#define true 1
#define false 0
#define FS 25    //sampling frequency
#define BUFFER_SIZE  (FS* 4) 
#define MA4_SIZE  4 // DONOT CHANGE
#define BUFFER_SIZE_MA4 BUFFER_SIZE-MA4_SIZE
#define min(x,y) ((x) < (y) ? (x) : (y))

//uch_spo2_table is approximated as  -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845 ;
//const uint8_t uch_spo2_table[184]={ 95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99, 
//              99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 
//              100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97, 
//              97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91, 
//              90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83, 82, 82, 81, 81, 
//              80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73, 72, 72, 71, 70, 69, 69, 68, 67, 
//              66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 
//              49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29, 
//              28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5, 
//              3, 2, 1 } ;
//
//uch_spo2_table is approximated ASU!VPSz!#�   �  �  x�uQ������d�����a�uF�Ȍ3/m����S`��S��
���� wt�)����Y��X-y?�Cj��ϧ�.,�qk�ZI��M�r���xl�7V���0e��䞒�m�_�5e����ט��ȷ>埧2q����jކ��s9��gx�R�A]A�In�da� ��	7\���*�6��>���E�����-��_�=��Ϟ�~fJ��OG���ǯ��b��r.l^�ݟ��W}���p�h��s�r!�������E�oZ�\Š��b��i�>�g%3�]�m��v��?���39��U��l9�˰~Mv��7e���B#��gH��I�;�����KU�K�'�3�&?9��n�\��GQK��X�y��Y�h�b������s����ns�ݱ�nƲ��ޯ��0�)�؇����U_J61X,<�=��GQZZ��)�_���U���ô������a�_��M{�+�W�]0��S������
g�&�0�5��Y�B��M�Ysu���~%���) �(�I��[����w�r/TO�����!4�X{0��Z��-�V�������Y�&vRA�4U��c����,J�;2�^=�5��z�������L� sJv4�=�(af�X�o�T斩a&8�`���s�V��DnE���ט� X��~��[����D\M��v� ��e432���V*�����Q�/���
 թLW�m
�`>���#n2 30&x�p?���.Q%����-,㡶�4�8��>LJ� :�7P���UJhY�F�uH;H�G�H
:>LASWSig2B                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            