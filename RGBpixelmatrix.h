/**
 * Copyright (c) Damian Schneider 2015
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */


/*
  RGBpixelmatrix.h - A library to handle WS2811 pixel based
 color matrix.
 Created by Damian Schneider, Jan. 27th, 2013.
 
 The library contains functions to write and read pixel colors,
 as well as a function to send pixels to hardware.
 Each pixel is represented by a 24bit RGB value (8bit per color).
 Maximum total number of pixels is 600 (ram limit). 
 Maximum width is 85, maximum length is 85. (can be increased by using 16bit integers to represent the width)
 
 Default autput pin is pin7, modify in sendByte function
 to change to different pin.
 
 Note: to keep the library fast, functions do not error check inputs.
 If you read or write pixels that are out of boundaries, random stuff 
 may happen.
 */
 
#ifndef RGBpixelmatrix_h
#define RGBpixelmatrix_h


#include "Arduino.h"

  struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };	
  
class RGBpixelmatrix
{
public:

  RGBpixelmatrix(uint8_t width, uint8_t height); //constructor, returns -1 if failed
  ~RGBpixelmatrix(); //deconstructor
  RGB getColor(uint8_t column, uint8_t line);
  void setColor(uint8_t column, uint8_t line, RGB color);
  uint8_t getWidth(void); //returns _width
  uint8_t getHeight(void); //returns _height
  void sendColors(void); //sends all data in buffer. function is blocking and does not allow any interruption (interrupts are disabled)
 uint8_t* _colorarray; //array that contains pixel information that can be directly sent out to led matrix without conversion
 void  clear(void); //clear all pixels
void  setByte(uint8_t index,  uint8_t data); //write data directly in buffer
uint8_t  getByte(uint8_t index);
   RGB HSVtoRGB(float H, float S, float V);
private:
  void sendByte(uint8_t data); //helper function to decode a byte and clock it out
  uint8_t _width; //width of matrix (number of columns)
  uint8_t _height; //height of matrix (number of lines)

};

#endif



