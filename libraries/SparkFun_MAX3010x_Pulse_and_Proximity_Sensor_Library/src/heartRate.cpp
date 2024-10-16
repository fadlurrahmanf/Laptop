/*
 Optical Heart Rate Detection (PBA Algorithm)
 By: Nathan Seidle
 SparkFun Electronics
 Date: October 2nd, 2016
 
 Given a series of IR samples from the MAX30105 we discern when a heart beat is occurring

 Let's have a brief chat about what this code does. We're going to try to detect
 heart-rate optically. This is tricky and prone to give false readings. We really don't
 want to get anyone hurt so use this code only as an example of how to process optical
 data. Build fun stuff with our MAX30105 breakout board but don't use it for actual
 medical diagnosis.

 Excellent background on optical heart rate detection:
 http://www.ti.com/lit/an/slaa655/slaa655.pdf

 Good reading:
 http://www.techforfuture.nl/fjc_documents/mitrabaratchi-measuringheartratewithopticalsensor.pdf
 https://fruct.org/publications/fruct13/files/Lau.pdf

 This is an implementation of Maxim's PBA (Penpheral Beat Amplitude) algorithm. It's been 
 converted to work within the Arduino framework.
*/

/* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
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
* 
*/

#include "heartRate.h"

int16_t IR_AC_Max = 20;
int16_t IR_AC_Min = -20;

int16_t IR_AC_Signal_Current = 0;
int16_t IR_AC_Signal_Previous;
int16_t IR_AC_Signal_min = 0;
int16_t IR_AC_Signal_max = 0;
int16_t IR_Average_Estimated;

int16_t positiveEdge = 0;
int16_t negativeEdge = 0;
int32_t ir_avg_reg = 0;

int16_t cbuf[32];
uint8_t offset = 0;

static const uint16_t FIRCoeffs[12] = {172, 321, 579, 927, 1360, 1858, 2390, 2916, 3391, 3768, 4012, 4096};

//  Heart Rate Monitor functions takes a sample value and the sample number
//  Returns true if a beat is detected
//  A running average of four samples is recommended for display on the screen.
bool checkForBeat(int32_t sample)
{
  bool beatDetected = false;

  //  Save current state
  IR_AC_Signal_Previous = IR_AC_Signal_Current;
  
  //This is good to view for debugging
  //Serial.print("Signal_Current: ");
  //Serial.println(IR_AC_Signal_Current);

  //  Process next data sample
  IR_Average_Estimated = averageDCEstimator(&ir_avg_reg, sample);
  IR_AC_Signal_Current = lowPassFIRFilter(sample - IR_Average_Estimated);

  //  Detect positive zero crossing (rising edge)
  if ((IR_AC_Signal_Previous < 0) & (IR_AC_Signal_Current >= 0))
  {
  
    IR_AC_Max = IR_AC_Signal_max; //Adjust our AC max and min
    IR_AC_Min = IR_AC_Signal_min;

    positiveEdge = 1;
    negativeEdge = 0;
    IR_AC_Signal_max = 0;

    //if ((IR_AC_Max - IR_AC_Min) > 100 & (IR_AC_Max - IR_AC_Min) < 1000)
    if ((IR_AC_Max - IR_AC_Min) > 20 & (IR_AC_Max - IR_AC_Min) < 1000)
    {
  MAM�:  �������������	�����������������y��������������������������������������������������	�������	�
�����	���	���	�
�	�������	��       
      �x 
    ���    �����  ���� � �������������
 �p����� �{��� ������  
����� �����������                     ����mIh��#E>���E2���M�{֭֕���h�/j"�+{�&R�S:"Q�Z�Fk[Ts�)x����ԛ�}b;�sQ�Nۑ��-�+��[k8�M�4?�<�p����4~�!p#�F�q0��A�}%��Ȅ�,�A*jF�L�qPM80~n�3(�1���]	`y2���`$�\ll����rETD�v`�.��}5�S�A�*����R,����D` ����EGA�X��-E����$se` 6it~6�Q�r(�#���\J?B��0�\.X� �@�Nu	FA��T�+����T��u	>A_���8P���"q66��D�J�T��*e~�^��j�.d��72����o�f`���[,& ���@[준l}�f�۠���Py�]L��&��t�h��Z�>[�m@[�y��&m���6� 8Yg.�����p;�g���#`����,��к����`��0A��838�@��[(bh��z 8bD��� ��9 t�µ��ڥ>NQ�X
S�,���� ��@#p�HD�`��)���fhС�PjmbH��x`������A$04�@��lH��ZS��z 0���A$	$J0T��3�� <�@4��6u4���$�&�@	�JX��d)�,�Ds�ς<4�����5w�ƯTr���� }a �(��:�| wl�4ۀ4���	#k�t�i@^�����e��X .���@Д��`�t��� �`2�����a07`,(�s6�I�X�&	aśH�l��:�H�h0(8b``5p�Q5<�|N�J���&A��#h �	ILÀ3�pH��N�