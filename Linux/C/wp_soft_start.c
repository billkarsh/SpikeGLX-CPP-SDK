
//#include "stdafx.h" // enable if using Visual Studio precompiled headers

#include "C_SglxApi.h"
#include <stdio.h>
#include <unistd.h>

// Edit the server address/port here
static const char*  addr = "localhost";
static int          port = 4142;




// Plays wave 'jwave' at OneBox channel AO-0.
// Playback is triggered by software command.
//
void wp_soft_start()
{
    // Handle to SpikeGLX
    void    *hSglx = c_sglx_createHandle();

    if( c_sglx_connect( hSglx, addr, port ) ) {

        // For demo purposes we assume the OneBox is not recording...
        // So we refer to it using its slot index
        int ip      = -1,
            slot    = 21;

        // Load the wave plan
        const char  *wave_name = "jwave";
        if( !c_sglx_obx_wave_load( hSglx, ip, slot, wave_name ) )
            goto error;

        // Select software triggering and infinite looping
        int     trigger = -2;
        bool    looping = true;
        if( !c_sglx_obx_wave_arm( hSglx, ip, slot, trigger, looping ) )
            goto error;

        // Start playback now, output is always at AO-0
        bool    start = true;
        if( !c_sglx_obx_wave_startstop( hSglx, ip, slot, start ) )
            goto error;

        // This section demonstrates a method to capture your
        // wave plan in action. The best sleep parameters will
        // depend upon the duration of your wave plan and how
        // fast your SpikeGLX graphs are sweeping
        //
        // Let this play for 1 second
        // Then sleep the SpikeGLX Graphs Window for 2 seconds
        // Then resume Graphs for 5 seconds
        usleep( 1000000 );
        c_sglx_pause_graphs( hSglx, true );
        usleep( 2000000 );
        c_sglx_pause_graphs( hSglx, false );
        usleep( 5000000 );

        // Stop playback
        start = false;
        if( !c_sglx_obx_wave_startstop( hSglx, ip, slot, start ) )
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


