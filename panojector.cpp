#include "plugin.hpp"
#include <iostream>
#include <dlfcn.h>

using namespace std;


IplImage* scan( int size, Projector& projector )
{
  IplImage* dst = cvCreateImage( cvSize(size, size), IPL_DEPTH_8U, 3 );
  for(int y=0;y<size;y++){
    for(int x=0;x<size;x++){
      float rx = (float)x*2 / size - 1.0;
      float ry = (float)y*2 / size - 1.0;
      uchar* pixel = projector.map(rx,ry);
      if ( pixel ){
	for ( int ch=0; ch<3; ch++ ){
	  dst->imageData[dst->widthStep * y + x * 3 + ch] = pixel[ch];
	}
      }
      else{
	for ( int ch=0; ch<3; ch++ ){
	  dst->imageData[dst->widthStep * y + x * 3 + ch] = 255;
	}
      }
    }
  }
  return dst;
}





void usage( int argc, char* argv[] )
{
    fprintf( stderr, "Usage: %s [-s x][-o outputfilename] [projectors]\n", argv[0] );
    fprintf( stderr, "Options:\n" );
    fprintf( stderr, "\t-s 400\tPicture size.\n" ); 
    fprintf( stderr, "\t-o pano.jpg\tFilename to be output.\n" ); 
    exit(1);
}

//first arg: origin file
//second: projection plugins


int main( int argc, char* argv[] ){
  int width=400;
  char dstfilename[1000];
  strcpy( dstfilename, "pano.jpg" );
  
  int c = 1;
  while ( c < argc ){
    if ( 0 == strcmp( argv[c], "-s" )){
      c++;
      width = atoi( argv[c] );
      c++;
    }
    if ( 0 == strcmp( argv[c], "-o" )){
      c++;
      strcpy( dstfilename, argv[c] );
      c++;
    }
    else if ( argv[c][0] == '-' ){
      usage(argc, argv);
    }
    else{
      break;
    }
  }
  argv += c;
  argc -= c;
  Projector* projector = plugin_load( argc, argv );
  IplImage* dst = scan( width, *projector );
  cvSaveImage( dstfilename, dst );
  cvReleaseImage (&dst);
}
