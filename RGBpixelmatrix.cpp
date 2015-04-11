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
  RGBpixelmatrix.cpp - A library to handle WS2811 pixel based
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
 
#include "Arduino.h"
#include "RGBpixelmatrix.h"


//constructor, deconstructor

 RGBpixelmatrix::RGBpixelmatrix(uint8_t width, uint8_t height)
{
  _width = width; //each pixel has 3 bytes (24bits per pixel)representing it this way makes calculations faster in the code
  //as this multiplication would have to be done for each read or write to a pixel
  _height = height*3; //height (=number of lines)
  
  //get memory reserved for our array using malloc()
  void * newarray = malloc ((uint16_t)_width*_height);
	_colorarray = (uint8_t*) newarray;
  DDRD |= 0b10000000 ;//pin7 as output
}

RGBpixelmatrix::~RGBpixelmatrix()
{
 if (_colorarray!=0)
        {
                free(_colorarray);
        }
}




/*
  Note on pixel order:
 The array containing the pixel information is meant to be sent out directly (shift out) without
 the need to re-organize pixels to make it as fast as possible during send.
 Assumptions:
 -input of pixels is on bottom left of matrix
 -The input connects to pixel 0/0 (=1st quadrant of cartesian coordinate system)
 -The first elements of the array represent this pixel. It is therefore sent out first
 -The strips are the columns, first strip on the left starts from bottom and goes to top, second one 
  from top and goes to bottom, like a meandering snake.
 
 The pixels (WS2811) work this way: pixel receives three bytes. After filling the buffer it connects
 the input to the output. The first pixel sent out is 0/0. it is not a shift register like operation!
 
 Each pixel wants the data in GRB not RGB, so data is stored that way.
 Also the orientation changes after each column. All even column are forward represented, all uneven columns
 are reversed. Only pixels order is reversed, each pixel still is in GRB.
 example:
 Actual Matrix        Array (each cell is in GRB)
 
 2|3|8|9 -> line 2          
 1|4|4|10 -> line 1         
 0|5|6|11 -> line 0
 
 c
 o
 l
 u
 m
 n
 0
 
 now the array would be sent out starting from index 0 (first pixel). example is a 4x3 matrix, hence 12*3 bytes
 for the array. sending will be from 0...35 sending pixel0, pixel1, pixel2, pixel3,pixel4 and so on
 */


RGB RGBpixelmatrix::getColor(uint8_t column,  uint8_t line) //get pixel color in RGB 
{
  uint16_t columnindex = column*_height; //index of first pixel at this column (not reversed!)
  uint8_t lineoffset;
  RGB returncolor;
  
  if(column%2) //uneven columns are pixel-reversed, color order is BRG
  {
    lineoffset = _height-line*3-3; //offset from first pixel in this column (each pixel has 24bit!) this column is reversed
  }
  else //even columns are not pixel-reversed, color order is BRG
  {
   lineoffset = line*3; //offset from first pixel in this column (each pixel has 24bit!)
  }
  
  returncolor.g = _colorarray[columnindex+lineoffset];    
  returncolor.r = _colorarray[columnindex+lineoffset+1];
  returncolor.b = _colorarray[columnindex+lineoffset+2];
  return returncolor;
}


  uint8_t RGBpixelmatrix::getWidth(void) //returns width
  {
  return _width/3;
  }
  
  uint8_t RGBpixelmatrix::getHeight(void) //returns height
  {
  return _height;
  }

void  RGBpixelmatrix::setColor(uint8_t column,  uint8_t line, RGB color) //set pixel color in RGB
{
  uint16_t columnindex = column*_height; //index of first pixel at this column (not reversed!)
  uint8_t lineoffset = line*3;

  if(column%2) //uneven lines are  pixel-reversed, color order is GRB
  {
    lineoffset = _height-lineoffset-3; //offset from first pixel in this line (each pixel has 24bit!) this line is reversed
  }

  _colorarray[columnindex+lineoffset]   = color.g;    
  _colorarray[columnindex+lineoffset+1] = color.r;
  _colorarray[columnindex+lineoffset+2] = color.b;
}

void  RGBpixelmatrix::clear(void) //clear all pixels
{
	memset(_colorarray, 0, _width*_height);
}

void  RGBpixelmatrix::setByte(uint8_t index,  uint8_t data) //write data directly in buffer
{
 
  _colorarray[index]   = data;    


}

uint8_t  RGBpixelmatrix::getByte(uint8_t index) //returns data directly from buffer
{
 
  return _colorarray[index];    


}

 RGB RGBpixelmatrix::HSVtoRGB(float H, float S, float V)
 {
 
  float s=(float)S/255.0; //auf 1 normieren
  float v=(float)V/255.0; //auf 1 normieren
  RGB result; 		
  int i;
  float f, p, q, t;
  if(s == 0 ) //zero saturation, return gray level
  {
    result.r = round(255*v);
    result.g = round(255*v);
    result.b = round(255*v);
    return result;
  }

  i = (int)((float)H/42.5) %6;
  f = (float)H/42.5 - (int)((float)H/42.5);		
  p = v * ( 1.0 - s );
  q = v * ( 1.0 - (s * f));
  t = v * ( 1.0 - (s * ( 1.0 - f )));


  switch( i )
  {
  case 0:
    result.r = round(255*v); 
    result.g = round(255*t); 
    result.b = round(255*p);
    break;

  case 1:
    result.r = round(255*q); 
    result.g = round(255*v); 
    result.b = round(255*p); 
    break;

  case 2:
    result.r = round(255*p); 
    result.g = round(255*v); 
    result.b = round(255*t); 

    break;
    
  case 3:
    result.r = round(255*p); 
    result.g = round(255*q); 
    result.b = round(255*v); 
    break;			
    
  case 4:
    result.r = round(255*t); 
    result.g = round(255*p); 
    result.b = round(255*v); 
    break;
    
  default:								
    result.r = round(255*v); 
    result.g = round(255*p); 
    result.b = round(255*q); 
    break;								

  }

return result;

 }

inline void RGBpixelmatrix::sendByte(uint8_t data)
{
  uint8_t bitmask = 0b10000000;

  for(uint8_t cnt = 8; cnt; cnt--)
  { 
    if(data & bitmask)  //write a "1"
    {   
      PORTD |= 0b10000000; //pin7 high
      asm volatile ("nop");
      asm volatile ("nop");
      asm volatile ("nop");
      asm volatile ("nop");
      asm volatile ("nop");
      asm volatile ("nop");
      asm volatile ("nop");
	    asm volatile ("nop");
		 asm volatile ("nop");
		  asm volatile ("nop");
		      asm volatile ("nop");
    asm volatile ("nop");
		 asm volatile ("nop");
		  asm volatile ("nop");
		      asm volatile ("nop");
      PORTD &= 0b01111111; //pin7 low

    }
    else  //write a "0"
    {
      PORTD |= 0b10000000; //pin7 high
      asm volatile ("nop");
	  asm volatile ("nop");
	   asm volatile ("nop");
	       asm volatile ("nop");
		 asm volatile ("nop");
		  
      PORTD &= 0b01111111; //pin7 low

    }
    bitmask = bitmask>>1;

  }
}

void  RGBpixelmatrix::sendColors(void) //function is blocking until send is done, also blocks interrupts
{
   
  cli();//globally disable interrupts, we do not want to be disturbed or timing will not work
  
  for( int16_t index = 0; index<(int16_t)_width * _height; index++)
  { 
    sendByte(_colorarray[index]);
  }
   // sendByte(_colorarray[0]); //index = 0 was not sent because the while loop
  sei();
  //_delay_us(50); //delay is not necessary, it can be assumed the function is not called in very fast succession

}

