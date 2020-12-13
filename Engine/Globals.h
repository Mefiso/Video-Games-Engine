#pragma once
#include <windows.h>
#include <stdio.h>

#define _FILE_NAME_ (strrchr("\\" __FILE__, '\\') + 1) // obtains the filename from the _FILE path
#define LOG(format, ...) log(_FILE_NAME_, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Deletes a buffer
#define RELEASE( x ) \
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Configuration -----------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define FULLSCREEN false
#define MAXIMIZED true
#define RESIZABLE true
#define VSYNC true
#define TITLE "TIMEngine"