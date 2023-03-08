/*********************************************************************
 File:     chromakey.cpp

 Author:   Amman Chuhan

 Email address: chuhan@usc.edu

 Usage: program_name in.bmp background.bmp dist_threshold out1.bmp out2.bmp

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

 Method 1 Utilize a user-defined distance threshold from the
          chromakey value as a discriminator

 Method 2 Devise a method that to determine the chromakey mask
          that doesn't require a user-input threshold

********************************************************************/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

// Prototypes
// IMPORTANT: you must exactly use these input types, function names, and 
// return types. Otherwise the grader can't test them.
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold);

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE]);

void replace(bool mask[][SIZE],
	     unsigned char inImage[][SIZE][RGB],
	     unsigned char bgImage[][SIZE][RGB],
	     unsigned char outImage[][SIZE][RGB]);

int main(int argc, char *argv[])
{
  // Image data array
  // Note:  DON'T use the static keyword except where we expressly say so.
  //        It puts the large array in a separate, fixed, area of memory. 
  //        It is bad practice. But useful until we have dynamic allocation.
  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }

  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  
  // Write code to convert the threshold (argv[3])
  // from string format to a double and assign the 'threshold'

  threshold = atof(argv[3]);

  // Call Method 1 Function

  method1(inputImage, chromaMask, threshold);

  // Produce the output by calling replace()

  replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[4]
  if (writeRGBBMP(argv[4], outputImage)) {
    cout << "Error writing file: " << argv[4] << endl;
    exit(1);
  }	

  // Call Method 2 Function

  method2(inputImage, chromaMask);

  // Produce the output by calling replace()

  replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }	

  return 0;
}



// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold)
{

  int avgRedValue;
  int avgGreenValue;
  int avgBlueValue;
  int sumRedRow0 = 0;
  int sumRedColumn0 = 0;
  int sumGreenRow0 = 0;
  int sumGreenColumn0 = 0;
  int sumBlueRow0 = 0;
  int sumBlueColumn0 = 0;

  // first we iterate across row 0 and determine its averages

  for (int i = 0; i < SIZE; i++){ 
    sumRedRow0 += inImage[0][i][0]; // sum up red values across row 0;
    sumGreenRow0 += inImage[0][i][1]; // sum up green values across row 0;
    sumBlueRow0 += inImage[0][i][2]; // sum up blue values across row 0;
  }

  avgRedValue = sumRedRow0/SIZE; // avg red value across row 0;
  avgGreenValue = sumGreenRow0/SIZE; // avg green value across row 0;
  avgBlueValue = sumBlueRow0/SIZE; // avg blue value across row 0;

  // next, we iterate down column 0 and update the average values;

  for (int i = 0; i < SIZE; i++){ 
    sumRedColumn0 += inImage[i][0][0]; // sum up red values down column 0;
    sumGreenColumn0 += inImage[i][0][1]; // sum up green values down column 0;
    sumBlueColumn0 += inImage[i][0][2]; // sum up blue values down column 0;
  }

  avgRedValue = ((sumRedColumn0/SIZE) + avgRedValue)/2;
  avgGreenValue = ((sumGreenColumn0/SIZE) + avgGreenValue)/2;
  avgBlueValue = ((sumBlueColumn0/SIZE) + avgBlueValue)/2;

  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){ // use nested loop to iterate through all pixels
      
      double distance; // distance from avg RGB value

      double redDistance = pow(avgRedValue - inImage[i][j][0], 2.0);
      double greenDistance = pow(avgGreenValue - inImage[i][j][1], 2.0);
      double blueDistance = pow(avgBlueValue - inImage[i][j][2], 2.0);

      distance = sqrt(redDistance + greenDistance + blueDistance);

      if (distance <= threshold){
        mask[i][j] = 0; // chromakey for pixels that are close enough to avg values
      }
      else {
        mask[i][j] = 1; // foreground for pixels that are not close enough to avg values
      }
    }
  }
}

// Devise a method to automatically come up with a threshold
//  for the chroma key determination
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{

  int avgRedValue;
  int avgGreenValue;
  int avgBlueValue;

  int sumRedRows = 0;
  int sumRedColumns = 0;
  int sumGreenRows = 0;
  int sumGreenColumns = 0;
  int sumBlueRows = 0;
  int sumBlueColumns = 0;

  // first we iterate across rows 254 and 255 and determine its averages

  for (int i = 0; i < SIZE; i++){ 
    sumRedRows += inImage[254][i][0] + inImage[255][i][0]; // sum up red values across rows 253-255

    sumGreenRows += inImage[254][i][1] + inImage[255][i][1]; // sum up green values across rows 253-255

    sumBlueRows += inImage[254][i][2] + inImage[255][i][2]; // sum up blue values across rows 253-255
  }

  avgRedValue = sumRedRows/(2 * SIZE); // avg red value across rows 253, 254 and 255

  avgGreenValue = sumGreenRows/(2 * SIZE); // avg green value across rows 253, 254 and 255

  avgBlueValue = sumBlueRows/(2 * SIZE); // avg blue value across rows 253, 254 and 255

  // next, we iterate down columns 0, 1 and 2 and update the average values

  for (int i = 0; i < SIZE; i++){ 
    sumRedColumns += inImage[i][0][0] + inImage[i][1][0]; // sum up red values down columns 0-4

    sumGreenColumns += inImage[i][0][1] + inImage[i][1][1]; // sum up green values down columns 0-4

    sumBlueColumns += inImage[i][0][2] + inImage[i][1][2]; // sum up blue values down columns 0-4
  }

  // update avg values to consider data from columns

  avgRedValue = ((sumRedColumns/(2 * SIZE)) + avgRedValue)/2; 

  avgGreenValue = ((sumGreenColumns/(2 * SIZE)) + avgGreenValue)/2;

  avgBlueValue = ((sumBlueColumns/(2 * SIZE)) + avgBlueValue)/2;

  double distance; // distance from avg RGB value
  double maxDistance = 0; // will be used as threshold value

  // we will now iterate through first few rows to find what the max distance a pixel has 
  // from the average RGB pixel is

  for (int i = 0; i < 1; i++){ 
    for (int j = 0; j < SIZE; j++){

      double redDistance = pow(avgRedValue - inImage[i][j][0], 2.0);
      double greenDistance = pow(avgGreenValue - inImage[i][j][1], 2.0);
      double blueDistance = pow(avgBlueValue - inImage[i][j][2], 2.0);

      distance = sqrt(redDistance + greenDistance + blueDistance);

      if (distance >= maxDistance){
        maxDistance = distance;
      }
    }
  }

  // now we will iterate through again, using max distance as a threshold

  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){

      double redDistance = pow(avgRedValue - inImage[i][j][0], 2.0);
      double greenDistance = pow(avgGreenValue - inImage[i][j][1], 2.0);
      double blueDistance = pow(avgBlueValue - inImage[i][j][2], 2.0);

      distance = sqrt(redDistance + greenDistance + blueDistance);

      // cout << maxDistance;

      if (distance <= (maxDistance)){ //
        mask[i][j] = 0; // chromakey for pixels that are close enough to avg values
      }
      else {
        mask[i][j] = 1; // foreground for pixels that are not close enough to avg values
      }
    }
  }
}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{
  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){
      for (int k = 0; k < RGB; k++){
        if (mask[i][j] == 0){ // if it is 0, the bg pixel is selected
          outImage[i][j][k] = bgImage[i][j][k];
        }
        else { // if it is 1 (the only other possibility) the input image pixel is selected
          outImage[i][j][k] = inImage[i][j][k];
        }
      }
    }
  }
}
