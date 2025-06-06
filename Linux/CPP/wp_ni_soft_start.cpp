
#include "SglxCppClient.h"
#include <unistd.h>

// Edit the server address/port here
static const char*  addr = "localhost";
static int          port = 4142;




// Plays wave 'jwave' at NI channel AO-0.
// Playback is triggered by software command.
//
void wp_ni_soft_start()
{
    // Handle to SpikeGLX
    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx, addr, port ) ) {

        // Load the wave plan, select infinite looping
        const char  *wave_name = "jwave";
        const char  *outChan   = "PXI1Slot6_2/ao0";
        bool        looping    = true;
        if( !sglx_ni_wave_load( hSglx, outChan, wave_name, looping ) )
            goto error;

        // Select software triggering
        const char  *trigTerm = "software";
        if( !sglx_ni_wave_arm( hSglx, outChan, trigTerm ) )
            goto error;

        // Start playback now
        bool    start = true;
        if( !sglx_ni_wave_startstop( hSglx, outChan, start ) )
            goto error;

        // This section demonstrates a method to capture your
        // wave plan in action. The best pause parameters will
        // depend upon the duration of your wave plan and how
        // fast your SpikeGLX graphs are sweeping
        //
        // Let this play for 1 second
        // Then pause the SpikeGLX Graphs Window for 2 seconds
        // Then resume Graphs for 5 seconds
        usleep( 1000000 );
        sglx_pause_graphs( hSglx, true );
        usleep( 2000000 );
        sglx_pause_graphs( hSglx, false );
        usleep( 5000000 );

        // Stop playback
        start = false;
        if( !sglx_ni_wave_startstop( hSglx, outChan, start ) )
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


