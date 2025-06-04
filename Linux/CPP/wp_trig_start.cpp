
//#include "stdafx.h" // enable if using Visual Studio precompiled headers

#include "SglxCppClient.h"
#include <unistd.h>

// Edit the server address/port here
static const char*  addr = "localhost";
static int          port = 4142;




// Plays wave 'jwave' at OneBox channel AO-0.
// Playback is triggered when OneBox channel AI-1 goes high.
// User needs to list AI-1 in the XA box of OBX Setup tab.
// We will configure NI device to send TTL rising edge from line-4.
// User needs to wire NI line-4 to OneBox AI-1.
//
void wp_trig_start()
{
    // Handle to SpikeGLX
    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx, addr, port ) ) {

        // OneBox assumed to be recording stream ip=0...
        // So the slot number is ignored in this case
        int ip      = 0,
            slot    = -1;

        // Load the wave plan
        const char  *wave_name = "jwave";
        if( !sglx_obx_wave_load( hSglx, ip, slot, wave_name ) )
            goto error;

        // Select AI-1 triggering and no looping
        int     trigger = 1;
        bool    looping = false;
        if( !sglx_obx_wave_arm( hSglx, ip, slot, trigger, looping ) )
            goto error;

        // Configure NI line-4
        const char      *digOutTerm = "/PXI1Slot6_2/line4";
        unsigned int    digBits     = 0x10; // binary 1 at bit-4, zero elsewhere
        if( !sglx_ni_DO_set( hSglx, digOutTerm, 0 ) )
            goto error;

        // Start playback now, output is always at AO-0
        if( !sglx_ni_DO_set( hSglx, digOutTerm, digBits ) )
            goto error;

        // Reset trigger after 50 ms
        usleep( 50000 );
        if( !sglx_ni_DO_set( hSglx, digOutTerm, 0 ) )
            goto error;

        // This section demonstrates a method to capture your
        // wave plan in action. The best sleep parameters will
        // depend upon the duration of your wave plan and how
        // fast your SpikeGLX graphs are sweeping
        //
        // Let this play for 1 second
        // Then sleep the SpikeGLX Graphs Window for 2 seconds
        // Then resume Graphs
        usleep( 1000000 );
        sglx_pause_graphs( hSglx, true );
        usleep( 2000000 );
        sglx_pause_graphs( hSglx, false );

        // Stop playback
        bool    start = false;
        if( !sglx_obx_wave_startstop( hSglx, ip, slot, start ) )
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


