# -*- coding: utf-8 -*-

import time
from sglx_pkg import sglx

# Edit the server address/port here
sglx_addr = "localhost"
sglx_port = 4142


# Plays wave 'jwave' at NI channel AO-0.
# Playback is triggered by software command.
#
def wp_ni_soft_start():

    # Handle to SpikeGLX
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        # Load the wave plan, select infinite looping
        wave_name = 'jwave'
        outChan   = 'PXI1Slot6_2/ao0'
        looping   = True
        ok = sglx.c_sglx_ni_wave_load( hSglx, outChan.encode(), wave_name.encode(), looping )

        if ok:
            # Select software triggering
            trigTerm = 'software'
            ok = sglx.c_sglx_ni_wave_arm( hSglx, outChan.encode(), trigTerm.encode() )

            if ok:
                # Start playback now
                start = True
                ok = sglx.c_sglx_ni_wave_startstop( hSglx, outChan.encode(), start )

                if ok:
                    # This section demonstrates a method to capture your
                    # wave plan in action. The best sleep parameters will
                    # depend upon the duration of your wave plan and how
                    # fast your SpikeGLX graphs are sweeping
                    #
                    # Let this play for 1 second
                    # Then sleep the SpikeGLX Graphs Window for 2 seconds
                    # Then resume Graphs for 5 seconds
                    time.sleep( 1.0 )
                    sglx.c_sglx_pause_graphs( hSglx, True )
                    time.sleep( 2.0 )
                    sglx.c_sglx_pause_graphs( hSglx, False )
                    time.sleep( 5.0 )

                    # Stop playback
                    start = False
                    ok = sglx.c_sglx_ni_wave_startstop( hSglx, outChan.encode(), start )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


if __name__ == "__main__":
    wp_ni_soft_start()


