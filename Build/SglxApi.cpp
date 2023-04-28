
#include "SglxApi.h"
#include "Comm.h"

#include <sstream>
#include <string.h>
using namespace std;


#define CATCH()                                     \
    catch( const exception &e )                     \
    {S.cpp_ins_str( S.err,                          \
    (string(__func__) + ": " + e.what()).c_str() ); \
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




SGLX_EXPORT bool SGLX_CALL sglx_connect(
    t_sglxconn          &S,
    Tcpp_zer_str        cpp_zer_str,
    Tcpp_zer_vi32       cpp_zer_vi32,
    Tcpp_zer_vdbl       cpp_zer_vdbl,
    Tcpp_zer_vstr       cpp_zer_vstr,
    Tcpp_zer_mstrstr    cpp_zer_mstrstr,
    Tcpp_ins_str        cpp_ins_str,
    Tcpp_ins_vi32       cpp_ins_vi32,
    Tcpp_ins_vdbl       cpp_ins_vdbl,
    Tcpp_ins_vstr       cpp_ins_vstr,
    Tcpp_ins_mstrstr    cpp_ins_mstrstr,
    Tcpp_siz_vi16       cpp_siz_vi16,
    Tcpp_get_str        cpp_get_str,
    Tcpp_itr_vi32       cpp_itr_vi32,
    Tcpp_itr_mstrstr    cpp_itr_mstrstr,
    const std::string   &host,
    int                 port )
{
    S.cpp_zer_str       = cpp_zer_str;
    S.cpp_zer_vi32      = cpp_zer_vi32;
    S.cpp_zer_vdbl      = cpp_zer_vdbl;
    S.cpp_zer_vstr      = cpp_zer_vstr;
    S.cpp_zer_mstrstr   = cpp_zer_mstrstr;
    S.cpp_ins_str       = cpp_ins_str;
    S.cpp_ins_vi32      = cpp_ins_vi32;
    S.cpp_ins_vdbl      = cpp_ins_vdbl;
    S.cpp_ins_vstr      = cpp_ins_vstr;
    S.cpp_ins_mstrstr   = cpp_ins_mstrstr;
    S.cpp_siz_vi16      = cpp_siz_vi16;
    S.cpp_get_str       = cpp_get_str;
    S.cpp_itr_vi32      = cpp_itr_vi32;
    S.cpp_itr_mstrstr   = cpp_itr_mstrstr;

    S.cpp_zer_str( S.vers );
    S.cpp_ins_str( S.host, S.cpp_get_str(host) );
    S.port          = port;
    S.handle        = -1;
    S.in_checkconn  = false;

    Comm    C;

    try {
        return C.checkConn( S );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_close( t_sglxconn &S )
{
    Comm    C;

    try {
        C.close( S );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_consoleHide( t_sglxconn &S )
{
    Comm    C;

    try {
        return C.doCommand( S, "CONSOLEHIDE" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_consoleShow( t_sglxconn &S )
{
    Comm    C;

    try {
        return C.doCommand( S, "CONSOLESHOW" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_enumDataDir(
    std::vector<std::string>    &vstr,
    t_sglxconn                  &S,
    int                         idir )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "ENUMDATADIR %d", idir );

    try {
        return C.doNLineQuery( vstr, S, cmd, false );
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_fetch(
    std::vector<short>      &data,
    t_sglxconn              &S,
    int                     js,
    int                     ip,
    t_ull                   start_samp,
    int                     max_samps,
    const std::vector<int>  &channel_subset,
    int                     downsample_ratio )
{
    Comm    C;
    string  s;
    char    cmd[32];

    try {
        C.checkConn( S );
        sprintf( cmd, "FETCH %d %d %llu %d ", js, ip, start_samp, max_samps );

        ostringstream   ss;
        int             val;
        void            *itr = 0;

        while( S.cpp_itr_vi32( val, channel_subset, itr ) )
            ss << val << "#";

        if( !ss.str().size() )
            ss << "-1#";

        ss << " " << downsample_ratio << "\n";

        C.sendString( S, cmd + ss.str() );
        C.read_1s_srvside( s, S );

        t_ull   headCt;
        int     nchans, nsamps;
        sscanf( s.c_str(), "BINARY_DATA %d %d uint64(%llu)", &nchans, &nsamps, &headCt );

        S.cpp_siz_vi16( data, nchans * nsamps );
        C.readBinary( &data[0], nchans * nsamps * sizeof(short), S );
        C.receiveOK( S, "FETCH" );

        if( !headCt )
            S.cpp_ins_str( S.err, "sglx_fetch: Stream not running." );

        return headCt;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_fetchLatest(
    std::vector<short>      &data,
    t_sglxconn              &S,
    int                     js,
    int                     ip,
    int                     max_samps,
    const std::vector<int>  &channel_subset,
    int                     downsample_ratio )
{
    t_ull   cur_samps = sglx_getStreamSampleCount( S, js, ip );

    if( !cur_samps )
        return 0;

    if( max_samps > cur_samps )
        max_samps = cur_samps;

    return sglx_fetch( data, S, js, ip,
            cur_samps - max_samps, max_samps,
            channel_subset, downsample_ratio );
}


SGLX_EXPORT bool SGLX_CALL sglx_getDataDir(
    std::string &dir,
    t_sglxconn  &S,
    int         idir )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "GETDATADIR %d", idir );

    try {
        return C.do1LineQuery( dir, S, cmd, false );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getImecChanGains(
    double      &APgain,
    double      &LFgain,
    t_sglxconn  &S,
    int         ip,
    int         chan )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETIMECCHANGAINS %d %d", ip, chan );

    try {
        C.do1LineQuery( s, S, cmd, true );

        if( 2 == sscanf( s.c_str(), "%lf %lf", &APgain, &LFgain ) )
            return true;
        else {
            S.cpp_ins_str( S.err, "sglx_getImecChanGains: Garbled gain string." );
            return false;
        }
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParams(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S )
{
    S.cpp_zer_mstrstr( mstrstr );

    Comm            C;
    vector<string>  vs;

    try {
        C.doNLineQuery( vs, S, "GETPARAMS", true );

        for( int i = 0, n = vs.size(); i < n; ++i ) {
            const string    &s = vs[i];
            int             eq = s.find( '=' );
            S.cpp_ins_mstrstr(
                mstrstr,
                s.substr( 0, eq ).c_str(),
                s.substr( eq + 1 ).c_str() );
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecCommon(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S )
{
    S.cpp_zer_mstrstr( mstrstr );

    Comm            C;
    vector<string>  vs;

    try {
        C.doNLineQuery( vs, S, "GETPARAMSIMALL", true );

        for( int i = 0, n = vs.size(); i < n; ++i ) {
            const string    &s = vs[i];
            int             eq = s.find( '=' );
            S.cpp_ins_mstrstr(
                mstrstr,
                s.substr( 0, eq ).c_str(),
                s.substr( eq + 1 ).c_str() );
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecProbe(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S,
    int                                 ip )
{
    S.cpp_zer_mstrstr( mstrstr );

    Comm            C;
    vector<string>  vs;
    char            cmd[32];
    sprintf( cmd, "GETPARAMSIMPRB %d", ip );

    try {
        C.doNLineQuery( vs, S, cmd, true );

        for( int i = 0, n = vs.size(); i < n; ++i ) {
            const string    &s = vs[i];
            int             eq = s.find( '=' );
            S.cpp_ins_mstrstr(
                mstrstr,
                s.substr( 0, eq ).c_str(),
                s.substr( eq + 1 ).c_str() );
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getParamsOneBox(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S,
    int                                 ip )
{
    S.cpp_zer_mstrstr( mstrstr );

    Comm            C;
    vector<string>  vs;
    char            cmd[32];
    sprintf( cmd, "GETPARAMSOBX %d", ip );

    try {
        C.doNLineQuery( vs, S, cmd, true );

        for( int i = 0, n = vs.size(); i < n; ++i ) {
            const string    &s = vs[i];
            int             eq = s.find( '=' );
            S.cpp_ins_mstrstr(
                mstrstr,
                s.substr( 0, eq ).c_str(),
                s.substr( eq + 1 ).c_str() );
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getRunName( std::string &name, t_sglxconn &S )
{
    Comm    C;

    try {
        return C.do1LineQuery( name, S, "GETRUNNAME", false );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamAcqChans(
    std::vector<int>    &vi32,
    t_sglxconn          &S,
    int                 js,
    int                 ip )
{
    S.cpp_zer_vi32( vi32 );

    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMACQCHANS %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        const char  *p = s.c_str(),
                    *L = p + strlen( p );

        while( p < L ) {
            int i, k;
            if( sscanf( p, "%d%n", &i, &k ) ) {
                S.cpp_ins_vi32( vi32, i );
                p += k + 1;
            }
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamFileStart(
    t_sglxconn  &S,
    int         js,
    int         ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMFILESTART %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        t_ull   start = stoull( s );

        if( !start )
            S.cpp_ins_str( S.err, "sglx_getStreamFileStart: Stream not running." );

        return start;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamI16ToVolts(
    double      &mult,
    t_sglxconn  &S,
    int         js,
    int         ip,
    int         chan )
{
    Comm    C;
    string  s;
    char    cmd[64];
    sprintf( cmd, "GETSTREAMI16TOVOLTS %d %d %d", js, ip, chan );

    try {
        mult = 0;
        C.do1LineQuery( s, S, cmd, true );
        mult = stod( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamMaxInt(
    int         &maxInt,
    t_sglxconn  &S,
    int         js,
    int         ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMMAXINT %d %d", js, ip );

    try {
        maxInt = 0;
        C.do1LineQuery( s, S, cmd, true );
        maxInt = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamNP( int &np, t_sglxconn &S, int js )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMNP %d", js );

    try {
        np = 0;
        C.do1LineQuery( s, S, cmd, true );
        np = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamSampleCount(
    t_sglxconn  &S,
    int         js,
    int         ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSAMPLECOUNT %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        t_ull   count = stoull( s );

        if( !count )
            S.cpp_ins_str( S.err, "sglx_getStreamSampleCount: Stream not running." );

        return count;
    }
    CATCH()
}


SGLX_EXPORT double SGLX_CALL sglx_getStreamSampleRate(
    t_sglxconn  &S,
    int         js,
    int         ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSAMPLERATE %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        double  rate = stod( s );

        if( !rate )
            S.cpp_ins_str( S.err, "sglx_getStreamSampleRate: Stream not running." );

        return rate;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamSaveChans(
    std::vector<int>    &vi32,
    t_sglxconn          &S,
    int                 js,
    int                 ip )
{
    S.cpp_zer_vi32( vi32 );

    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSAVECHANS %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        const char  *p = s.c_str(),
                    *L = p + strlen( p );

        while( p < L ) {
            int i, k;
            if( sscanf( p, "%d%n", &i, &k ) ) {
                S.cpp_ins_vi32( vi32, i );
                p += k + 1;
            }
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamShankMap(
    t_sglxshankmap  &shankmap,
    t_sglxconn      &S,
    int             js,
    int             ip )
{
    Comm    C;
    string  s;
    char    cmd[32];

    try {
        C.checkConn( S );
        sprintf( cmd, "GETSTREAMSHANKMAP %d %d\n", js, ip );
        C.sendString( S, cmd );
        C.read_1s_srvside( s, S );

        sscanf( s.c_str(), "SHANKMAP %d %d %d %d",
            &shankmap.ns, &shankmap.nc, &shankmap.nr, &shankmap.ne );

        S.cpp_siz_vi16( shankmap.e, 4*shankmap.ne );
        C.readBinary( &shankmap.e[0], 4*shankmap.ne * sizeof(short), S );
        C.receiveOK( S, "GETSTREAMSHANKMAP" );

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamSN(
    int             &slot_or_type,
    std::string     &SN,
    t_sglxconn      &S,
    int             js,
    int             ip )
{
    S.cpp_zer_str( SN );

    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMSN %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        char    sn[64];
        int     i = -1;

        if( 2 == sscanf( s.c_str(), "%[^ ] %d", sn, &i ) ) {
            S.cpp_ins_str( SN, sn );
            slot_or_type = i;
            return true;
        }
        else {
            S.cpp_ins_str( S.err, "sglx_getStreamSN: Garbled SN string." );
            return false;
        }
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getStreamVoltageRange(
    double      &vMin,
    double      &vMax,
    t_sglxconn  &S,
    int         js,
    int         ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "GETSTREAMVOLTAGERANGE %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        if( 2 == sscanf( s.c_str(), "%lf %lf", &vMin, &vMax ) )
            return true;
        else {
            S.cpp_ins_str( S.err, "sglx_getStreamVoltageRange: Garbled range string." );
            return false;
        }
    }
    CATCH()
}


SGLX_EXPORT double SGLX_CALL sglx_getTime( t_sglxconn &S )
{
    Comm    C;
    string  s;

    try {
        C.do1LineQuery( s, S, "GETTIME", true );
        return stod( s );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_getVersion( std::string &vers, t_sglxconn &S )
{
    Comm    C;

    try {
        C.checkConn( S );
        S.cpp_ins_str( vers, S.vers.c_str() );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isConsoleHidden( bool &hidden, t_sglxconn &S )
{
    Comm    C;
    string  s;

    try {
        C.do1LineQuery( s, S, "ISCONSOLEHIDDEN", true );
        hidden = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isInitialized( bool &ready, t_sglxconn &S )
{
    Comm    C;
    string  s;

    try {
        C.do1LineQuery( s, S, "ISINITIALIZED", true );
        ready = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isRunning( bool &running, t_sglxconn &S )
{
    Comm    C;
    string  s;

    try {
        C.do1LineQuery( s, S, "ISRUNNING", true );
        running = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isSaving( bool &saving, t_sglxconn &S )
{
    Comm    C;
    string  s;

    try {
        C.do1LineQuery( s, S, "ISSAVING", true );
        saving = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_isUserOrder(
    bool        &userOrder,
    t_sglxconn  &S,
    int         js,
    int         ip )
{
    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "ISUSRORDER %d %d", js, ip );

    try {
        C.do1LineQuery( s, S, cmd, true );
        userOrder = stoi( s );
        return true;
    }
    CATCH()
}


SGLX_EXPORT t_ull SGLX_CALL sglx_mapSample(
    t_sglxconn  &S,
    int         dstjs,
    int         dstip,
    t_ull       srcSample,
    int         srcjs,
    int         srcip )
{
    Comm    C;
    string  s;
    char    cmd[64];
    sprintf( cmd, "MAPSAMPLE %d %d %llu %d %d", dstjs, dstip, srcSample, srcjs, srcip );

    try {
        C.do1LineQuery( s, S, cmd, true );

        t_ull   dstSample = stoull( s );

        if( !dstSample )
            S.cpp_ins_str( S.err, "sglx_mapSample: Stream not running." );

        return dstSample;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_opto_emit(
    t_sglxconn  &S,
    int         ip,
    int         color,
    int         site )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "OPTOEMIT %d %d %d", ip, color, site );

    try {
        return C.doCommand( S, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_opto_getAttenuations(
    std::vector<double> &vdbl,
    t_sglxconn          &S,
    int                 ip,
    int                 color )
{
    S.cpp_zer_vdbl( vdbl );

    Comm    C;
    string  s;
    char    cmd[32];
    sprintf( cmd, "OPTOGETATTENS %d %d", ip, color );

    try {
        C.do1LineQuery( s, S, cmd, true );

        const char  *p = s.c_str(),
                    *L = p + strlen( p );

        while( p < L ) {
            double  d;
            int     k;
            if( sscanf( p, "%lf%n", &d, &k ) ) {
                S.cpp_ins_vdbl( vdbl, d );
                p += k + 1;
            }
        }

        return true;
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_par2(
    void                (SGLX_CALL *callback)(const char *status),
    t_sglxconn          &S,
    char                op,
    const std::string   &file )
{
    Comm    C;
    char    cmd[32];
    bool    running;

    if( !sglx_isRunning( running, S ) )
        return false;
    else if( running ) {
        S.cpp_ins_str( S.err, "sglx_par2: Can't be used during run." );
        return false;
    }

    try {
        C.checkConn( S );
        sprintf( cmd, "PAR2 %c ", op );
        C.sendString( S, string(cmd) + S.cpp_get_str(file) + "\n" );

        for(;;) {

            string      s;
            const char  *s0;
            C.read_1s_srvside( s, S );
            s0 = s.c_str();

            if( !strcmp( s0, "OK" ) )
                return true;

            if( callback )
                callback( s0 );
        }
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setAnatomy_Pinpoint(
    t_sglxconn          &S,
    const std::string   &shankdat )
{
    Comm    C;

    try {
        return C.doCommand( S, string("SETANATOMYPP ") + S.cpp_get_str(shankdat) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setAudioEnable( t_sglxconn &S, bool enable )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETAUDIOENABLE %d", enable );

    try {
        return C.doCommand( S, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setAudioParams(
    t_sglxconn                              &S,
    const std::string                       &group,
    const std::map<std::string,std::string> &mstrstr )
{
    Comm    C;

    try {
        C.checkConn( S );
        C.sendString( S, string("SETAUDIOPARAMS ") + S.cpp_get_str(group) + "\n" );
        C.receiveREADY( S, "SETAUDIOPARAMS" );

        const char  *key, *val;
        void        *itr = 0;

        while( S.cpp_itr_mstrstr( key, val, mstrstr, itr ) )
            C.sendString( S, string(key) + "=" + val + "\n" );

        C.sendString( S, "\n" );
        return C.receiveOK( S, "SETAUDIOPARAMS" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setDataDir(
    t_sglxconn          &S,
    int                 idir,
    const std::string   &dir )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETDATADIR %d ", idir );

    try {
        return C.doCommand( S, string(cmd) + S.cpp_get_str(dir) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setDigitalOut(
    t_sglxconn          &S,
    bool                hi_lo,
    const std::string   &channels )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETDIGOUT %d ", hi_lo );

    try {
        return C.doCommand( S, string(cmd) + S.cpp_get_str(channels) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setMetadata(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr )
{
    Comm    C;

    try {
        C.checkConn( S );
        C.sendString( S, "SETMETADATA\n" );
        C.receiveREADY( S, "SETMETADATA" );

        const char  *key, *val;
        void        *itr = 0;

        while( S.cpp_itr_mstrstr( key, val, mstrstr, itr ) )
            C.sendString( S, string(key) + "=" + val + "\n" );

        C.sendString( S, "\n" );
        return C.receiveOK( S, "SETMETADATA" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setMultiDriveEnable( t_sglxconn &S, bool enable )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETMULTIDRIVEENABLE %d", enable );

    try {
        return C.doCommand( S, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setNextFileName(
    t_sglxconn          &S,
    const std::string   &name )
{
    Comm    C;

    try {
        return C.doCommand( S, string("SETNEXTFILENAME ") + S.cpp_get_str(name) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParams(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr )
{
    Comm    C;

    try {
        C.checkConn( S );
        C.sendString( S, "SETPARAMS\n" );
        C.receiveREADY( S, "SETPARAMS" );

        const char  *key, *val;
        void        *itr = 0;

        while( S.cpp_itr_mstrstr( key, val, mstrstr, itr ) )
            C.sendString( S, string(key) + "=" + val + "\n" );

        C.sendString( S, "\n" );
        return C.receiveOK( S, "SETPARAMS" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecCommon(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr )
{
    Comm    C;

    try {
        C.checkConn( S );
        C.sendString( S, "SETPARAMSIMALL\n" );
        C.receiveREADY( S, "SETPARAMSIMALL" );

        const char  *key, *val;
        void        *itr = 0;

        while( S.cpp_itr_mstrstr( key, val, mstrstr, itr ) )
            C.sendString( S, string(key) + "=" + val + "\n" );

        C.sendString( S, "\n" );
        return C.receiveOK( S, "SETPARAMSIMALL" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecProbe(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr,
    int                                     ip )
{
    Comm    C;
    char    cmd[32];

    try {
        C.checkConn( S );
        sprintf( cmd, "SETPARAMSIMPRB %d\n", ip );
        C.sendString( S, cmd );
        C.receiveREADY( S, "SETPARAMSIMPRB" );

        const char  *key, *val;
        void        *itr = 0;

        while( S.cpp_itr_mstrstr( key, val, mstrstr, itr ) )
            C.sendString( S, string(key) + "=" + val + "\n" );

        C.sendString( S, "\n" );
        return C.receiveOK( S, "SETPARAMSIMPRB" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setParamsOneBox(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr,
    int                                     ip )
{
    Comm    C;
    char    cmd[32];

    try {
        C.checkConn( S );
        sprintf( cmd, "SETPARAMSOBX %d\n", ip );
        C.sendString( S, cmd );
        C.receiveREADY( S, "SETPARAMSOBX" );

        const char  *key, *val;
        void        *itr = 0;

        while( S.cpp_itr_mstrstr( key, val, mstrstr, itr ) )
            C.sendString( S, string(key) + "=" + val + "\n" );

        C.sendString( S, "\n" );
        return C.receiveOK( S, "SETPARAMSOBX" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setRecordingEnable( t_sglxconn &S, bool enable )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "SETRECORDENAB %d", enable );

    try {
        return C.doCommand( S, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setRunName(
    t_sglxconn          &S,
    const std::string   &name )
{
    Comm    C;

    try {
        return C.doCommand( S, string("SETRUNNAME ") + S.cpp_get_str(name) );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOffBeep(
    t_sglxconn          &S,
    int                 hertz,
    int                 millisec )
{
    Comm    C;
    char    cmd[64];
    sprintf( cmd, "SETTRIGGEROFFBEEP %d %d", hertz, millisec );

    try {
        return C.doCommand( S, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOnBeep(
    t_sglxconn          &S,
    int                 hertz,
    int                 millisec )
{
    Comm    C;
    char    cmd[64];
    sprintf( cmd, "SETTRIGGERONBEEP %d %d", hertz, millisec );

    try {
        return C.doCommand( S, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_startRun(
    t_sglxconn          &S,
    const std::string   &name )
{
    if( S.cpp_get_str(name)[0] && !sglx_setRunName( S, name ) )
        return false;

    Comm    C;

    try {
        return C.doCommand( S, "STARTRUN" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_stopRun( t_sglxconn &S )
{
    Comm    C;

    try {
        return C.doCommand( S, "STOPRUN" );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_triggerGT( t_sglxconn &S, int g, int t )
{
    Comm    C;
    char    cmd[32];
    sprintf( cmd, "TRIGGERGT %d %d", g, t );

    try {
        return C.doCommand( S, cmd );
    }
    CATCH()
}


SGLX_EXPORT bool SGLX_CALL sglx_verifySha1(
    void                (SGLX_CALL *callback)(const char *status),
    t_sglxconn          &S,
    const std::string   &filename )
{
    Comm    C;
    bool    running;

    if( !sglx_isRunning( running, S ) )
        return false;
    else {
        S.cpp_ins_str( S.err, "sglx_verifySha1: Can't be used during run." );
        return false;
    }

    try {
        C.checkConn( S );
        C.sendString( S, string("VERIFYSHA1 ") + S.cpp_get_str(filename) + "\n" );

        for(;;) {

            string      s;
            const char  *s0;
            C.read_1s_srvside( s, S );
            s0 = s.c_str();

            if( !strcmp( s0, "OK" ) )
                return true;

            if( callback )
                callback( s0 );
        }
    }
    CATCH()
}


