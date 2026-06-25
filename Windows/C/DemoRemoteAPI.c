// =================================
// Random assortment of remote calls
// =================================

//#include "stdafx.h" // enable if using Visual Studio precompiled headers

#include "C_SglxApi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Edit the server address/port here
static const char*  addr = "localhost";
static int          port = 4142;




void justConnect()
{
    printf( "\nCalling connect...\n\n" );

    void    *hSglx = c_sglx_createHandle();

// Using default loopback address and port

    if( c_sglx_connect( hSglx, addr, port ) )
        printf( "version <%s>\n", c_sglx_getVersion( hSglx ) );
    else
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


// Simple query.
//
void console_test()
{
    printf( "\nConsole test...\n\n" );

    void    *hSglx = c_sglx_createHandle();

    if( c_sglx_connect( hSglx, addr, port ) ) {

        bool    hid;

        if( c_sglx_isConsoleHidden( &hid, hSglx ) )
            printf( "Console hidden: %d\n", hid );
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


static void SGLX_CALL callback( const char *status )
{
    printf( "%s\n", status );
}


// SHA1.
//
void verify_test()
{
    printf( "\nVerify test...\n\n" );

    void    *hSglx = c_sglx_createHandle();

    if( c_sglx_connect( hSglx, addr, port ) ) {

        char *file = "D:\\AAA\\TT_g0\\TT_g0_imec0\\TT_g0_t0.imec0.ap.bin";

        if( !c_sglx_verifySha1( callback, hSglx, file ) )
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


// Par2.
//
void par2_test()
{
    printf( "\nPar2 test...\n\n" );

    void    *hSglx = c_sglx_createHandle();

    if( c_sglx_connect( hSglx, addr, port ) ) {

        char *file = "D:\\AAA\\TT_g0\\TT_g0_imec0\\TT_g0_t0.imec0.ap.bin";

        if( !c_sglx_par2( callback, hSglx, 'v', file ) )
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


// Query.
//
void getParams_test()
{
    printf( "\nGet params...\n\n" );

    void    *hSglx = c_sglx_createHandle();

    if( c_sglx_connect( hSglx, addr, port ) ) {

        int nval, len;

        if( c_sglx_getParams( &nval, hSglx ) ) {

            for( int i = 0; i < nval; ++i )
                printf( "%s\n", c_sglx_getstr( &len, hSglx, i ) );
        }
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


// Structure.
//
void getShankMap_test()
{
    printf( "\nGet shank map...\n\n" );

    void    *hSglx = c_sglx_createHandle();

    if( c_sglx_connect( hSglx, addr, port ) ) {

        int nval, len;

        if( c_sglx_getNIShankMap( &nval, hSglx ) ) {

            for( int i = 0; i < nval; ++i )
                printf( "%s\n", c_sglx_getstr( &len, hSglx, i ) );
        }
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


// Set the user notes for the next run; read that back.
//
void set_get_userNotes_test()
{
    printf( "\nSet, get notes...\n\n" );

    void    *hSglx = c_sglx_createHandle();
    char    *newRunName = 0;

    if( c_sglx_connect( hSglx, addr, port ) ) {

        // Make sure we also set a new run name.
        const char *run;
        if( !c_sglx_getRunName( &run, hSglx ) )
            goto error;

        newRunName = malloc( strlen( run ) + 4 );
        strcpy( newRunName, run );
        strcat( newRunName, "_v2" );

        c_sglx_setkv( hSglx, 0, 0 );
        c_sglx_setkv( hSglx, "snsNotes", "Test setting of run comment field." );
        c_sglx_setkv( hSglx, "snsRunName", newRunName );

        if( !c_sglx_setKVParams( hSglx ) )
            goto error;

        int nval;

        if( c_sglx_getParams( &nval, hSglx ) ) {

            for( int i = 0; i < nval; ++i ) {

                int         len;
                const char  *s = c_sglx_getstr( &len, hSglx, i );

                if( 0 == strncmp( s, "snsNotes", 8 ) ) {
                    printf( "%s\n", s );
                    break;
                }
            }
        }
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );

    if( newRunName )
        free( newRunName );
}


// Fetch and plot latest 1 second of NI channel AI0.
// Run SpikeGLX_NISIM.exe which generates a 1 Hz sine wave on AI0.
//
void plot_NI_1sec_test()
{
    printf( "\nPlot NI AI0...\n\n" );

    void    *hSglx = c_sglx_createHandle();

    if( c_sglx_connect( hSglx, addr, port ) ) {

        double      srate;
        t_ull       headCt;
        const short *data;
        int         channels[]  = {0},
                    nC          = 1,
                    ndata;

        // NI stream is (js,ip) = (0,0)

        if( !(srate = c_sglx_getStreamSampleRate( hSglx, 0, 0 )) )
            goto error;

        printf( "Sample rate %f\n", srate );

        if( !(headCt = c_sglx_fetchLatest( &data, &ndata, hSglx, 0, 0, (int)srate, channels, 1, 1 )) )
            goto error;

        const short *v = data;
        int         nt = ndata / nC;

        printf( "Head count %llu, samples %d\n", headCt, nt );

        for( int it = 0; it < nt; it += 250, v += 250 * nC ) {

            char    star[256];
            int     spc = 40 + (80 * *v) / 32768;

            for( int i = 0; i < spc; ++i )
                star[i] = ' ';
            star[spc] = '*';
            star[spc+1] = 0;

            printf( "%s\n", star );
        }
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );
}


// Continuously fetch data from imec probe-0.
// Threshold selected meas_chan.
// Send digital out command tracking threshold crossings.
//
// Runs until error or Ctrl-C.
//
// To measure closed-loop latency, immerse the probe in saline
// and give it a square wave signal (1 to 5 mV p-p, 10 Hz).
//
// We fetch 384 channels of these data in the remote program;
// fetching performance is similar regardless of channel count.
//
// If the probe has an LF-band we'll use that so the 10 Hz signal
// isn't too distorted, otherwise we'll fetch AP (full-band).
//
// We analyze one of these channels looking for a rising edge.
// We then react to that threshold crossing by commanding an
// NI device to make another edge that is sent to the probe's
// SMA connector as a digital input. The separation between
// the threshold event and the resulting NI event gives a
// direct readout of closed-loop latency. We measure the mean
// closed-loop latency to be 2 to 3 ms.
//
// Note that the square wave amplitude and/or thresh voltage may
// need to be adjusted until you are seeing a regular square
// wave in the imec SY channel; not many edges missed, and not
// too noisy.
//
// While running this script you can enable file writing in SpikeGLX,
// and save a few minutes of recording. Get SpikeGLX_Datafile_Tools
// from the SpikeGLX download site and use its latency_test_analysis
// script to calculate latency statistics.
// https://billkarsh.github.io/SpikeGLX/#post-processing-tools
//
void latency_test()
{
    printf( "\nLatency test...\n\n" );

    void    *hSglx = c_sglx_createHandle();
    int     *channels = 0;

    if( c_sglx_connect( hSglx, addr, port ) ) {

        const char      *line = "Dev4/port0/line5";
        unsigned int    bits = 0;

        double      i162V;
        t_ull       fromCt;
        const short *data;
        int         js = 2,
                    ip = 0,
                    nC = 384,
                    ntypes,
                    nLF,
                    ch0,
                    meas_chan,
                    meas_idx,
                    thresh,
                    ndata;

        // Get [AP LF SY] channel counts for probe stream
        c_sglx_getStreamAcqChans( &ntypes, hSglx, js, ip );
        nLF = c_sglx_getint( hSglx, 1 );

        if( nLF > 0 ) {
            ch0 = 384;
            meas_chan = 393;    // arb chan in fetched set
        }
        else {
            ch0 = 0;
            meas_chan = 9;      // arb chan in fetched set
        }

        channels = malloc( nC * sizeof(int) );

        for( int i = 0; i < 384; ++i )
            channels[i] = ch0 + i;

        meas_idx = meas_chan - ch0;

        c_sglx_getStreamI16ToVolts( &i162V, hSglx, js, ip, meas_chan );
        thresh = (int)(0.00025 / i162V);    // 0.25 mv as probe i16

        printf( "Threshold %d\n", thresh );

        if( !(fromCt = c_sglx_getStreamSampleCount( hSglx, js, ip )) )
            goto error;

        if( !c_sglx_ni_DO_set( hSglx, line, bits ) )
            goto error;

        for( ;; ) {

            t_ull   headCt;

            if( !(headCt = c_sglx_fetch( &data, &ndata, hSglx, js, ip, fromCt, 120, channels, nC, 1 )) )
                goto error;

            int tpts = ndata / nC;

            if( tpts > 1 ) {

                int     diff  = data[meas_idx + (tpts-1)*nC] - data[meas_idx];
                bool    digOK = true;

                if( diff > thresh && bits == 0 ) {
                    bits  = 0xFFFFFFFF;
                    digOK = c_sglx_ni_DO_set( hSglx, line, bits );
                }
                else if( diff < -thresh && bits == 0xFFFFFFFF ) {
                    bits  = 0;
                    digOK = c_sglx_ni_DO_set( hSglx, line, bits );
                }

                if( !digOK )
                    goto error;

                fromCt = headCt + tpts;
            }
        }
    }
    else {
error:
        printf( "error [%s]\n", c_sglx_getError( hSglx ) );
    }

    c_sglx_close( hSglx );
    c_sglx_destroyHandle( hSglx );

    if( channels )
        free( channels );
}


