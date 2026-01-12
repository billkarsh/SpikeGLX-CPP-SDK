
#include "C_SglxApi.h"
#include "SglxCppClient.h"

#include <sstream>
using namespace std;


#define HS  reinterpret_cast<t_sglxconn*>(hSglx)




SGLX_EXPORT const char* SGLX_CALL c_sglx_getstr( int *len, void *hSglx, int ith )
{
    if( ith < HS->xvstr.size() ) {
        *len = HS->xvstr[ith].size();
        return HS->xvstr[ith].c_str();
    }

    *len = 0;
    return 0;
}


SGLX_EXPORT int SGLX_CALL c_sglx_getint( void *hSglx, int ith )
{
    if( ith < HS->xvint.size() )
        return HS->xvint[ith];

    return 0;
}


SGLX_EXPORT double SGLX_CALL c_sglx_getdbl( void *hSglx, int ith )
{
    if( ith < HS->xvdbl.size() )
        return HS->xvdbl[ith];

    return 0;
}


SGLX_EXPORT void SGLX_CALL c_sglx_setkv( void *hSglx, const char *key, const char *val )
{
    if( key && key[0] )
        HS->xmstrstr[key] = val;
    else
        HS->xmstrstr.clear();
}


SGLX_EXPORT void* SGLX_CALL c_sglx_createHandle()
{
    return sglx_createHandle_std();
}


SGLX_EXPORT void SGLX_CALL c_sglx_destroyHandle( void *hSglx )
{
    sglx_destroyHandle( hSglx );
}


SGLX_EXPORT const char* SGLX_CALL c_sglx_getError( void *hSglx )
{
    return sglx_getError( hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_connect(
    void        *hSglx,
    const char  *host,
    int         port )
{
    return sglx_connect( hSglx, host, port );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_close( void *hSglx )
{
    return sglx_close( hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_consoleHide( void *hSglx )
{
    return sglx_consoleHide( hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_consoleShow( void *hSglx )
{
    return sglx_consoleShow( hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_enumDataDir( int *nval, void *hSglx, int idir )
{
    *nval = 0;

    cClient_sglx_get_strs   vs( HS->xvstr );

    if( sglx_enumDataDir( vs, hSglx, idir ) ) {
        *nval = HS->xvstr.size();
        return true;
    }

    return false;
}


SGLX_EXPORT t_ull SGLX_CALL c_sglx_fetch(
    const short **data,
    int         *n_data,
    void        *hSglx,
    int         js,
    int         ip,
    t_ull       start_samp,
    int         max_samps,
    const int   *channel_subset,
    int         n_cs,
    int         downsample )
{
    HS->xvi16.clear();
    *data   = &HS->xvi16[0];
    *n_data = 0;

    cClient_sglx_fetch  io( HS->xvi16 );

    io.channel_subset   = channel_subset;
    io.n_cs             = n_cs;
    io.max_samps        = max_samps;
    io.downsample       = downsample;
    io.js               = js;
    io.ip               = ip;

    t_ull   headCt = sglx_fetch( io, hSglx, start_samp );

    if( headCt ) {
        *data   = &HS->xvi16[0];
        *n_data = HS->xvi16.size();
    }

    return headCt;
}


SGLX_EXPORT t_ull SGLX_CALL c_sglx_fetchLatest(
    const short **data,
    int         *n_data,
    void        *hSglx,
    int         js,
    int         ip,
    int         max_samps,
    const int   *channel_subset,
    int         n_cs,
    int         downsample )
{
    HS->xvi16.clear();
    *data   = &HS->xvi16[0];
    *n_data = 0;

    cClient_sglx_fetch  io( HS->xvi16 );

    io.channel_subset   = channel_subset;
    io.n_cs             = n_cs;
    io.max_samps        = max_samps;
    io.downsample       = downsample;
    io.js               = js;
    io.ip               = ip;

    t_ull   headCt = sglx_fetchLatest( io, hSglx );

    if( headCt ) {
        *data   = &HS->xvi16[0];
        *n_data = HS->xvi16.size();
    }

    return headCt;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getDataDir(
    const char  **dir,
    void        *hSglx,
    int         idir )
{
    HS->xstr.clear();
    *dir = HS->xstr.c_str();

    if( sglx_getDataDir( HS->xstr, hSglx, idir ) ) {
        *dir = HS->xstr.c_str();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getGeomMap(
    int     *nval,
    void    *hSglx,
    int     ip )
{
    *nval = 0;

    cClient_sglx_get_strs   kv( HS->xvstr );

    if( sglx_getGeomMap( kv, hSglx, ip ) ) {
        *nval = HS->xvstr.size();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getImecChanGains(
    double  *APgain,
    double  *LFgain,
    void    *hSglx,
    int     ip,
    int     chan )
{
    return sglx_getImecChanGains( *APgain, *LFgain, hSglx, ip, chan );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getLastGT( int *g, int *t, void *hSglx )
{
    return sglx_getLastGT( *g, *t, hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getNIShankMap( int *nval, void *hSglx )
{
    HS->xvstr.clear();
    *nval = 0;

    cppClient_sglxshankmap  SM;

    if( sglx_getStreamShankMap( SM, hSglx, 0, 0 ) ) {

        ostringstream   ss;
        int             ne = SM.ne;

        ss << SM.ns << " " << SM.nc << " " << SM.nr;
        HS->xvstr.push_back( ss.str() );

        for( int ie = 0; ie < ne; ++ie ) {
            const t_sglxshankmap::t_entry   &E = SM.e[ie];
            ss.str("");
            ss.clear();
            ss << E.s << " " << E.c << " " << E.r << " " << E.u;
            HS->xvstr.push_back( ss.str() );
        }

        *nval = 1 + ne;
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getParams( int *nval, void *hSglx )
{
    *nval = 0;

    cClient_sglx_get_strs   kv( HS->xvstr );

    if( sglx_getParams( kv, hSglx ) ) {
        *nval = HS->xvstr.size();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getParamsImecCommon( int *nval, void *hSglx )
{
    *nval = 0;

    cClient_sglx_get_strs   kv( HS->xvstr );

    if( sglx_getParamsImecCommon( kv, hSglx ) ) {
        *nval = HS->xvstr.size();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getParamsImecProbe(
    int     *nval,
    void    *hSglx,
    int     ip )
{
    *nval = 0;

    cClient_sglx_get_strs   kv( HS->xvstr );

    if( sglx_getParamsImecProbe( kv, hSglx, ip ) ) {
        *nval = HS->xvstr.size();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getParamsOneBox(
    int     *nval,
    void    *hSglx,
    int     ip,
    int     slot )
{
    *nval = 0;

    cClient_sglx_get_strs   kv( HS->xvstr );

    if( sglx_getParamsOneBox( kv, hSglx, ip, slot ) ) {
        *nval = HS->xvstr.size();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getProbeAddrs( const char **list, void *hSglx )
{
    HS->xstr.clear();
    *list = HS->xstr.c_str();

    if( sglx_getProbeAddrs( HS->xstr, hSglx ) ) {
        *list = HS->xstr.c_str();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getProbeList( const char **list, void *hSglx )
{
    HS->xstr.clear();
    *list = HS->xstr.c_str();

    if( sglx_getProbeList( HS->xstr, hSglx ) ) {
        *list = HS->xstr.c_str();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getRunName( const char **name, void *hSglx )
{
    HS->xstr.clear();
    *name = HS->xstr.c_str();

    if( sglx_getRunName( HS->xstr, hSglx ) ) {
        *name = HS->xstr.c_str();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamAcqChans(
    int     *nval,
    void    *hSglx,
    int     js,
    int     ip )
{
    *nval = 0;

    cClient_sglx_get_ints   vint( HS->xvint );

    if( sglx_getStreamAcqChans( vint, hSglx, js, ip ) ) {
        *nval = HS->xvint.size();
        return true;
    }

    return false;
}


SGLX_EXPORT t_ull SGLX_CALL c_sglx_getStreamFileStart(
    void    *hSglx,
    int     js,
    int     ip )
{
    return sglx_getStreamFileStart( hSglx, js, ip );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamI16ToVolts(
    double  *mult,
    void    *hSglx,
    int     js,
    int     ip,
    int     chan )
{
    return sglx_getStreamI16ToVolts( *mult, hSglx, js, ip, chan );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamMaxInt(
    int     *maxInt,
    void    *hSglx,
    int     js,
    int     ip )
{
    return sglx_getStreamMaxInt( *maxInt, hSglx, js, ip );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamNP( int *np, void *hSglx, int js )
{
    return sglx_getStreamNP( *np, hSglx, js );
}


SGLX_EXPORT t_ull SGLX_CALL c_sglx_getStreamSampleCount(
    void    *hSglx,
    int     js,
    int     ip )
{
    return sglx_getStreamSampleCount( hSglx, js, ip );
}


SGLX_EXPORT double SGLX_CALL c_sglx_getStreamSampleRate(
    void    *hSglx,
    int     js,
    int     ip )
{
    return sglx_getStreamSampleRate( hSglx, js, ip );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamSaveChans(
    int     *nval,
    void    *hSglx,
    int     js,
    int     ip )
{
    *nval = 0;

    cClient_sglx_get_ints   vint( HS->xvint );

    if( sglx_getStreamSaveChans( vint, hSglx, js, ip ) ) {
        *nval = HS->xvint.size();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamSN(
    int         *slot_or_type,
    const char  **SN,
    void        *hSglx,
    int         js,
    int         ip )
{
    HS->xstr.clear();
    *SN = HS->xstr.c_str();

    if( sglx_getStreamSN( *slot_or_type, HS->xstr, hSglx, js, ip ) ) {
        *SN = HS->xstr.c_str();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamVoltageRange(
    double  *vMin,
    double  *vMax,
    void    *hSglx,
    int     js,
    int     ip )
{
    return sglx_getStreamVoltageRange( *vMin, *vMax, hSglx, js, ip );
}


SGLX_EXPORT double SGLX_CALL c_sglx_getTime( void *hSglx )
{
    return sglx_getTime( hSglx );
}


SGLX_EXPORT const char* SGLX_CALL c_sglx_getVersion( void *hSglx )
{
    return sglx_getVersion( hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_isConsoleHidden( bool *hidden, void *hSglx )
{
    return sglx_isConsoleHidden( *hidden, hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_isInitialized( bool *ready, void *hSglx )
{
    return sglx_isInitialized( *ready, hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_isRunning( bool *running, void *hSglx )
{
    return sglx_isRunning( *running, hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_isSaving( bool *saving, void *hSglx )
{
    return sglx_isSaving( *saving, hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_isUserOrder(
    bool    *userOrder,
    void    *hSglx,
    int     js,
    int     ip )
{
    return sglx_isUserOrder( *userOrder, hSglx, js, ip );
}


SGLX_EXPORT t_ull SGLX_CALL c_sglx_mapSample(
    void    *hSglx,
    int     dstjs,
    int     dstip,
    t_ull   srcSample,
    int     srcjs,
    int     srcip )
{
    return sglx_mapSample( hSglx, dstjs, dstip, srcSample, srcjs, srcip );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_ni_DO_set(
    void            *hSglx,
    const char      *lines,
    unsigned int    bits )
{
    return sglx_ni_DO_set( hSglx, lines, bits );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_ni_wave_arm(
    void        *hSglx,
    const char  *outChan,
    const char  *trigTerm )
{
    return sglx_ni_wave_arm( hSglx, outChan, trigTerm );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_ni_wave_load(
    void        *hSglx,
    const char  *outChan,
    const char  *wave,
    bool        loop )
{
    return sglx_ni_wave_load( hSglx, outChan, wave, loop );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_ni_wave_startstop(
    void        *hSglx,
    const char  *outChan,
    bool        start )
{
    return sglx_ni_wave_startstop( hSglx, outChan, start );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_obx_AO_set(
    void        *hSglx,
    int         ip,
    int         slot,
    const char  *chn_vlt )
{
    return sglx_obx_AO_set( hSglx, ip, slot, chn_vlt );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_obx_wave_arm(
    void    *hSglx,
    int     ip,
    int     slot,
    int     trig,
    bool    loop )
{
    return sglx_obx_wave_arm( hSglx, ip, slot, trig, loop );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_obx_wave_load(
    void        *hSglx,
    int         ip,
    int         slot,
    const char  *wave )
{
    return sglx_obx_wave_load( hSglx, ip, slot, wave );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_obx_wave_startstop(
    void    *hSglx,
    int     ip,
    int     slot,
    bool    start )
{
    return sglx_obx_wave_startstop( hSglx, ip, slot, start );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_opto_emit(
    void    *hSglx,
    int     ip,
    int     color,
    int     site )
{
    return sglx_opto_emit( hSglx, ip, color, site );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_opto_getAttenuations(
    int     *nval,
    void    *hSglx,
    int     ip,
    int     color )
{
    *nval = 0;

    cClient_sglx_get_dbls   vdbl( HS->xvdbl );

    if( sglx_opto_getAttenuations( vdbl, hSglx, ip, color ) ) {
        *nval = HS->xvdbl.size();
        return true;
    }

    return false;
}


SGLX_EXPORT bool SGLX_CALL c_sglx_par2(
    void        (SGLX_CALL *callback)(const char *status),
    void        *hSglx,
    char        op,
    const char  *file )
{
    return sglx_par2( callback, hSglx, op, file );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_pause_graphs( void *hSglx, bool pause )
{
    return sglx_pause_graphs( hSglx, pause );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setAnatomy_Pinpoint(
    void        *hSglx,
    const char  *shankdat )
{
    return sglx_setAnatomy_Pinpoint( hSglx, shankdat );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setAudioEnable( void *hSglx, bool enable )
{
    return sglx_setAudioEnable( hSglx, enable );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setAudioKVParams(
    void        *hSglx,
    const char  *group )
{
    cClient_sglx_set_keyvals    kv( HS->xmstrstr );

    return sglx_setAudioParams( hSglx, group, kv );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setDataDir(
    void        *hSglx,
    int         idir,
    const char  *dir )
{
    return sglx_setDataDir( hSglx, idir, dir );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setKVMetadata( void *hSglx )
{
    cClient_sglx_set_keyvals    kv( HS->xmstrstr );

    return sglx_setMetadata( hSglx, kv );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParams( void *hSglx )
{
    cClient_sglx_set_keyvals    kv( HS->xmstrstr );

    return sglx_setParams( hSglx, kv );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParamsImecCommon( void *hSglx )
{
    cClient_sglx_set_keyvals    kv( HS->xmstrstr );

    return sglx_setParamsImecCommon( hSglx, kv );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParamsImecProbe( void *hSglx, int ip )
{
    cClient_sglx_set_keyvals    kv( HS->xmstrstr );

    return sglx_setParamsImecProbe( hSglx, kv, ip );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParamsOneBox( void *hSglx, int ip, int slot )
{
    cClient_sglx_set_keyvals    kv( HS->xmstrstr );

    return sglx_setParamsOneBox( hSglx, kv, ip, slot );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setMultiDriveEnable( void *hSglx, bool enable )
{
    return sglx_setMultiDriveEnable( hSglx, enable );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setNextFileName( void *hSglx, const char *name )
{
    return sglx_setNextFileName( hSglx, name );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setRecordingEnable( void *hSglx, bool enable )
{
    return sglx_setRecordingEnable( hSglx, enable );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setRunName( void *hSglx, const char *name )
{
    return sglx_setRunName( hSglx, name );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setTriggerOffBeep(
    void    *hSglx,
    int     hertz,
    int     millisec )
{
    return sglx_setTriggerOffBeep( hSglx, hertz, millisec );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_setTriggerOnBeep(
    void    *hSglx,
    int     hertz,
    int     millisec )
{
    return sglx_setTriggerOnBeep( hSglx, hertz, millisec );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_startRun( void *hSglx, const char *name )
{
    string _name;

    if( name && name[0] )
        _name = name;

    return sglx_startRun( hSglx, _name );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_stopRun( void *hSglx )
{
    return sglx_stopRun( hSglx );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_triggerGT( void *hSglx, int g, int t )
{
    return sglx_triggerGT( hSglx, g, t );
}


SGLX_EXPORT bool SGLX_CALL c_sglx_verifySha1(
    void        (SGLX_CALL *callback)(const char *status),
    void        *hSglx,
    const char  *filename )
{
    return sglx_verifySha1( callback, hSglx, filename );
}


