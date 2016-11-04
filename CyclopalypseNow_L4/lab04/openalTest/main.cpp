/*
 *  CSCI 441 / 598B, Computer Graphics, Fall 2016
 *
 *  Project: helloWorld - OpenAL
 *  File: hello.cpp
 *
 *  Description:
 *		The first program written for any language or library is "Hello World".  What
 *		better way for a tutorial on sound than for the program to speak "Hello World"!
 *		This program does just that, setting up a single source at the same position as
 *		our listener and allows the user the start/stop/pause the playback.
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *  
 *  Notes:
 *		Based off of MindCode OpenAL Lesson 1
 *
 */

#ifdef __APPLE__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
	#include <ALUT/alut.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
	#include <AL/alut.h>
#endif

#include <stdlib.h>
#include <stdio.h>

/*
 * These are OpenAL "names" (or "objects"). They store an id of a buffer
 * or a source object. Generally you would expect to see the implementation
 * use values that scale up from '1', but don't count on it. The spec does
 * not make this mandatory (as it is OpenGL). The id's can easily be memory
 * pointers as well. It will depend on the implementation.
 */
// Buffers to hold sound data.
ALuint Buffer;

// Sources are points of emitting sound.
ALuint Source;

/*
 * These are 3D cartesian vector coordinates. A structure or class would be
 * a more flexible way for handling these, but for the sake of simplicity we 
 * will just leave it as is.
 */
// Position of the source sound.
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };

// Velocity of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };


// Position of the Listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the Listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

/*
 * ALboolean LoadALData()
 *
 *	This function will load our sample data from the disk using the Alut
 *	utility and send the data into OpenAL as a buffer. A source is then
 *	also created to play that buffer.
 */
ALboolean LoadALData() {
	// Variables to load into.
	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop = AL_FALSE;

	// Load wav data into a buffer.
	alGenBuffers(1, &Buffer);

	// Check if there was an error creatng a buffer
	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

#ifdef __APPLE__
	// Mac OS X version does not have a looping parameter
	alutLoadWAVFile( (ALbyte*)"HelloWorld.wav", &format, &data, &size, &freq );
#else
	// *nix version does have a looping parameter
	alutLoadWAVFile( (ALbyte*)"HelloWorld.wav", &format, &data, &size, &freq, &loop );
#endif
	
	// load sound data into buffer
	alBufferData(Buffer, format, data, size, freq);
	
	// release WAV file from memory
	alutUnloadWAV(format, data, size, freq);

	// Bind the buffer with the source.
	alGenSources(1, &Source);

	// Check if there was an error loading into the buffer
	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	// set up our source parameters
	alSourcei (Source, AL_BUFFER,   Buffer   );
	alSourcef (Source, AL_PITCH,    1.0      );
	alSourcef (Source, AL_GAIN,     1.0      );
	alSourcefv(Source, AL_POSITION, SourcePos);
	alSourcefv(Source, AL_VELOCITY, SourceVel);
	alSourcei (Source, AL_LOOPING,  loop     );

	// Do another error check and return.
	if(alGetError() == AL_NO_ERROR)
		return AL_TRUE;

	return AL_FALSE;
}

/*
 * void SetListenerValues()
 *
 *	We already defined certain values for the Listener, but we need
 *	to tell OpenAL to use that data. This function does just that.
 */
void SetListenerValues() {
	alListenerfv( AL_POSITION,    ListenerPos );
	alListenerfv( AL_VELOCITY,    ListenerVel );
	alListenerfv( AL_ORIENTATION, ListenerOri );
}

/*
 * void KillALData()
 *
 *	We have allocated memory for our buffers and sources which needs
 *	to be returned to the system. This function frees that memory.
 */
void KillALData() {
	alDeleteBuffers(1, &Buffer);
	alDeleteSources(1, &Source);
	alutExit();
}

int main( int argc, char *argv[] ) {
    printf("MindCode's OpenAL Lesson 1: Single Static Source\n\n");
	printf("Controls:\n");
	printf("p) Play\n");
	printf("s) Stop\n");
	printf("h) Hold (pause)\n");
	printf("q) Quit\n\n");

	// Initialize OpenAL and clear the error bit.
	alutInit(NULL, 0);
	alGetError();

	// Load the wav data.
	if(LoadALData() == AL_FALSE) {
	    printf("Error loading data.");
		return 0;
	}

        printf( "OpenAL Version %s\n", alGetString( AL_VERSION ) );
        printf( "OpenAL Renderer %s\n", alGetString( AL_RENDERER ) );
        printf( "OpenAL Vendor %s\n", alGetString( AL_VENDOR ) );
        printf( "OpenAL Extensions %s\n", alGetString( AL_EXTENSIONS ) );


	// Set up our listener
	SetListenerValues();

	// Setup an exit procedure.
	atexit( KillALData );

	// Loop.
	char c = ' ';

	// Until user quits
	while(c != 'q') {
		// get input from user
		c = getchar();

		switch(c) {
			// Pressing 'p' will begin playing the sample.
			case 'p': alSourcePlay( Source );  break;

			// Pressing 's' will stop the sample from playing.
			case 's': alSourceStop( Source );  break;

			// Pressing 'h' will pause the sample.
			case 'h': alSourcePause( Source ); break;
		};
	}

	return 0;
}
