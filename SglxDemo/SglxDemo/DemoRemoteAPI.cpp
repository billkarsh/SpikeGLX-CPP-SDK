// =================================
// Random assortment of remote calls
// =================================

#include "SglxCppClient.h"
using namespace std;




void justConnect()
{
    printf( "\nCalling connect...\n\n" );

    t_sglxconn  S;

// Using default loopback address and port

    if( sglx_connect_std( S ) ) {
        printf( "vers %s\n", S.vers.c_str() );
        printf( "host %s\n", S.host.c_str() );
        printf( "port %d\n", S.port );
        printf( "hndl %d\n", S.handle );
    }
    else
        printf( "error [%s]\n", S.err.c_str() );
}


// Simple query.
//
void consoleTest()
{
    printf( "\nConsole test...\n\n" );

    t_sglxconn  S;

    if( sglx_connect_std( S ) ) {

        bool    hid;

        if( sglx_isConsoleHidden( hid, S ) )
            printf( "Console hidden: %d\n", hid );
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", S.err.c_str() );
    }
}


// Query.
//
void getParams()
{
    printf( "\nGet params...\n\n" );

    t_sglxconn  S;

    if( sglx_connect_std( S ) ) {

        map<string,string>  m;

        if( sglx_getParams( m, S ) ) {

            for( map<string,string>::iterator it=m.begin(), end=m.end(); it != end; ++it )
                printf( "%s = %s\n", it->first.c_str(), it->second.c_str() );
        }
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", S.err.c_str() );
    }
}


// Structure.
//
void getShankMap()
{
    printf( "\nGet shank map...\n\n" );

    t_sglxconn  S;

    if( sglx_connect_std( S ) ) {

        t_sglxshankmap  map;

        if( sglx_getStreamShankMap( map, S, 0, 0 ) ) {

            printf( "ns nc nr ne: %d %d %d %d\n\n", map.ns, map.nc, map.nr, map.ne );

            for( int ic = 0; ic < map.ne; ++ic ) {
                const t_sglxshankmap::t_entry *E = map.e_i( ic );
                printf( "chan %d:  %d %d %d %d\n", ic, E->s, E->c, E->r, E->u );
            }
        }
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", S.err.c_str() );
    }
}


// Set the user notes for the next run; read that back.
//
void set_get_userNotes()
{
    printf( "\nSet, get notes...\n\n" );

    t_sglxconn  S;

    if( sglx_connect_std( S ) ) {

        // Make sure we also set a new run name.
        string  run;
        if( !sglx_getRunName( run, S ) )
            goto error;

        map<string,string>  m1;
        m1["snsNotes"]   = "Test setting of run comment field.";
        m1["snsRunName"] = run + "_v2";

        if( !sglx_setParams( S, m1 ) )
            goto error;

        map<string,string>  m2;

        if( sglx_getParams( m2, S ) )
            printf( "snsNotes = %s\n", m2["snsNotes"].c_str() );
        else
            goto error;
    }
    else {
error:
        printf( "error [%s]\n", S.err.c_str() );
    }
}


// Fetch and plot latest 1 second of NI channel AI0.
// Run SpikeGLX_NISIM.exe which generates a 1 Hz sine wave on AI0.
//
void plot_NI_1sec()
{
    printf( "\nPlot NI AI0...\n\n" );

    t_sglxconn  S;

    if( sglx_connect_std( S ) ) {

        vector<short>   data;
        vector<int>     channels;
        double          srate;
        t_ull           headCt;

        // NI stream is (js,ip) = (0,0)

        if( !(srate = sglx_getStreamSampleRate( S, 0, 0 )) )
            goto error;

        printf( "Sample rate %f\n", srate );

        channels.push_back( 0 );

        if( !(headCt = sglx_fetchLatest( data, S, 0, 0, int(srate), channels )) )
            goto error;

        short   *v = &data[0];
        int     nC = 1,
                nt = int(data.size() / nC);

        printf( "Head count %llu, samples %d\n", headCt, nt );

        for( int it = 0; it < nt; it += 250, v += 250 * nC ) {

            int spc = 40 + (80 * *v) / 32768;

            string s;
            for( int i = 0; i < spc; ++i )
                s += " ";
            s += "*";
            printf( "%s\n", s.c_str() );
        }
    }
    else {
error:
        printf( "error [%s]\n", S.err.c_str() );
    }
}


// Continuously fetch LFP-band data from imec probe-0.
// Threshold channel 393 @ 0.45 mV.
// Send digital out command tracking threshold crossings.
//
// Runs until error or Ctrl-C.
//
// To measure closed-loop latency, immerse the probe in
// saline and give it a square wave signal (1 mV p-p, 1 Hz).
// We fetch all 384 channels of these data in a remote program.
// We analyze one of these channels looking for a rising edge.
// We then react to that threshold crossing by commanding an
// NI device to make another edge that is sent to the probe's
// SMA connector as a digital input. Now the separation between
// the LFP threshold event and the resulting NI event gives a
// direct readout of closed-loop latency. We measure the median
// closed-loop latency to be 7.8 ms using the Cpp API.
//
void latency_test()
{
    printf( "\nLatency test...\n\n" );

    t_sglxconn  S;

    if( sglx_connect_std( S ) ) {

        vector<short>   data;
        vector<int>     channels;
        double          mv2i16  = 1.0/(1200.0/250/1024);
        char            *line   = "Dev4/port0/line5";
        t_ull           fromCt;
        int             js      = 2,
                        ip      = 0,
                        nC      = 385,
                        id      = 393 - 384,
                        thresh  = 0.45*mv2i16;
        bool            level   = 0;

        printf( "Threshold %d\n", thresh );

        for( int i = 384; i < 769; ++i )
            channels.push_back( i );

        if( !(fromCt = sglx_getStreamSampleCount( S, js, ip )) )
            goto error;

        if( !sglx_setDigitalOut( S, level, line ) )
            goto error;

        for( ;; ) {

            t_ull   headCt;

            if( !(headCt = sglx_fetch( data, S, js, ip, fromCt, 120, channels )) )
                goto error;

            int tpts = data.size() / nC,
                diff = data[id + (tpts-1)*nC] - data[id];

            bool    digOK = true;

            if( diff > thresh && level == 0 ) {
                level = 1;
                digOK = sglx_setDigitalOut( S, level, line );
            }
            else if( diff < -thresh && level == 1 ) {
                level = 0;
                digOK = sglx_setDigitalOut( S, level, line );
            }

            if( !digOK )
                goto error;

            fromCt = headCt + tpts;
        }
    }
    else {
error:
        printf( "error [%s]\n", S.err.c_str() );
    }
}


