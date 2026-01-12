
#include "SglxApi.h"
#include "Comm.h"

#include <sstream>
#include <string.h>
using namespace std;


#define HS  reinterpret_cast<t_sglxconn*>(hSglx)

#define CATCH()                                         \
    catch( const exception &e )                         \
        {HS->err = string(__func__) + ": " + e.what();  \
        return false;}


//-----------------------------------------------------------
// Develop/debug tools
#if 0

#include <QDebug>
#include <windows.h>

static double getTime()
{
    static __int64  freq    = 0;
    static __int64  t0      = 0;
    __int64         tNow;

    QueryPerformanceCounter( (LARGE_INTEGER*)&tNow );

    if( !t0 )
        t0 = tNow;

    if( !freq )
        QueryPerformanceFrequency( (LARGE_INTEGER*)&freq );

    return double(tNow - t0) / double(freq);
}
#endif
//-----------------------------------------------------------




SGLX_EXPORT void* SGLX_CALL sglx_createHandle(
    Tcpp_zer_str        cpp_zer_str,
    Tcpp_set_str        cpp_set_str,
    Tcpp_get_str        cpp_get_str )
{
    t_sglxconn  *S = new t_sglxconn;

    S->cpp_zer_str      = cpp_zer_str;
    S->cpp_set_str      = cpp_set_str;
    S->cpp_get_str      = cpp_get_str;

    return S;
}


SGLX_EXPORT void SGLX_CALL sglx_destroyHandle( void *hSglx )
{
    if( hSglx )
        delete HS;
}


SGLX_EXPORT const char* SGLX_CALL sglx_getError( void *hSglx )
{
    return HS->err.c_str();
}


SGLX_EXPORT bool SGLX_CALL sglx_connect(
    void        *hSglx,
    const char  *host,
    int         port )
{
    HS->vers.clear();
    HS->host            = host;
    HS->port            = port;
    HS->handle          = -1;
    HS->in_checkconn    = false;

    Comm    C;

    try {
        return C.checkConn( HS );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_close( void *hSglx )
{
    if( !hSglx )
        return true;

    Comm    C;

    try {
        C.close( HS );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_consoleHide( void *hSglx )
{
    Comm    C;

    try {
        return C.doCommand( HS, "CONSOLEHIDE" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_consoleShow( void *hSglx )
{
    Comm    C;

    try {
        return C.doCommand( HS, "CONSOLESHOW" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_enumDataDir(
    T_sglx_get_strs     &vs,
    void                *hSglx,
    int                 idir )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "ENUMDATADIR %d", idir );

    try {
        return C.doNLineQuery( vs, HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_fetch( T_sglx_fetch &io, void *hSglx, t_ull start_samp )
{
    Comm    C;
    string  s;
    char    cmd[32];

    try {
        C.checkConn( HS );
        sprintf( cmd, "FETCH %d %d %llu %d ", io.js, io.ip, start_samp, io.max_samps );

        ostringstream   ss;

        for( int i = 0; i < io.n_cs; ++i )
            ss << io.channel_subset[i] << "#";

        if( !ss.str().size() )
            ss << "-1#";

        ss << " " << io.downsample << "\n";

        C.sendString( HS, cmd + ss.str() );
        C.read_1s_srvside( s, HS );

        t_ull   headCt;
        int     nchans, nsamps;
        sscanf( s.c_str(), "BINARY_DATA %d %d uint64(%llu)", &nchans, &nsamps, &headCt );

        C.readBinary( io._dispatch( io, nchans * nsamps ),
            nchans * nsamps * sizeof(short), HS );

        C.receiveOK( HS, "FETCH" );

        if( !headCt )
            HS->cpp_set_str( HS->err, "sglx_fetch: Stream not running." );

        return headCt;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_fetchLatest( T_sglx_fetch &io, void *hSglx )
{
    t_ull   cur_samps = sglx_getStreamSampleCount( HS, io.js, io.ip );

    if( !cur_samps )
        return 0;

    int max_samps = io.max_samps;

    if( max_samps > cur_samps )
        max_samps = cur_samps;

    return sglx_fetch( io, hSglx, cur_samps - max_samps );
}


SGLX_EXPORT bool SGLX_CALL sglx_getDataDir(
    std::string &dir,
    void        *hSglx,
    int         idir )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "GETDATADIR %d", idir );

    try {
        return C.do1LineQuery( dir, HS, cmd, false );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getGeomMap(
    T_sglx_get_strs     &kv,
    void                *hSglx,
    int                 ip )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "GETGEOMMAP %d", ip );

    try {
        return C.doNLineQuery( kv, HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getImecChanGains(
    double  &APgain,
    double  &LFgain,
    void    *hSglx,
    int     ip,
    int     chan )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETIMECCHANGAINS %d %d", ip, chan );

    try {
        APgain = 0;
        LFgain = 0;
        C.do1LineQuery( s, HS, cmd, true );

        if( 2 == sscanf( s.c_str(), "%lf %lf", &APgain, &LFgain ) )
            return true;
        else {
            HS->cpp_set_str( HS->err, "sglx_getImecChanGains: Garbled gain string." );
            return false;
        }
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getLastGT( int &g, int &t, void *hSglx )
{
    Comm    C;
    string  s;

    try {
        g = -1;
        t = -1;
        C.do1LineQuery( s, HS, "GETLASTGT", true );

        if( 2 == sscanf( s.c_str(), "%d %d", &g, &t ) )
            return true;
        else {
            HS->cpp_set_str( HS->err, "sglx_getLastGT: Garbled gt string." );
            return false;
        }
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParams(
    T_sglx_get_strs     &kv,
    void                *hSglx )
{
    Comm    C;

    try {
        return C.doNLineQuery( kv, HS, "GETPARAMS" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecCommon(
    T_sglx_get_strs     &kv,
    void                *hSglx )
{
    Comm    C;

    try {
        return C.doNLineQuery( kv, HS, "GETPARAMSIMALL" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecProbe(
    T_sglx_get_strs     &kv,
    void                *hSglx,
    int                 ip )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "GETPARAMSIMPRB %d", ip );

    try {
        return C.doNLineQuery( kv, HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParamsOneBox(
    T_sglx_get_strs     &kv,
    void                *hSglx,
    int                 ip,
    int                 slot )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "GETPARAMSOBX %d %d", ip, slot );

    try {
        return C.doNLineQuery( kv, HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getProbeAddrs( std::string &list, void *hSglx )
{
    Comm    C;

    try {
        return C.do1LineQuery( list, HS, "GETPROBEADDRS", false );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getProbeList( std::string &list, void *hSglx )
{
    Comm    C;

    try {
        return C.do1LineQuery( list, HS, "GETPROBELIST", false );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getRunName( std::string &name, void *hSglx )
{
    Comm    C;

    try {
        return C.do1LineQuery( name, HS, "GETRUNNAME", false );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamAcqChans(
    T_sglx_get_ints     &vint,
    void                *hSglx,
    int                 js,
    int                 ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMACQCHANS %d %d", js, ip );

    try {
        vint._dispatch( vint, 0, 0 );
        C.do1LineQuery( s, HS, cmd, true );

        const char  *p = s.c_str(),
                    *L = p + strlen( p );

        while( p < L ) {
            int i, k;
            if( sscanf( p, "%d%n", &i, &k ) ) {
                vint._dispatch( vint, i, 1 );
                p += k + 1;
            }
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamFileStart(
    void    *hSglx,
    int     js,
    int     ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMFILESTART %d %d", js, ip );

    try {
        C.do1LineQuery( s, HS, cmd, true );

        t_ull   start = stoull( s );

        if( !start )
            HS->cpp_set_str( HS->err, "sglx_getStreamFileStart: Stream not running." );

        return start;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamI16ToVolts(
    double  &mult,
    void    *hSglx,
    int     js,
    int     ip,
    int     chan )
{
    Comm    C;
    string  s;
    char    cmd[64];
    sprintf( cmd, "GETSTREAMI16TOVOLTS %d %d %d", js, ip, chan );

    try {
        mult = 0;
        C.do1LineQuery( s, HS, cmd, true );
        mult = stod( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamMaxInt(
    int     &maxInt,
    void    *hSglx,
    int     js,
    int     ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMMAXINT %d %d", js, ip );

    try {
        maxInt = 0;
        C.do1LineQuery( s, HS, cmd, true );
        maxInt = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamNP( int &np, void *hSglx, int js )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMNP %d", js );

    try {
        np = 0;
        C.do1LineQuery( s, HS, cmd, true );
        np = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamSampleCount(
    void    *hSglx,
    int     js,
    int     ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSAMPLECOUNT %d %d", js, ip );

    try {
        C.do1LineQuery( s, HS, cmd, true );

        t_ull   count = stoull( s );

        if( !count )
            HS->cpp_set_str( HS->err, "sglx_getStreamSampleCount: Stream not running." );

        return count;
    }
    CATCH()
}


SGLX_EXPORT double SGLX_CALL sglx_getStreamSampleRate(
    void    *hSglx,
    int     js,
    int     ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSAMPLERATE %d %d", js, ip );

    try {
        C.do1LineQuery( s, HS, cmd, true );

        double  rate = stod( s );

        if( !rate )
            HS->cpp_set_str( HS->err, "sglx_getStreamSampleRate: Stream not running." );

        return rate;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamSaveChans(
    T_sglx_get_ints     &vint,
    void                *hSglx,
    int                 js,
    int                 ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSAVECHANS %d %d", js, ip );

    try {
        vint._dispatch( vint, 0, 0 );
        C.do1LineQuery( s, HS, cmd, true );

        const char  *p = s.c_str(),
                    *L = p + strlen( p );

        while( p < L ) {
            int i, k;
            if( sscanf( p, "%d%n", &i, &k ) ) {
                vint._dispatch( vint, i, 1 );
                p += k + 1;
            }
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamShankMap(
    t_sglxshankmap  &shankmap,
    void            *hSglx,
    int             js,
    int             ip )
{
    Comm    C;
    string  s;
    char    cmd[32];

    try {
        C.checkConn( HS );
        sprintf( cmd, "GETSTREAMSHANKMAP %d %d\n", js, ip );
        C.sendString( HS, cmd );
        C.read_1s_srvside( s, HS );

        sscanf( s.c_str(), "SHANKMAP %d %d %d %d",
            &shankmap.ns, &shankmap.nc, &shankmap.nr, &shankmap.ne );

        C.readBinary( shankmap._dispatch( shankmap ),
            shankmap.ne * sizeof(t_sglxshankmap::t_entry), HS );

        C.receiveOK( HS, "GETSTREAMSHANKMAP" );

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamSN(
    int             &slot_or_type,
    std::string     &SN,
    void            *hSglx,
    int             js,
    int             ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSN %d %d", js, ip );

    try {
        slot_or_type = 0;
        HS->cpp_zer_str( SN );
        C.do1LineQuery( s, HS, cmd, true );

        char    sn[64];
        int     i = -1;

        if( 2 == sscanf( s.c_str(), "%[^ ] %d", sn, &i ) ) {
            HS->cpp_set_str( SN, sn );
            slot_or_type = i;
            return true;
        }
        else {
            HS->cpp_set_str( HS->err, "sglx_getStreamSN: Garbled SN string." );
            return false;
        }
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamVoltageRange(
    double  &vMin,
    double  &vMax,
    void    *hSglx,
    int     js,
    int     ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMVOLTAGERANGE %d %d", js, ip );

    try {
        vMin = 0;
        vMax = 0;
        C.do1LineQuery( s, HS, cmd, true );

        if( 2 == sscanf( s.c_str(), "%lf %lf", &vMin, &vMax ) )
            return true;
        else {
            HS->cpp_set_str( HS->err, "sglx_getStreamVoltageRange: Garbled range string." );
            return false;
        }
    }
    CATCH()
}


SGLX_EXPORT double SGLX_CALL sglx_getTime( void *hSglx )
{
    Comm    C;
    string  s;

    try {
        C.do1LineQuery( s, HS, "GETTIME", true );
        return stod( s );
    }
    CATCH()
}


SGLX_EXPORT const char* SGLX_CALL sglx_getVersion( void *hSglx )
{
    return HS->vers.c_str();
}


SGLX_EXPORT bool SGLX_CALL sglx_isConsoleHidden( bool &hidden, void *hSglx )
{
    Comm    C;
    string  s;

    try {
        hidden = false;
        C.do1LineQuery( s, HS, "ISCONSOLEHIDDEN", true );
        hidden = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isInitialized( bool &ready, void *hSglx )
{
    Comm    C;
    string  s;

    try {
        ready = false;
        C.do1LineQuery( s, HS, "ISINITIALIZED", true );
        ready = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isRunning( bool &running, void *hSglx )
{
    Comm    C;
    string  s;

    try {
        running = false;
        C.do1LineQuery( s, HS, "ISRUNNING", true );
        running = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isSaving( bool &saving, void *hSglx )
{
    Comm    C;
    string  s;

    try {
        saving = false;
        C.do1LineQuery( s, HS, "ISSAVING", true );
        saving = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isUserOrder(
    bool    &userOrder,
    void    *hSglx,
    int     js,
    int     ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "ISUSRORDER %d %d", js, ip );

    try {
        userOrder = false;
        C.do1LineQuery( s, HS, cmd, true );
        userOrder = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_mapSample(
    void    *hSglx,
    int     dstjs,
    int     dstip,
    t_ull   srcSample,
    int     srcjs,
    int     srcip )
{
    Comm    C;
    string  s;
    char    cmd[64];
    sprintf( cmd, "MAPSAMPLE %d %d %llu %d %d", dstjs, dstip, srcSample, srcjs, srcip );

    try {
        C.do1LineQuery( s, HS, cmd, true );

        t_ull   dstSample = stoull( s );

        if( !dstSample )
            HS->cpp_set_str( HS->err, "sglx_mapSample: Stream not running." );

        return dstSample;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_ni_DO_set(
    void                *hSglx,
    const std::string   &lines,
    unsigned int        bits )
{
    Comm    C;
    char    sbits[32];
    sprintf( sbits, " %u", bits );

    try {
        return C.doCommand( HS, string("NIDOSET ") + HS->cpp_get_str(lines) + sbits );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_ni_wave_arm(
    void                *hSglx,
    const std::string   &outChan,
    const std::string   &trigTerm )
{
    Comm    C;

    try {
        return C.doCommand( HS, string("NIWVARM ")
                + HS->cpp_get_str(outChan)
                + string(" ")
                + HS->cpp_get_str(trigTerm) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_ni_wave_load(
    void                *hSglx,
    const std::string   &outChan,
    const std::string   &wave,
    bool                loop )
{
    Comm    C;
    char    sloop[32];
    sprintf( sloop, " %d", loop );

    try {
        return C.doCommand( HS, string("NIWVLOAD ")
                + HS->cpp_get_str(outChan)
                + string(" ")
                + HS->cpp_get_str(wave)
                + sloop );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_ni_wave_startstop(
    void                *hSglx,
    const std::string   &outChan,
    bool                start )
{
    Comm    C;
    char    sstart[32];
    sprintf( sstart, " %d", start );

    try {
        return C.doCommand( HS, string("NIWVSTSP ") + HS->cpp_get_str(outChan) + sstart );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_obx_AO_set(
    void                *hSglx,
    int                 ip,
    int                 slot,
    const std::string   &chn_vlt )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "OBXAOSET %d %d ", ip, slot );

    try {
        return C.doCommand( HS, string(cmd) + HS->cpp_get_str(chn_vlt) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_obx_wave_arm(
    void    *hSglx,
    int     ip,
    int     slot,
    int     trig,
    bool    loop )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "OBXWVARM %d %d %d %d", ip, slot, trig, loop );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_obx_wave_load(
    void                *hSglx,
    int                 ip,
    int                 slot,
    const std::string   &wave )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "OBXWVLOAD %d %d ", ip, slot );

    try {
        return C.doCommand( HS, string(cmd) + HS->cpp_get_str(wave) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_obx_wave_startstop(
    void    *hSglx,
    int     ip,
    int     slot,
    bool    start )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "OBXWVSTSP %d %d %d", ip, slot, start );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_opto_emit(
    void    *hSglx,
    int     ip,
    int     color,
    int     site )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "OPTOEMIT %d %d %d", ip, color, site );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_opto_getAttenuations(
    T_sglx_get_dbls     &vdbl,
    void                *hSglx,
    int                 ip,
    int                 color )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "OPTOGETATTENS %d %d", ip, color );

    try {
        vdbl._dispatch( vdbl, 0, 0 );
        C.do1LineQuery( s, HS, cmd, true );

        const char  *p = s.c_str(),
                    *L = p + strlen( p );

        while( p < L ) {
            double  d;
            int     k;
            if( sscanf( p, "%lf%n", &d, &k ) ) {
                vdbl._dispatch( vdbl, d, 1 );
                p += k + 1;
            }
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_par2(
    void                (SGLX_CALL *callback)(const char *status),
    void                *hSglx,
    char                op,
    const std::string   &file )
{
    Comm    C;
    char    cmd[32];
    bool    running;

    if( !sglx_isRunning( running, hSglx ) )
        return false;
    else if( running ) {
        HS->cpp_set_str( HS->err, "sglx_par2: Can't be used during run." );
        return false;
    }

    try {
        C.checkConn( HS );
        sprintf( cmd, "PAR2 %c ", op );
        C.sendString( HS, string(cmd) + HS->cpp_get_str(file) + "\n" );

        for(;;) {

            string      s;
            const char  *s0;
            C.read_1s_srvside( s, HS );
            s0 = s.c_str();

            if( !strcmp( s0, "OK" ) )
                return true;

            if( callback )
                callback( s0 );
        }
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_pause_graphs( void *hSglx, bool pause )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "PAUSEGRF %d", pause );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setAnatomy_Pinpoint(
    void                *hSglx,
    const std::string   &shankdat )
{
    Comm    C;

    try {
        return C.doCommand( HS, string("SETANATOMYPP ") + HS->cpp_get_str(shankdat) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setAudioEnable( void *hSglx, bool enable )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETAUDIOENABLE %d", enable );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setAudioParams(
    void                *hSglx,
    const std::string   &group,
    T_sglx_set_keyvals  &kv )
{
    Comm    C;

    try {
        C.checkConn( HS );
        C.sendString( HS, string("SETAUDIOPARAMS ") + HS->cpp_get_str(group) + "\n" );
        C.receiveREADY( HS, "SETAUDIOPARAMS" );

        const char  *key = 0, *val = 0;

        while( kv._dispatch( kv, key, val ) )
            C.sendString( HS, string(key) + "=" + val + "\n" );

        C.sendString( HS, "\n" );
        return C.receiveOK( HS, "SETAUDIOPARAMS" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setDataDir(
    void                *hSglx,
    int                 idir,
    const std::string   &dir )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETDATADIR %d ", idir );

    try {
        return C.doCommand( HS, string(cmd) + HS->cpp_get_str(dir) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setMetadata(
    void                *hSglx,
    T_sglx_set_keyvals  &kv )
{
    Comm    C;

    try {
        C.checkConn( HS );
        C.sendString( HS, "SETMETADATA\n" );
        C.receiveREADY( HS, "SETMETADATA" );

        const char  *key = 0, *val = 0;

        while( kv._dispatch( kv, key, val ) )
            C.sendString( HS, string(key) + "=" + val + "\n" );

        C.sendString( HS, "\n" );
        return C.receiveOK( HS, "SETMETADATA" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setMultiDriveEnable( void *hSglx, bool enable )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETMULTIDRIVEENABLE %d", enable );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setNextFileName(
    void                *hSglx,
    const std::string   &name )
{
    Comm    C;

    try {
        return C.doCommand( HS, string("SETNEXTFILENAME ") + HS->cpp_get_str(name) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParams(
    void                *hSglx,
    T_sglx_set_keyvals  &kv )
{
    Comm    C;

    try {
        C.checkConn( HS );
        C.sendString( HS, "SETPARAMS\n" );
        C.receiveREADY( HS, "SETPARAMS" );

        const char  *key = 0, *val = 0;

        while( kv._dispatch( kv, key, val ) )
            C.sendString( HS, string(key) + "=" + val + "\n" );

        C.sendString( HS, "\n" );
        return C.receiveOK( HS, "SETPARAMS" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecCommon(
    void                *hSglx,
    T_sglx_set_keyvals  &kv )
{
    Comm    C;

    try {
        C.checkConn( HS );
        C.sendString( HS, "SETPARAMSIMALL\n" );
        C.receiveREADY( HS, "SETPARAMSIMALL" );

        const char  *key = 0, *val = 0;

        while( kv._dispatch( kv, key, val ) )
            C.sendString( HS, string(key) + "=" + val + "\n" );

        C.sendString( HS, "\n" );
        return C.receiveOK( HS, "SETPARAMSIMALL" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecProbe(
    void                *hSglx,
    T_sglx_set_keyvals  &kv,
    int                 ip )
{
    Comm    C;
    char    cmd[32];

    try {
        C.checkConn( HS );
        sprintf( cmd, "SETPARAMSIMPRB %d\n", ip );
        C.sendString( HS, cmd );
        C.receiveREADY( HS, "SETPARAMSIMPRB" );

        const char  *key = 0, *val = 0;

        while( kv._dispatch( kv, key, val ) )
            C.sendString( HS, string(key) + "=" + val + "\n" );

        C.sendString( HS, "\n" );
        return C.receiveOK( HS, "SETPARAMSIMPRB" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParamsOneBox(
    void                *hSglx,
    T_sglx_set_keyvals  &kv,
    int                 ip,
    int                 slot )
{
    Comm    C;
    char    cmd[32];

    try {
        C.checkConn( HS );
        sprintf( cmd, "SETPARAMSOBX %d %d\n", ip, slot );
        C.sendString( HS, cmd );
        C.receiveREADY( HS, "SETPARAMSOBX" );

        const char  *key = 0, *val = 0;

        while( kv._dispatch( kv, key, val ) )
            C.sendString( HS, string(key) + "=" + val + "\n" );

        C.sendString( HS, "\n" );
        return C.receiveOK( HS, "SETPARAMSOBX" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setRecordingEnable( void *hSglx, bool enable )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETRECORDENAB %d", enable );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setRunName(
    void                *hSglx,
    const std::string   &name )
{
    Comm    C;

    try {
        return C.doCommand( HS, string("SETRUNNAME ") + HS->cpp_get_str(name) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOffBeep(
    void    *hSglx,
    int     hertz,
    int     millisec )
{
    Comm    C;
    char    cmd[64];
    sprintf( cmd, "SETTRIGGEROFFBEEP %d %d", hertz, millisec );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOnBeep(
    void    *hSglx,
    int     hertz,
    int     millisec )
{
    Comm    C;
    char    cmd[64];
    sprintf( cmd, "SETTRIGGERONBEEP %d %d", hertz, millisec );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_startRun(
    void                *hSglx,
    const std::string   &name )
{
    if( HS->cpp_get_str(name)[0] && !sglx_setRunName( HS, name ) )
        return false;

    Comm    C;

    try {
        return C.doCommand( HS, "STARTRUN" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_stopRun( void *hSglx )
{
    Comm    C;

    try {
        return C.doCommand( HS, "STOPRUN" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_triggerGT( void *hSglx, int g, int t )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "TRIGGERGT %d %d", g, t );

    try {
        return C.doCommand( HS, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_verifySha1(
    void                (SGLX_CALL *callback)(const char *status),
    void                *hSglx,
    const std::string   &filename )
{
    Comm    C;
    bool    running;

    if( !sglx_isRunning( running, hSglx ) )
        return false;
    else if( running ) {
        HS->cpp_set_str( HS->err, "sglx_verifySha1: Can't be used during run." );
        return false;
    }

    try {
        C.checkConn( HS );
        C.sendString( HS, string("VERIFYSHA1 ") + HS->cpp_get_str(filename) + "\n" );

        for(;;) {

            string      s;
            const char  *s0;
            C.read_1s_srvside( s, HS );
            s0 = s.c_str();

            if( !strcmp( s0, "OK" ) )
                return true;

            if( callback )
                callback( s0 );
        }
    }
    CATCH()
}


