# -*- coding: utf-8 -*-

from sglx_pkg import sglx as sglx
from ctypes import byref, POINTER, c_double, c_int, c_short, c_bool, c_char_p

# Edit the server address/port here
sglx_addr = "localhost"
sglx_port = 4142


def justConnect():
    print( "\nCalling connect...\n\n" )
    hSglx = sglx.c_sglx_createHandle()

    # Using default loopback address and port
    if sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port ):
        print( "version <{}>\n".format( sglx.c_sglx_getVersion( hSglx ) ) )
    else:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# Simple query.
#
def console_test():
    print( "\nConsole test...\n\n" )
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        hid = c_bool()
        ok  = sglx.c_sglx_isConsoleHidden( byref(hid), hSglx )
        if ok:
            print( "Console hidden: {}\n".format( bool(hid) ) )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# SHA1.
#
def verify_test():
    print( "\nVerify test...\n\n" )
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        filename = "D:\\AAA\\TT_g0\\TT_g0_imec0\\TT_g0_t0.imec0.ap.bin"
        ok = sglx.c_sglx_verifySha1( sglx.sglx_demo_callback, hSglx, filename.encode() )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# Par2.
#
def par2_test():
    print( "\nPar2 test...\n\n" )
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        filename = "D:\\AAA\\TT_g0\\TT_g0_imec0\\TT_g0_t0.imec0.ap.bin"
        ok = sglx.c_sglx_par2( sglx.sglx_demo_callback, hSglx, ord('v'), filename.encode() )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# Query.
#
def getParams_test():
    print( "\nGet params...\n\n" )
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        nval = c_int()
        len  = c_int()
        ok   = sglx.c_sglx_getParams( byref(nval), hSglx )
        if ok:
            kv = {}
            for i in range(0, nval.value):
                line = sglx.c_sglx_getstr( byref(len), hSglx, i ).decode()
                parts = line.split( '=' )
                kv.update( {parts[0]: parts[1]} )
            print( "{}".format( kv.items() ) )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# Structure.
#
def getShankMap_test():
    print( "\nGet shank map...\n\n" )
    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        nval = c_int()
        len  = c_int()
        ok   = sglx.c_sglx_getNIShankMap( byref(nval), hSglx )
        if ok:
            for i in range(0, nval.value):
                print( "{}".format( sglx.c_sglx_getstr( byref(len), hSglx, i ) ) )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# Set the user notes for the next run; read that back.
#
def set_get_userNotes_test():
    print( "\nSet, get notes...\n\n" )

    hSglx = sglx.c_sglx_createHandle()
    ok = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        # Make sure we also set a new run name.
        run = c_char_p()
        ok  = sglx.c_sglx_getRunName( byref(run), hSglx )

        if ok:
            sglx.c_sglx_setkv( hSglx, "".encode(), "".encode() )
            sglx.c_sglx_setkv( hSglx, "snsNotes".encode(), "Test setting of run comment field.".encode() )
            sglx.c_sglx_setkv( hSglx, "snsRunName".encode(), (run.value.decode() + "_v2").encode() )

            ok = sglx.c_sglx_setKVParams( hSglx )

            if ok:
                nval = c_int()
                len  = c_int()
                ok   = sglx.c_sglx_getParams( byref(nval), hSglx )

                if ok:
                    for i in range(0, nval.value):
                        s = sglx.c_sglx_getstr( byref(len), hSglx, i ).decode()
                        if s.startswith( "snsNotes" ):
                            print( "{}\n".format( s ) )
                            break

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# Fetch and plot latest 1 second of NI channel AI0.
# Run SpikeGLX_NISIM.exe which generates a 1 Hz sine wave on AI0.
#
def plot_NI_1sec_test():
    print( "\nPlot NI AI0...\n\n" )

    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        # NI stream is (js,ip) = (0,0)
        srate = sglx.c_sglx_getStreamSampleRate( hSglx, 0, 0 )
        print( "Sample rate {}\n".format( srate ) )

        if srate > 0:
            data     = POINTER(c_short)()
            ndata    = c_int()
            py_chans = [0]
            nC       = len(py_chans)
            channels = (c_int * nC)(*py_chans)

            headCt = sglx.c_sglx_fetchLatest( byref(data), byref(ndata), hSglx, 0, 0, int(srate), channels, nC, 1 )

            if headCt > 0:
                nt = int(ndata.value / nC)
                print( "Head count {}, samples {}\n".format( headCt, nt ) )

                for it in range(0, nt, 250):
                    spc = int(40 + (80 * data[it*nC]) / 32768)
                    s   = ""
                    for i in range(0, spc):
                        s += ' '
                    s += '*'
                    print( f"{s}" )

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


# Continuously fetch data from imec probe-0.
# Threshold selected meas_chan.
# Send digital out command tracking threshold crossings.
#
# Runs until error or Ctrl-C.
#
# To measure closed-loop latency, immerse the probe in saline
# and give it a square wave signal (1 to 5 mV p-p, 10 Hz).
#
# We fetch 384 channels of these data in the remote program;
# fetching performance is similar regardless of channel count.
#
# If the probe has an LF-band we'll use that so the 10 Hz signal
# isn't too distorted, otherwise we'll fetch AP (full-band).
#
# We analyze one of these channels looking for a rising edge.
# We then react to that threshold crossing by commanding an
# NI device to make another edge that is sent to the probe's
# SMA connector as a digital input. The separation between
# the threshold event and the resulting NI event gives a
# direct readout of closed-loop latency. We measure the mean
# closed-loop latency to be 2 to 3 ms.
#
# Note that the square wave amplitude and/or thresh voltage may
# need to be adjusted until you are seeing a regular square
# wave in the imec SY channel; not many edges missed, and not
# too noisy.
#
# While running this script you can enable file writing in SpikeGLX,
# and save a few minutes of recording. Get SpikeGLX_Datafile_Tools
# from the SpikeGLX download site and use its latency_test_analysis
# script to calculate latency statistics.
# https://billkarsh.github.io/SpikeGLX/#post-processing-tools
#
def latency_test():
    print( "\nLatency test...\n\n" )

    hSglx = sglx.c_sglx_createHandle()
    ok    = sglx.c_sglx_connect( hSglx, sglx_addr.encode(), sglx_port )

    if ok:
        line   = "Dev4/port0/line5"
        bits   = 0

        js = 2
        ip = 0

        # Get [AP LF SY] channel counts for probe stream
        ntypes = c_int()
        ok = sglx.c_sglx_getStreamAcqChans( byref(ntypes), hSglx, js, ip )
        nLF = sglx.c_sglx_getint( hSglx, 1 )

        if nLF > 0:
            fetch_chans = [i for i in range(384, 768)]
            meas_chan = 393             # arb chan in fetched set
            meas_idx = meas_chan - 384  # index into fetched set
        else:
            fetch_chans = [i for i in range(0, 384)]
            meas_chan = 9               # arb chan in fetched set
            meas_idx = meas_chan - 0    # index into fetched set

        data     = POINTER(c_short)()
        ndata    = c_int()
        nC       = len(fetch_chans)
        channels = (c_int * nC)(*fetch_chans)

        i162V = c_double()
        ok = sglx.c_sglx_getStreamI16ToVolts( byref(i162V), hSglx, js, ip, meas_chan )
        thresh = 0.00025 / i162V.value  # 0.25 mv as probe i16

        print( "Threshold {}\n".format( thresh ) )

        fromCt = sglx.c_sglx_getStreamSampleCount( hSglx, js, ip )

        if fromCt > 0:
            ok = sglx.c_sglx_ni_DO_set( hSglx, line.encode(), bits )

            if ok:
                while True:
                    headCt = sglx.c_sglx_fetch( byref(data), byref(ndata), hSglx, js, ip, fromCt, 120, channels, nC, 1 )

                    if headCt == 0:
                        break

                    tpts = int(ndata.value / nC)

                    if tpts > 1:
                        diff  = data[meas_idx + (tpts-1)*nC] - data[meas_idx]
                        digOK = True

                        if diff > thresh and bits == 0:
                            bits  = 0xFF
                            digOK = sglx.c_sglx_ni_DO_set( hSglx, line.encode(), bits )
                        elif diff < -thresh and bits == 0xFF:
                            bits  = 0
                            digOK = sglx.c_sglx_ni_DO_set( hSglx, line.encode(), bits )

                        if not digOK:
                            break

                        fromCt = headCt + tpts

    if not ok:
        print( "error [{}]\n".format( sglx.c_sglx_getError( hSglx ) ) )

    sglx.c_sglx_close( hSglx )
    sglx.c_sglx_destroyHandle( hSglx )


if __name__ == "__main__":
    justConnect()


