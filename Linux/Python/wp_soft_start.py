# -*- coding: utf-8 -*-

import time
from sglx_pkg import sglx

# Edit the server address/port here
sglx_addr = "localhost"
sglx_port = 4142


# Plays wave 'jwave' at OneBox channel AO-0.
# Playback is triggered by software command.
#
def wp_soft_start():

    # Handle to SpikeGLX
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        # For demo purposes we assume the OneBox is not recording...
        # So we refer to it using its slot index
        ip = -1
        slot = 21

        # Load the wave plan
        wave_name = 'jwave'
        ok = sglx.c_sglx_obx_wave_load( hSglx, ip, slot, wave_name.encode() )

        if ok:
            # Select software triggering and infinite looping
            trigger = -2
            looping = True
            ok = sglx.c_sglx_obx_wave_arm( hSglx, ip, slot, trigger, looping )

            if ok:
                # Start playback now, output is always at AO-0
                start = True
                ok = sglx.c_sglx_obx_wave_startstop( hSglx, ip, slot, start )

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
                    ok = sglx.c_sglx_obx_wave_startstop( hSglx, ip, slot, start )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


if __name__ == "__main__":
    wp_soft_start()


