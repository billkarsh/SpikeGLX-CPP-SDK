#ifndef SGLXAPI_H
#define SGLXAPI_H

/* ---------------------------------------------------------------- */
/* C++ API -------------------------------------------------------- */
/* ---------------------------------------------------------------- */

#include <map>
#include <string>
#include <vector>

#if defined(SGLXAPI_LIBRARY)
#  define SGLX_EXPORT __declspec(dllexport)
#else
#  define SGLX_EXPORT __declspec(dllimport)
#endif

#define SGLX_CALL   __stdcall

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------- */
/* Client-side container callbacks -------------------------------- */
/* ---------------------------------------------------------------- */

// clear client container
typedef void (SGLX_CALL *Tcpp_zer_str)( std::string &str );

// insert into client container
typedef void (SGLX_CALL *Tcpp_set_str)( std::string &str, const char *data );

// get C-string from client container
typedef const char* (SGLX_CALL *Tcpp_get_str)( const std::string &str );

/* ---------------------------------------------------------------- */
/* Client-side functors ------------------------------------------- */
/* ---------------------------------------------------------------- */

// For functions that get (API -> client) an array of strings.
//
// This API base class defines a method to give, to the client,
// one string at a time. The data are delivered as C-strings.
// The client's subclass can do whatever it wants with received
// strings. If the client is seeking (key,value) pairs, which have
// the form 'key=value', then the client may elect to split the
// data into substrings.
// - API calls set_str( "" ) to clear client data.
// - API calls set_str( string ) to set each pair.
//
// Clients can maintain data in their derived class
// in any desired form. Our examples use std::vector
// for whole strings, and std::map for (key,value).
//
struct T_sglx_get_strs {
    T_sglx_get_strs();
    virtual ~T_sglx_get_strs()  {}
    virtual void set_str( const char *s ) = 0;
    static void SGLX_CALL dispatch( T_sglx_get_strs &u, const char *s );
    void (SGLX_CALL *_dispatch)( T_sglx_get_strs &u, const char *s );
};

// For functions that get (API -> client) an array of ints.
//
// This API base class defines a method to give, to the client,
// one int at a time.
// - API calls set_val( -, 0 ) to clear client data.
// - API calls set_val( v, 1 ) to set each val.
//
// Clients can maintain data in their derived class
// in any desired form. Our example uses std::vector.
//
struct T_sglx_get_ints {
    T_sglx_get_ints();
    virtual ~T_sglx_get_ints()  {}
    virtual void set_val( int val, int flag ) = 0;
    static void SGLX_CALL dispatch( T_sglx_get_ints &u, int val, int flag );
    void (SGLX_CALL *_dispatch)( T_sglx_get_ints &u, int val, int flag );
};

// For functions that get (API -> client) an array of doubles.
//
// This API base class defines a method to give, to the client,
// one double at a time.
// - API calls set_val( -, 0 ) to clear client data.
// - API calls set_val( v, 1 ) to set each val.
//
// Clients can maintain data in their derived class
// in any desired form. Our example uses std::vector.
//
struct T_sglx_get_dbls {
    T_sglx_get_dbls();
    virtual ~T_sglx_get_dbls()  {}
    virtual void set_val( double val, int flag ) = 0;
    static void SGLX_CALL dispatch( T_sglx_get_dbls &u, double val, int flag );
    void (SGLX_CALL *_dispatch)( T_sglx_get_dbls &u, double val, int flag );
};

// For functions that set (client -> API) an array of int32.
//
// This API base class defines a method to get, from the client,
// one int at a time:
// - API calls get_int( j ) to get jth.
// - Return true if int exists.
//
// Clients can maintain data in their derived class
// in any desired form. Our example uses std::vector.
//
struct T_sglx_set_ints {
    T_sglx_set_ints();
    virtual ~T_sglx_set_ints()  {}
    virtual bool get_int( int &val, int j ) = 0;
    static bool SGLX_CALL dispatch( T_sglx_set_ints &u, int &val, int j );
    bool (SGLX_CALL *_dispatch)( T_sglx_set_ints &u, int &val, int j );
};

// For functions that set (client -> API) (key,value) pairs.
//
// This API base class defines a method to get, from the client,
// one (key,value) pair at a time:
// - API calls get_pair( 0, 0 ) to get first pair.
// - API calls get_pair( key, val ) to get next pair.
// - Return true if pair exists.
//
// Clients can maintain (key,value) pairs in their derived class
// in any desired form. Our example uses std::map.
//
struct T_sglx_set_keyvals {
    T_sglx_set_keyvals();
    virtual ~T_sglx_set_keyvals()   {}
    virtual bool get_pair( const char* &key, const char* &val ) = 0;
    static bool SGLX_CALL dispatch( T_sglx_set_keyvals &u, const char* &key, const char* &val );
    bool (SGLX_CALL *_dispatch)( T_sglx_set_keyvals &u, const char* &key, const char* &val );
};

// For use with the data fetching functions.
//
// On entry to a fetch the client will have set each of the input fields.
// - channel_subset is an array of specific channels to fetch, optionally,
//      channel_subset[0] = -1 = all acquired channels, or,
//      channel_subset[0] = -2 = all saved channels.
// - n_cs       = number of channels in subset.
// - Samp count = MIN(max_samps,available).
// - Fetch every (downsample)th sample (1 = all samples).
// - (js, ip)   = stream selectors.
//
// On exit:
// The API calls base_addr during the fetch, whereupon the the client
// should make sure storage is allocated for (nshort) items and the base
// address of that storage is returned. See sglx_fetch.
//
struct T_sglx_fetch {
    // client input
    const int   *channel_subset;
    int         n_cs,
                max_samps,
                downsample;
    short       js,
                ip;
    T_sglx_fetch();
    virtual ~T_sglx_fetch()  {}
    virtual short* base_addr( int nshort ) = 0;
    static short* SGLX_CALL dispatch( T_sglx_fetch &u, int nshort );
    short* (SGLX_CALL *_dispatch)( T_sglx_fetch &u, int nshort );
};

// For functions that get (API -> client) a shankMap.
//
// This API base class defines a method to give, to the client,
// all of the packed entries in one call.
// - First, API sets the header values {ne, ns, nc, nr}.
// - Then API calls base_addr(), whereupon, the client should
// allocate storage of size t_entry[ne] and return the base
// address of that storage.
//
// Clients can maintain data in their derived class
// in any desired form. Our example uses std::vector.
//
struct t_sglxshankmap {
    struct t_entry {
        // shank, col, row, used
        short s, c, r, u;
    };
    // Header: N {entries, shanks, cols, rows}
    int ne, ns, nc, nr;
    t_sglxshankmap();
    virtual ~t_sglxshankmap()   {}
    virtual t_entry* base_addr() = 0;
    static t_entry* SGLX_CALL dispatch( t_sglxshankmap &u );
    t_entry* (SGLX_CALL *_dispatch)( t_sglxshankmap &u );
};

/* ---------------------------------------------------------------- */
/* Types ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

typedef unsigned long long t_ull;

/* ---------------------------------------------------------------- */
/* Private server-side handle ------------------------------------- */
/* ---------------------------------------------------------------- */

struct t_sglxconn {
    // C++ client container helpers
    Tcpp_zer_str        cpp_zer_str;
    Tcpp_set_str        cpp_set_str;
    Tcpp_get_str        cpp_get_str;

    // Connection state
    std::string         err,
                        vers,
                        host;
    int                 port,
                        handle;
    bool                in_checkconn;

    // C client data storage
    std::string                         xstr;
    std::vector<int>                    xvint;
    std::vector<short>                  xvi16;
    std::vector<double>                 xvdbl;
    std::vector<std::string>            xvstr;
    std::map<std::string,std::string>   xmstrstr;
};

/* ---------------------------------------------------------------- */
/* Usage ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

// A client application first creates a connection handle with either:
// - sglx_createHandle()        (your custom container callbacks), or,
// - sglx_createHandle_std()    (installs our client-side callbacks).
//
// The handle can then be passed to any of the API functions.
// When done, the client should call
// - sglx_close(), and,
// - sglx_destroyHandle().
//
// Almost all functions return either true=SUCCESS, or false=FAIL.
// Get the reason for the most recent failure with sglx_getError().

/* ---------------------------------------------------------------- */
/* Functions ------------------------------------------------------ */
/* ---------------------------------------------------------------- */

// Create a new connection handle to be passed to sglx_connect()
// and to all subsequent API calls. This call allocates memory
// owned by the DLL. Release it with sglx_destroyHandle() when
// you are completely done with the connection.
//
// The returned handle is an opaque (void*).
//
SGLX_EXPORT void* SGLX_CALL sglx_createHandle(
    Tcpp_zer_str        cpp_zer_str,
    Tcpp_set_str        cpp_set_str,
    Tcpp_get_str        cpp_get_str );

// Destroy handle and release memory resources.
//
SGLX_EXPORT void SGLX_CALL sglx_destroyHandle( void *hSglx );

// Get latest error message.
//
SGLX_EXPORT const char* SGLX_CALL sglx_getError( void *hSglx );

// Connect to SpikeGLX server.
//
// Client containers must be accessed by the client (calling app).
// Recommended code for these methods is provided in SglxCppClient.cpp.
//
SGLX_EXPORT bool SGLX_CALL sglx_connect(
    void        *hSglx,
    const char  *host   = "localhost",
    int         port    = 4142 );

// Close connection and release network resources.
//
SGLX_EXPORT bool SGLX_CALL sglx_close( void *hSglx );

// Hide console/log window to reduce screen clutter.
//
SGLX_EXPORT bool SGLX_CALL sglx_consoleHide( void *hSglx );

// Show console/log window.
//
SGLX_EXPORT bool SGLX_CALL sglx_consoleShow( void *hSglx );

// Retrieve a listing of files in idir data directory.
// Get main data directory by setting idir=0 or omitting it.
//
SGLX_EXPORT bool SGLX_CALL sglx_enumDataDir(
    T_sglx_get_strs     &vs,
    void                *hSglx,
    int                 idir = 0 );

// Get binary stream data as linear array.
// Samp count = MIN(max_samps,available).
// Each sample contains N 16-bit channels, N depends upon channel_subset.
// Fetching starts at index start_samp.
// channel_subset is an array of specific channels to fetch, optionally,
//      -1 = all acquired channels, or,
//      -2 = all saved channels.
// Data are int16 type.
// downsample is an integer (default = 1).
//
// Return headCt = index of first sample, or zero if error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_fetch( T_sglx_fetch &io, void *hSglx, t_ull start_samp );

// Get binary stream data as linear array.
// Samp count = MIN(max_samps,available).
// Each sample contains N 16-bit channels, N depends upon channel_subset.
// channel_subset is an array of specific channels to fetch, optionally,
//      -1 = all acquired channels, or,
//      -2 = all saved channels.
// Data are int16 type.
// downsample is an integer (default = 1).
//
// Return headCt = index of first sample, or zero if error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_fetchLatest( T_sglx_fetch &io, void *hSglx );

// Get ith global data directory.
// Get main data directory by setting idir=0 or omitting it.
//
SGLX_EXPORT bool SGLX_CALL sglx_getDataDir(
    std::string &dir,
    void        *hSglx,
    int         idir = 0 );

// Get geomMap for given logical imec probe.
// Returned as (key,value) pairs.
// Header fields:
//     head_partNumber
//     head_numShanks
//     head_shankPitch   ; microns
//     head_shankWidth   ; microns
// Channel 5, e.g.:
//     ch5_s   ; shank index
//     ch5_x   ; microns from left edge of shank
//     ch5_z   ; microns from center of tip-most electrode row
//     ch5_u   ; used-flag (in CAR operations)
//
//  Note: Fields are in ascending alphanumeric order!
//
SGLX_EXPORT bool SGLX_CALL sglx_getGeomMap(
    T_sglx_get_strs     &kv,
    void                *hSglx,
    int                 ip );

// Get gains for given probe and channel.
//
SGLX_EXPORT bool SGLX_CALL sglx_getImecChanGains(
    double  &APgain,
    double  &LFgain,
    void    *hSglx,
    int     ip,
    int     chan );

// Get the most recently used run parameters.
// These are (key,value) pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParams(
    T_sglx_get_strs     &kv,
    void                *hSglx );

// Get imec parameters common to all enabled probes.
// These are (key,value) pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecCommon(
    T_sglx_get_strs     &kv,
    void                *hSglx );

// Get imec parameters for given logical probe.
// These are (key,value) pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecProbe(
    T_sglx_get_strs     &kv,
    void                *hSglx,
    int                 ip );

// Get parameters for given logical OneBox.
// These are (key,value) pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParamsOneBox(
    T_sglx_get_strs     &kv,
    void                *hSglx,
    int                 ip );

// Get string with format:
// (probeID,nShanks,partNumber)()...
// - A parenthesized entry for each selected probe.
// - probeID: zero-based integer.
// - nShanks: integer {1,4}.
// - partNumber: string, e.g., NP1000.
// - If no probes, return '()'.
//
SGLX_EXPORT bool SGLX_CALL sglx_getProbeList( std::string &list, void *hSglx );

// Get run base name.
//
SGLX_EXPORT bool SGLX_CALL sglx_getRunName( std::string &name, void *hSglx );

// For the selected substream, return an array of the
// number of channels of each type that stream is acquiring.
//
// js = 0: NI channels: {MN,MA,XA,DW}.
// js = 1: OB channels: {XA,DW,SY}.
// js = 2: IM channels: {AP,LF,SY}.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamAcqChans(
    T_sglx_get_ints     &vint,
    void                *hSglx,
    int                 js,
    int                 ip );

// Return index of first sample in selected stream file,
// or zero if unavailable.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamFileStart(
    void    *hSglx,
    int     js,
    int     ip );

// Return multiplier converting 16-bit binary channel to volts.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamI16ToVolts(
    double  &mult,
    void    *hSglx,
    int     js,
    int     ip,
    int     chan );

// Return largest positive integer value for selected stream.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamMaxInt(
    int     &maxInt,
    void    *hSglx,
    int     js,
    int     ip );

// Get number (np) of js-type substreams.
// For the given js, ip has range [0..np-1].
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamNP( int &np, void *hSglx, int js );

// Return number of samples since current run started,
// or zero if not running or error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamSampleCount(
    void    *hSglx,
    int     js,
    int     ip );

// Return sample rate of selected stream in Hz, or zero if error.
//
SGLX_EXPORT double SGLX_CALL sglx_getStreamSampleRate(
    void    *hSglx,
    int     js,
    int     ip );

// Get an array containing the indices of
// the acquired channels that are being saved.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamSaveChans(
    T_sglx_get_ints     &vint,
    void                *hSglx,
    int                 js,
    int                 ip );

// Get t_sglxshankmap struct.
// Only supported for NI stream (js = 0).
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamShankMap(
    t_sglxshankmap  &shankmap,
    void            *hSglx,
    int             js,
    int             ip );

// js = 1: Get OneBox SN and slot.
// js = 2: Get probe  SN and type.
// SN = serial number string.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamSN(
    int             &slot_or_type,
    std::string     &SN,
    void            *hSglx,
    int             js,
    int             ip );

// Get voltage range of selected data stream.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamVoltageRange(
    double  &vMin,
    double  &vMax,
    void    *hSglx,
    int     js,
    int     ip );

// Return number of seconds since SpikeGLX application was launched,
// or zero if error.
//
SGLX_EXPORT double SGLX_CALL sglx_getTime( void *hSglx );

// Get SpikeGLX version string.
//
SGLX_EXPORT const char* SGLX_CALL sglx_getVersion( void *hSglx );

// Test if console window is hidden.
//
SGLX_EXPORT bool SGLX_CALL sglx_isConsoleHidden( bool &hidden, void *hSglx );

// Test if SpikeGLX has completed its startup initialization
// and is ready to run.
//
SGLX_EXPORT bool SGLX_CALL sglx_isInitialized( bool &ready, void *hSglx );

// Test if SpikeGLX is currently acquiring data.
//
SGLX_EXPORT bool SGLX_CALL sglx_isRunning( bool &running, void *hSglx );

// Test if SpikeGLX is currently running AND saving data.
//
SGLX_EXPORT bool SGLX_CALL sglx_isSaving( bool &saving, void *hSglx );

// Test if graphs currently sorted in user order.
// This query is sent only to the main Graphs window.
//
SGLX_EXPORT bool SGLX_CALL sglx_isUserOrder(
    bool    &userOrder,
    void    *hSglx,
    int     js,
    int     ip );

// Return sample in dst stream corresponding to given sample in src stream,
// or zero if error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_mapSample(
    void    *hSglx,
    int     dstjs,
    int     dstip,
    t_ull   srcSample,
    int     srcjs,
    int     srcip );

// Direct emission to specified site (-1=dark).
// ip:    imec probe index.
// color: {0=blue, 1=red}.
// site:  [0..13], or, -1=dark.
//
SGLX_EXPORT bool SGLX_CALL sglx_opto_emit(
    void    *hSglx,
    int     ip,
    int     color,
    int     site );

// Get array of 14 (double) site power attenuation factors.
// ip:    imec probe index.
// color: {0=blue, 1=red}.
//
SGLX_EXPORT bool SGLX_CALL sglx_opto_getAttenuations(
    T_sglx_get_dbls     &vdbl,
    void                *hSglx,
    int                 ip,
    int                 color );

// Create, verify, or repair Par2 redundancy files for 'file'.
//
// Arguments:
// op:      {'c'=create, 'v'=verify, 'r'=repair}.
// file:    Path to a .par2 or .bin file to which 'op' is applied.
//
// Status/progress lines are reported to optional callback.
//
SGLX_EXPORT bool SGLX_CALL sglx_par2(
    void                (SGLX_CALL *callback)(const char *status),
    void                *hSglx,
    char                op,
    const std::string   &file );

// Set anatomy data string with Pinpoint format:
// [probe-id,shank-id](startpos,endpos,R,G,B,rgnname)(startpos,endpos,R,G,B,rgnname)…()
//    - probe-id: SpikeGLX logical probe id.
//    - shank-id: [0..n-shanks].
//    - startpos: region start in microns from tip.
//    - endpos:   region end in microns from tip.
//    - R,G,B:    region color as RGB, each [0..255].
//    - rgnname:  region name text.
//
SGLX_EXPORT bool SGLX_CALL sglx_setAnatomy_Pinpoint(
    void                *hSglx,
    const std::string   &shankdat );

// Set audio output on/off. Note that this command has
// no effect if not currently running.
//
SGLX_EXPORT bool SGLX_CALL sglx_setAudioEnable( void *hSglx, bool enable );

// Set subgroup of parameters for audio-out operation. This call
// stops current output. Call sglx_setAudioEnable() to restart it.
//
SGLX_EXPORT bool SGLX_CALL sglx_setAudioParams(
    void                    *hSglx,
    const std::string       &group,
    T_sglx_set_keyvals      &kv );

// Set ith global data directory.
// Set required parameter idir to zero for main data directory.
//
SGLX_EXPORT bool SGLX_CALL sglx_setDataDir(
    void                *hSglx,
    int                 idir,
    const std::string   &dir );

// Set digital output high/low. Channel strings have form:
// "Dev6/port0/line2,Dev6/port0/line5".
//
SGLX_EXPORT bool SGLX_CALL sglx_setDigitalOut(
    void                *hSglx,
    bool                hi_lo,
    const std::string   &channels );

// If a run is in progress, set metadata to be added to the
// next output file-set.
//
SGLX_EXPORT bool SGLX_CALL sglx_setMetadata(
    void                    *hSglx,
    T_sglx_set_keyvals      &kv );

// Set multi-drive run-splitting on/off.
//
SGLX_EXPORT bool SGLX_CALL sglx_setMultiDriveEnable( void *hSglx, bool enable );

// For only the next trigger (file writing event) this overrides
// all auto-naming, giving you complete control of where to save
// the files, the file name, and what g- and t-indices you want
// (if any). For example, regardless of the run's current data dir,
// run name and indices, if you set: 'otherdir/yyy_g5/yyy_g5_t7',
// SpikeGLX will save the next files in flat directory yyy_g5/:
//    - otherdir/yyy_g5/yyy.g5_t7.nidq.bin,meta
//    - otherdir/yyy_g5/yyy.g5_t7.imec0.ap.bin,meta
//    - otherdir/yyy_g5/yyy.g5_t7.imec0.lf.bin,meta
//    - otherdir/yyy_g5/yyy.g5_t7.imec1.ap.bin,meta
//    - otherdir/yyy_g5/yyy.g5_t7.imec1.lf.bin,meta
//    - etc.
//
// - The destination directory must already exist...No parent directories
// or probe subfolders are created in this naming mode.
// - The run must already be in progress.
// - Neither the custom name nor its indices are displayed in the Graphs
// window toolbars. Rather, the toolbars reflect standard auto-names.
// - After writing this file set, the override is cleared and auto-naming
// will resume as if you never called sglx_setNextFileName. You have to call
// sglx_setNextFileName before each trigger event to create custom trial series.
// For example, you can build a software-triggered t-series using sequence:
//    + sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t0' )
//    + sglx_setRecordingEnable( 1 )
//    + sglx_setRecordingEnable( 0 )
//    + sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t1' )
//    + sglx_setRecordingEnable( 1 )
//    + sglx_setRecordingEnable( 0 )
//    + etc.
//
SGLX_EXPORT bool SGLX_CALL sglx_setNextFileName(
    void                *hSglx,
    const std::string   &name );

// The inverse of sglx_getParams, this sets run parameters.
// The call will error if a run is currently in progress.
//
// Note: You can set any subset of [DAQSettings].
//
SGLX_EXPORT bool SGLX_CALL sglx_setParams(
    void                    *hSglx,
    T_sglx_set_keyvals      &kv );

// The inverse of sglx_getParamsImecCommon, this sets parameters
// common to all enabled probes. The call will error if a run is
// currently in progress.
//
// Note: You can set any subset of [DAQ_Imec_All].
//
SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecCommon(
    void                    *hSglx,
    T_sglx_set_keyvals      &kv );

// The inverse of sglx_getParamsImecProbe, this sets parameters
// for a given logical probe. The call will error if file writing
// is currently in progress.
//
// Note: You can set any subset of fields under [SerialNumberToProbe]/SNjjj.
//
SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecProbe(
    void                    *hSglx,
    T_sglx_set_keyvals      &kv,
    int                     ip );

// The inverse of sglx_getParamsOneBox, this sets parameters
// for a given logical OneBox. The call will error if a run is
// currently in progress.
//
// Note: You can set any subset of fields under [SerialNumberToOneBox]/SNjjj.
//
SGLX_EXPORT bool SGLX_CALL sglx_setParamsOneBox(
    void                    *hSglx,
    T_sglx_set_keyvals      &kv,
    int                     ip );

// Set gate (file writing) on/off during run.
//
// When auto-naming is in effect, opening the gate advances
// the g-index and resets the t-index to zero. Auto-naming is
// on unless sglx_setNextFileName has been used to override it.
//
SGLX_EXPORT bool SGLX_CALL sglx_setRecordingEnable( void *hSglx, bool enable );

// Set the run name for the next time files are created
// (either by trigger, sglx_setRecordingEnable() or by sglx_startRun()).
//
SGLX_EXPORT bool SGLX_CALL sglx_setRunName(
    void                *hSglx,
    const std::string   &name );

// During a run, set frequency and duration of Windows
// beep signaling file closure. hertz=0 disables the beep.
//
SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOffBeep(
    void    *hSglx,
    int     hertz,
    int     millisec );

// During a run set frequency and duration of Windows
// beep signaling file creation. hertz=0 disables the beep.
//
SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOnBeep(
    void    *hSglx,
    int     hertz,
    int     millisec );

// Start data acquisition run. Last-used parameters remain
// in effect. An error is flagged if already running. The
// name parameter is optional; set NULL or "" to use existing.
//
SGLX_EXPORT bool SGLX_CALL sglx_startRun(
    void                *hSglx,
    const std::string   &name = "" );

// Unconditionally stop current run, close data files
// and return to idle state.
//
SGLX_EXPORT bool SGLX_CALL sglx_stopRun( void *hSglx );

// Using standard auto-naming, set both the gate (g) and
// trigger (t) levels that control file writing.
//   -1 = no change.
//    0 = set low.
//    1 = increment and set high.
// E.g., sglx_triggerGT( -1, 1 ) = same g, increment t, start writing.
//
// - sglx_triggerGT only affects the 'Remote controlled' gate type
// and/or the 'Remote controlled' trigger type.
// - The 'Enable Recording' button, when shown, is a master override
// switch. sglx_triggerGT is blocked until you click the button or call
// sglx_setRecordingEnable.
//
SGLX_EXPORT bool SGLX_CALL sglx_triggerGT( void *hSglx, int g, int t );

// Verifies the SHA1 sum of the file specified by filename.
// If filename is relative, it is appended to the run dir.
// Absolute path/filenames are also supported. Status/progress
// lines are reported to optional callback.
//
// Return true if verified (and no errors).
//
SGLX_EXPORT bool SGLX_CALL sglx_verifySha1(
    void                (SGLX_CALL *callback)(const char *status),
    void                *hSglx,
    const std::string   &filename );

#ifdef __cplusplus
}
#endif

#endif // SGLXAPI_H


