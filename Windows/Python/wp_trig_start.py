# -*- coding: utf-8 -*-

import time
from sglx_pkg import sglx

# Edit the server address/port here
sglx_addr = "localhost"
sglx_port = 4142


# Plays wave 'jwave' at OneBox channel AO-0.
# Playback is triggered when OneBox channel AI-1 goes high.
# User needs to list AI-1 in the XA box of OBX Setup tab.
# We will configure NI device to send TTL rising edge from line-4.
# User needs to wire NI line-4 to OneBox AI-1.
#
def wp_trig_start():

    # Handle to SpikeGLX
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        # OneBox assumed to be recording stream ip=0...
        # So the slot number is ignored in this case
        ip = 0
        slot = -1

        # Load the wave plan
        wave_name = 'jwave'
        ok = sglx.c_sglx_obx_wave_load( hSglx, ip, slot, wave_name.encode() )

        if ok:
            # Select AI-1 triggering and no looping
            trigger = 1
            looping = False
            ok = sglx.c_sglx_obx_wave_arm( hSglx, ip, slot, trigger, looping )

            if ok:
                # Configure NI line-4
                digOutTerm = '/PXI1Slot6_2/line4'
                digBits = 0x10 # binary 1 at bit-4, zero elsewhere
                ok = sglx.c_sglx_ni_DO_set( hSglx, digOutTerm.encode(), 0 )

                if ok:
                    # Start playback now, output is always at AO-0
                    ok = sglx.c_sglx_ni_DO_set( hSglx, digOutTerm.encode(), digBits )

                    if ok:
                        # Reset trigger after 50 ms
                        time.sleep( 0.05 )
                        ok = sglx.c_sglx_ni_DO_set( hSglx, digOutTerm.encode(), 0 )

                        if ok:
                            # This section demonstrates a method to capture your
                            # wave plan in action. The best sleep parameters will
                            # depend upon the duration of your wave plan and how
                            # fast your SpikeGLX graphs are sweeping
                            #
                            # Let this play for 1 second
                            # Then sleep the SpikeGLX Graphs Window for 2 seconds
                            # Then resume Graphs
                            time.sleep( 1.0 )
                            sglx.c_sglx_pause_graphs( hSglx, True )
                            time.sleep( 2.0 )
                            sglx.c_sglx_pause_graphs( hSglx, False )

                            # Stop playback
                            start = False
                            ok = sglx.c_sglx_obx_wave_startstop( hSglx, ip, slot, start )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


if __name__ == "__main__":
    wp_trig_start()


