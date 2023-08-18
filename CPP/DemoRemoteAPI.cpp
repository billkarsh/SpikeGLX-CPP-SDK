// =================================
// Random assortment of remote calls
// =================================

//#include "stdafx.h" // enable if using Visual Studio precompiled headers

#include "SglxCppClient.h"
using namespace std;




void justConnect()
{
    printf( "\nCalling connect...\n\n" );

    void    *hSglx = sglx_createHandle_std();

// Using default loopback address and port

    if( sglx_connect( hSglx ) )
        printf( "version <%s>\n", sglx_getVersion( hSglx ) );
    else
        printf( "error [%s]\n", sglx_getError( hSglx ) );

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


// Simple query.
//
void console_test()
{
    printf( "\nConsole test...\n\n" );

    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx ) ) {

        bool    hid;

        if( sglx_isConsoleHidden( hid, hSglx ) )
            printf( "Console hidden: %d\n", hid );
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


// Query.
//
void getParams_test()
{
    printf( "\nGet params...\n\n" );

    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx ) ) {

        cppClient_sglx_get_keyvals  kv;

        if( sglx_getParams( kv, hSglx ) ) {

            for( map<string,string>::iterator it=kv.mstrstr.begin(), end=kv.mstrstr.end(); it != end; ++it )
                printf( "%s = %s\n", it->first.c_str(), it->second.c_str() );
        }
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


// Structure.
//
void getShankMap_test()
{
    printf( "\nGet shank map...\n\n" );

    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx ) ) {

        cppClient_sglxshankmap  map;

        if( sglx_getStreamShankMap( map, hSglx, 0, 0 ) ) {

            printf( "ne ns nc nr: %d %d %d %d\n\n", map.ne, map.ns, map.nc, map.nr );

            for( int ie = 0; ie < map.ne; ++ie ) {
                const t_sglxshankmap::t_entry &E = map.e[ie];
                printf( "chan %d:  %d %d %d %d\n", ie, E.s, E.c, E.r, E.u );
            }
        }
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


// Set the user notes for the next run; read that back.
//
void set_get_userNotes_test()
{
    printf( "\nSet, get notes...\n\n" );

    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx ) ) {

        // Make sure we also set a new run name.
        string  run;
        if( !sglx_getRunName( run, hSglx ) )
            goto error;

        cppClient_sglx_set_keyvals  skv;
        skv.mstrstr["snsNotes"]      = "Test setting of run comment field.";
        skv.mstrstr["snsRunName"]    = run + "_v2";

        if( !sglx_setParams( hSglx, skv ) )
            goto error;

        cppClient_sglx_get_keyvals  gkv;

        if( sglx_getParams( gkv, hSglx ) )
            printf( "snsNotes = %s\n", gkv.mstrstr["snsNotes"].c_str() );
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


// Fetch and plot latest 1 second of NI channel AI0.
// Run SpikeGLX_NISIM.exe which generates a 1 Hz sine wave on AI0.
//
void plot_NI_1sec_test()
{
    printf( "\nPlot NI AI0...\n\n" );

    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx ) ) {

        cppClient_sglx_fetch    io;
        double                  srate;
        t_ull                   headCt;

        // NI stream is (js,ip) = (0,0)

        if( !(srate = sglx_getStreamSampleRate( hSglx, 0, 0 )) )
            goto error;

        printf( "Sample rate %f\n", srate );

        io.chans.push_back( 0 );
        io.channel_subset = &io.chans[0];
        io.n_cs         = 1;
        io.max_samps    = int(srate);
        io.downsample   = 1;
        io.js           = 0;
        io.ip           = 0;

        if( !(headCt = sglx_fetchLatest( io, hSglx )) )
            goto error;

        short   *v = &io.data[0];
        int     nC = 1,
                nt = int(io.data.size() / nC);

        printf( "Head count %llu, samples %d\n", headCt, nt );

        for( int it = 0; it < nt; it += 250, v += 250 * nC ) {

            int spc = 40 + (80 * *v) / 32768;

            string s( spc, ' ' );
            s += "*";
            printf( "%s\n", s.c_str() );
        }
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


// Continuously fetch LFP-band data from imec probe-0.
// Threshold channel 393 @ 0.45 mV.
// Send digital out command tracking threshold crossings.
//
// Runs until error or Ctrl-C.
//
// To measure closed-loop latency, immerse the probe in
// saline and give it a square wave signal (1 mV p-p, 1 Hz).
// We fetch all 384 channels of LFP data in a remote program.
// We analyze one of these channels looking for a rising edge.
// We then react to that threshold crossing by commanding an
// NI device to make another edge that is sent to the probe's
// SMA connector as a digital input. Now the separation between
// the LFP threshold event and the resulting NI event gives a
// direct readout of closed-loop latency. We measure the typical
// closed-loop latency to be 5.5 ms using the Cpp API.
//
void latency_test()
{
    printf( "\nLatency test...\n\n" );

    void    *hSglx = sglx_createHandle_std();

    if( sglx_connect( hSglx ) ) {

        cppClient_sglx_fetch    io;
        double                  mv2i16  = 1.0/(1200.0/250/1024);
        const char              *line   = "Dev4/port0/line5";
        t_ull                   fromCt;
        int                     nC      = 385,
                                id      = 393 - 384,
                                thresh  = 0.45*mv2i16;
        bool                    level   = 0;

        printf( "Threshold %d\n", thresh );

        for( int i = 384; i < 769; ++i )
            io.chans.push_back( i );
        io.channel_subset = &io.chans[0];
        io.n_cs         = nC;
        io.max_samps    = 120;
        io.downsample   = 1;
        io.js           = 2;
        io.ip           = 0;

        if( !(fromCt = sglx_getStreamSampleCount( hSglx, io.js, io.ip )) )
            goto error;

        if( !sglx_setDigitalOut( hSglx, level, line ) )
            goto error;

        for( ;; ) {

            t_ull   headCt;

            if( !(headCt = sglx_fetch( io, hSglx, fromCt )) )
                goto error;

            int tpts = io.data.size() / nC;

            if( tpts > 1 ) {

                int     diff  = io.data[id + (tpts-1)*nC] - io.data[id];
                bool    digOK = true;

                if( diff > thresh && level == 0 ) {
                    level = 1;
                    digOK = sglx_setDigitalOut( hSglx, level, line );
                }
                else if( diff < -thresh && level == 1 ) {
                    level = 0;
                    digOK = sglx_setDigitalOut( hSglx, level, line );
                }

                if( !digOK )
                    goto error;

                fromCt = headCt + tpts;
            }
        }
    }
    else {
error:
        printf( "error [%s]\n", sglx_getError( hSglx ) );
    }

    sglx_close( hSglx );
    sglx_destroyHandle( hSglx );
}


