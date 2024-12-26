#ifndef C_SGLXAPI_H
#define C_SGLXAPI_H

/* ---------------------------------------------------------------- */
/* C API ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

#include <stdbool.h>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__NT__)
// Windows
#if defined(SGLXAPI_LIBRARY)
#  define SGLX_EXPORT __declspec(dllexport)
#else
#  define SGLX_EXPORT __declspec(dllimport)
#endif
#define SGLX_CALL   __stdcall
#else
// Not Windows
#if defined(SGLXAPI_LIBRARY)
#  define SGLX_EXPORT __attribute__((visibility("default")))
#else
#  define SGLX_EXPORT
#endif
#define SGLX_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------- */
/* Types ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

typedef unsigned long long t_ull;

/* ---------------------------------------------------------------- */
/* Usage ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

/*
    A client application first creates a connection handle:
    hSglx = c_sglx_createHandle().

    The returned handle can then be passed to any of the API functions.
    When done, client calls c_sglx_close() and c_sglx_destroyHandle().

    Almost all functions return either true=SUCCESS, or false=FAIL.
    Get the reason for the most recent failure with c_sglx_getError().
*/

/* ---------------------------------------------------------------- */
/* Functions ------------------------------------------------------ */
/* ---------------------------------------------------------------- */

/* ------------
    After a call returning (nval) strings, retrieve the ith
    string with this function. Index (ith) is zero-based.
*/
SGLX_EXPORT const char* SGLX_CALL c_sglx_getstr( int *len, void *hSglx, int ith );

/* ------------
    After a call returning (nval) int, retrieve the ith
    int with this function. Index (ith) is zero-based.
*/
SGLX_EXPORT int SGLX_CALL c_sglx_getint( void *hSglx, int ith );

/* ------------
    After a call returning (nval) double, retrieve the ith
    double with this function. Index (ith) is zero-based.
*/
SGLX_EXPORT double SGLX_CALL c_sglx_getdbl( void *hSglx, int ith );

/* ------------
    Before any call that sets key-value (kv) params, first create them
    with a series of calls to this function. (1) Initialize the params
    by calling c_sglx_setkv( hSglx, "", "" ) or c_sglx_setkv( hSglx, 0, 0 ).
    (2) For each kv pair, call c_sglx_setkv( hSglx, key, val ). (3) Call
    the desired set() function.
*/
SGLX_EXPORT void SGLX_CALL c_sglx_setkv( void *hSglx, const char *key, const char *val );

/* ------------
    Create a new connection handle to be passed to c_sglx_connect()
    and to all subsequent API calls. This call allocates memory
    owned by the DLL. Release it with c_sglx_destroyHandle() when
    you are completely done with the connection.

    The returned handle is an opaque (void*).
*/
SGLX_EXPORT void* SGLX_CALL c_sglx_createHandle();

/* ------------
    Destroy handle and release memory resources.
*/
SGLX_EXPORT void SGLX_CALL c_sglx_destroyHandle( void *hSglx );

/* ------------
    Get latest error message.
*/
SGLX_EXPORT const char* SGLX_CALL c_sglx_getError( void *hSglx );

/* ------------
    Connect to SpikeGLX server.
    Note: local machine address: (host="localhost", port=4142).
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_connect(
    void        *hSglx,
    const char  *host,
    int         port );

/* ------------
    Close connection and release network resources.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_close( void *hSglx );

/* ------------
    Hide console/log window to reduce screen clutter.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_consoleHide( void *hSglx );

/* ------------
    Show console/log window.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_consoleShow( void *hSglx );

/* ------------
    Retrieve a listing of files in idir data directory.
    Get main data directory by setting idir=0. If successful,
    nval is the count of path strings. See c_sglx_getstr().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_enumDataDir( int *nval, void *hSglx, int idir );

/* ------------
    Get binary stream data as linear array.
    Samp count = MIN(max_samps,available).
    Each sample contains N int16 channels, N depends upon channel_subset.
    If filtered IM stream buffers are enabled, you may fetch from them with js=-2.
    Fetching starts at index start_samp.
    channel_subset is an array of specific channels to fetch, optionally,
         -1 = all acquired channels, or,
         -2 = all saved channels.
    n_cs is the channel_subset count.
    downsample is an integer, every nth sample.

    Return:
    - headCt    = index of first sample, or zero if error.
    - data      = address of these native data.
    - n_data    = data length (number of shorts).
    Client should not try to free data, this is managed by DLL.
*/
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
    int         downsample );

/* ------------
    Get binary stream data as linear array.
    Samp count = MIN(max_samps,available).
    Each sample contains N int16 channels, N depends upon channel_subset.
    If filtered IM stream buffers are enabled, you may fetch from them with js=-2.
    channel_subset is an array of specific channels to fetch, optionally,
         -1 = all acquired channels, or,
         -2 = all saved channels.
    n_cs is the channel_subset count.
    downsample is an integer, every nth sample.

    Return:
    - headCt    = index of first sample, or zero if error.
    - data      = address of these native data.
    - n_data    = data length (number of shorts).
    Client should not try to free data, this is managed by DLL.
*/
SGLX_EXPORT t_ull SGLX_CALL c_sglx_fetchLatest(
    const short **data,
    int         *n_data,
    void        *hSglx,
    int         js,
    int         ip,
    int         max_samps,
    const int   *channel_subset,
    int         n_cs,
    int         downsample );

/* ------------
    Get ith global data directory.
    Get main data directory by setting idir=0.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getDataDir(
    const char  **dir,
    void        *hSglx,
    int         idir );

/* ------------
    Get geomMap for given logical imec probe.
    Returned as a struct of key-value pairs.
    Header fields:
        head_partNumber
        head_numShanks
        head_shankPitch   ; microns
        head_shankWidth   ; microns
    Channel 5, e.g.:
        ch5_s   ; shank index
        ch5_x   ; microns from left edge of shank
        ch5_z   ; microns from center of tip-most electrode row
        ch5_u   ; used-flag (in CAR operations)

    Note: Fields are in ascending alphanumeric order!
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getGeomMap(
    int     *nval,
    void    *hSglx,
    int     ip );

/* ------------
    Get gains for given probe and channel.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getImecChanGains(
    double      *APgain,
    double      *LFgain,
    void        *hSglx,
    int         ip,
    int         chan );

/* ------------
    Get shankMap for NI stream. If successful the data are returned
    as nval strings. The first string is the header: "ns nc ns" giving
    the shank's maximum count of {shanks, cols, rows}. The remaining
    strings are in channel order: "s, c, r, u" giving that channel's
    {shank, col, row}-index and its used-flag (used in CAR operations).
    See c_sglx_getstr().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getNIShankMap( int *nval, void *hSglx );

/* ------------
    Get the most recently used run parameters.
    These are a set of 'key=value' strings.
    If successful, nval is the string count.
    See c_sglx_getstr().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getParams( int *nval, void *hSglx );

/* ------------
    Get imec parameters common to all enabled probes.
    These are a set of 'key=value' strings.
    If successful, nval is the string count.
    See c_sglx_getstr().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getParamsImecCommon( int *nval, void *hSglx );

/* ------------
    Get imec parameters for given logical probe.
    These are a set of 'key=value' strings.
    If successful, nval is the string count.
    See c_sglx_getstr().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getParamsImecProbe(
    int     *nval,
    void    *hSglx,
    int     ip );

/* ------------
    Get parameters for selected OneBox.
    These are a set of 'key=value' strings.
    If successful, nval is the string count.
    See c_sglx_getstr().
    To reference a OneBox configured as a recording stream
    set ip to its stream-id; if ip >= 0, slot is ignored.
    Any selected OneBox can also be referenced by setting
    ip = -1, and giving its slot index.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getParamsOneBox(
    int     *nval,
    void    *hSglx,
    int     ip,
    int     slot );

/* ------------
    Get string with format:
    (probeID,nShanks,partNumber)()...
    - A parenthesized entry for each selected probe.
    - probeID: zero-based integer.
    - nShanks: integer {1,4}.
    - partNumber: string, e.g., NP1000.
    - If no probes, return '()'.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getProbeList( const char **list, void *hSglx );

/* ------------
    Get run base name.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getRunName( const char **name, void *hSglx );

/* ------------
    For the selected substream, return the number of channels of
    each type that stream is acquiring. If successful, nval is the
    type count. See c_sglx_getint().

    js = 0: NI channels: {MN,MA,XA,DW}.
    js = 1: OB channels: {XA,DW,SY}.
    js = 2: IM channels: {AP,LF,SY}.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamAcqChans(
    int     *nval,
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    Return index of first sample in selected stream file,
    or zero if unavailable.
*/
SGLX_EXPORT t_ull SGLX_CALL c_sglx_getStreamFileStart(
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    Return multiplier converting 16-bit binary channel to volts.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamI16ToVolts(
    double  *mult,
    void    *hSglx,
    int     js,
    int     ip,
    int     chan );

/* ------------
    Return largest positive integer value for selected stream.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamMaxInt(
    int     *maxInt,
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    Get number (np) of js-type substreams.
    For the given js, ip has range [0..np-1].
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamNP( int *np, void *hSglx, int js );

/* ------------
    Return number of samples since current run started,
    or zero if not running or error.
*/
SGLX_EXPORT t_ull SGLX_CALL c_sglx_getStreamSampleCount(
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    Return sample rate of selected stream in Hz, or zero if error.
*/
SGLX_EXPORT double SGLX_CALL c_sglx_getStreamSampleRate(
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    Get a list containing the indices of the acquired channels
    that are being saved. If successful, nval is the list length.
    See c_sglx_getint().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamSaveChans(
    int     *nval,
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    js = 1: Get OneBox SN and slot.
    js = 2: Get probe  SN and type.
    SN = serial number string.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamSN(
    int         *slot_or_type,
    const char  **SN,
    void        *hSglx,
    int         js,
    int         ip );

/* ------------
    Get voltage range of selected data stream.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_getStreamVoltageRange(
    double  *vMin,
    double  *vMax,
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    Return number of seconds since SpikeGLX application was launched,
    or zero if error.
*/
SGLX_EXPORT double SGLX_CALL c_sglx_getTime( void *hSglx );

/* ------------
    Get SpikeGLX version string.
*/
SGLX_EXPORT const char* SGLX_CALL c_sglx_getVersion( void *hSglx );

/* ------------
    Test if console window is hidden.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_isConsoleHidden( bool *hidden, void *hSglx );

/* ------------
    Test if SpikeGLX has completed its startup initialization
    and is ready to run.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_isInitialized( bool *ready, void *hSglx );

/* ------------
    Test if SpikeGLX is currently acquiring data.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_isRunning( bool *running, void *hSglx );

/* ------------
    Test if SpikeGLX is currently running AND saving data.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_isSaving( bool *saving, void *hSglx );

/* ------------
    Test if graphs currently sorted in user order.
    This query is sent only to the main Graphs window.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_isUserOrder(
    bool    *userOrder,
    void    *hSglx,
    int     js,
    int     ip );

/* ------------
    Return sample in dst stream corresponding to given sample in src stream,
    or zero if error.
*/
SGLX_EXPORT t_ull SGLX_CALL c_sglx_mapSample(
    void    *hSglx,
    int     dstjs,
    int     dstip,
    t_ull   srcSample,
    int     srcjs,
    int     srcip );

/* ------------
    Direct emission to specified site (-1=dark).
    ip:    imec probe index.
    color: {0=blue, 1=red}.
    site:  [0..13], or, -1=dark.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_opto_emit(
    void    *hSglx,
    int     ip,
    int     color,
    int     site );

/* ------------
    Get array of 14 (double) site power attenuation factors.
    ip:    imec probe index.
    color: {0=blue, 1=red}.

    If successful, nval is the 14. See c_sglx_getdbl().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_opto_getAttenuations(
    int     *nval,
    void    *hSglx,
    int     ip,
    int     color );

/* ------------
    Create, verify, or repair Par2 redundancy files for 'file'.

    Arguments:
    op:      {'c'=create, 'v'=verify, 'r'=repair}.
    file:    Path to a .par2 or .bin file to which 'op' is applied.

    Status/progress lines are reported to optional callback.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_par2(
    void        (SGLX_CALL *callback)(const char *status),
    void        *hSglx,
    char        op,
    const char  *file );

/* ------------
    Set anatomy data string with Pinpoint format:
    [probe-id,shank-id](startpos,endpos,R,G,B,rgnname)(startpos,endpos,R,G,B,rgnname)…()
       - probe-id: SpikeGLX logical probe id.
       - shank-id: [0..n-shanks].
       - startpos: region start in microns from tip.
       - endpos:   region end in microns from tip.
       - R,G,B:    region color as RGB, each [0..255].
       - rgnname:  region name text.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setAnatomy_Pinpoint(
    void        *hSglx,
    const char  *shankdat );

/* ------------
    Set audio output on/off. Note that this command has
    no effect if not currently running.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setAudioEnable( void *hSglx, bool enable );

/* ------------
    Set subgroup of parameters for audio-out operation. Parameters
    are key-value pairs. See c_sglx_setkv(). This call stops current
    output. Call c_sglx_setAudioEnable() to restart it.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setAudioKVParams(
    void        *hSglx,
    const char  *group );

/* ------------
    Set ith global data directory.
    Set required parameter idir to zero for main data directory.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setDataDir(
    void        *hSglx,
    int         idir,
    const char  *dir );

/* ------------
    Set one or more NI lines high/low.
      - lines is a string list of lines to set, e.g.:
          'Dev6/port0/line2,Dev6/port0/line5'
          'Dev6/port1/line0:3'
          'Dev6/port1:2'
      - bits is a uint32 value, each bit maps to a line:
          The lowest 8 bits map to port 0.
          The next higher 8 bits map to port 1, etc.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_set_NI_DO(
    void            *hSglx,
    const char      *lines,
    unsigned int    bits );

/* ------------
    If a run is in progress, set metadata to be added to
    the next output file-set. Metadata are key-value pairs.
    See c_sglx_setkv().
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setKVMetadata( void *hSglx );

/* ------------
    The inverse of c_sglx_getParams, this sets run parameters.
    Parameters are key-value pairs. See c_sglx_setkv(). The call
    will error if a run is currently in progress.

    Note: You can set any subset of [DAQSettings].
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParams( void *hSglx );

/* ------------
    The inverse of c_sglx_getParamsImecCommon, this sets parameters
    common to all enabled probes. Parameters are key-value pairs.
    See c_sglx_setkv(). The call will error if a run is currently
    in progress.

    Note: You can set any subset of [DAQ_Imec_All].
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParamsImecCommon( void *hSglx );

/* ------------
    The inverse of c_sglx_getParamsImecProbe, this sets parameters
    for a given logical probe. Parameters are key-value pairs.
    See c_sglx_setkv(). The call will error if file writing is
    currently in progress.

    Note: You can set any subset of fields under [SerialNumberToProbe]/SNjjj.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParamsImecProbe( void *hSglx, int ip );

/* ------------
    The inverse of c_sglx_getParamsOneBox, this sets params
    for a selected OneBox. Parameters are key-value pairs.
    See c_sglx_setkv(). The call will error if a run is currently
    in progress.
    To reference a OneBox configured as a recording stream
    set ip to its stream-id; if ip >= 0, slot is ignored.
    Any selected OneBox can also be referenced by setting
    ip = -1, and giving its slot index.

    Note: You can set any subset of fields under [SerialNumberToOneBox]/SNjjj.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setKVParamsOneBox( void *hSglx, int ip, int slot );

/* ------------
    Set multi-drive run-splitting on/off.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setMultiDriveEnable( void *hSglx, bool enable );

/* ------------
    For only the next trigger (file writing event) this overrides
    all auto-naming, giving you complete control of where to save
    the files, the file name, and what g- and t-indices you want
    (if any). For example, regardless of the run's current data dir,
    run name and indices, if you set: 'otherdir/yyy_g5/yyy_g5_t7',
    SpikeGLX will save the next files in flat directory yyy_g5/:
       - otherdir/yyy_g5/yyy.g5_t7.nidq.bin,meta
       - otherdir/yyy_g5/yyy.g5_t7.imec0.ap.bin,meta
       - otherdir/yyy_g5/yyy.g5_t7.imec0.lf.bin,meta
       - otherdir/yyy_g5/yyy.g5_t7.imec1.ap.bin,meta
       - otherdir/yyy_g5/yyy.g5_t7.imec1.lf.bin,meta
       - etc.

    - The destination directory must already exist...No parent directories
    or probe subfolders are created in this naming mode.
    - The run must already be in progress.
    - Neither the custom name nor its indices are displayed in the Graphs
    window toolbars. Rather, the toolbars reflect standard auto-names.
    - After writing this file set, the override is cleared and auto-naming
    will resume as if you never called c_sglx_setNextFileName. You have to call
    c_sglx_setNextFileName before each trigger event to create custom trial series.
    For example, you can build a software-triggered t-series using sequence:
       + c_sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t0' )
       + c_sglx_setRecordingEnable( 1 )
       + c_sglx_setRecordingEnable( 0 )
       + c_sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t1' )
       + c_sglx_setRecordingEnable( 1 )
       + c_sglx_setRecordingEnable( 0 )
       + etc.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setNextFileName( void *hSglx, const char *name );

/* ------------
    Set gate (file writing) on/off during run.

    When auto-naming is in effect, opening the gate advances
    the g-index and resets the t-index to zero. Auto-naming is
    on unless c_sglx_setNextFileName has been used to override it.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setRecordingEnable( void *hSglx, bool enable );

/* ------------
    Set the run name for the next time files are created
    (either by trigger, c_sglx_setRecordingEnable() or by
    c_sglx_startRun()).
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setRunName( void *hSglx, const char *name );

/* ------------
    During a run, set frequency and duration of Windows
    beep signaling file closure. hertz=0 disables the beep.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setTriggerOffBeep(
    void    *hSglx,
    int     hertz,
    int     millisec );

/* ------------
    During a run set frequency and duration of Windows
    beep signaling file creation. hertz=0 disables the beep.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_setTriggerOnBeep(
    void    *hSglx,
    int     hertz,
    int     millisec );

/* ------------
    Start data acquisition run. Last-used parameters remain
    in effect. An error is flagged if already running. The
    name parameter is optional; set NULL or "" to use existing.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_startRun( void *hSglx, const char *name );

/* ------------
    Unconditionally stop current run, close data files
    and return to idle state.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_stopRun( void *hSglx );

/* ------------
    Using standard auto-naming, set both the gate (g) and
    trigger (t) levels that control file writing.
      -1 = no change.
       0 = set low.
       1 = increment and set high.
    E.g., c_sglx_triggerGT( -1, 1 ) = same g, increment t, start writing.

    - c_sglx_triggerGT only affects the 'Remote controlled' gate type
    and/or the 'Remote controlled' trigger type.
    - The 'Enable Recording' button, when shown, is a master override
    switch. c_sglx_triggerGT is blocked until you click the button or call
    c_sglx_setRecordingEnable.
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_triggerGT( void *hSglx, int g, int t );

/* ------------
    Verifies the SHA1 sum of the file specified by filename.
    If filename is relative, it is appended to the run dir.
    Absolute path/filenames are also supported. Status/progress
    lines are reported to optional callback.

    Return true if verified (and no errors).
*/
SGLX_EXPORT bool SGLX_CALL c_sglx_verifySha1(
    void        (SGLX_CALL *callback)(const char *status),
    void        *hSglx,
    const char  *filename );

#ifdef __cplusplus
}
#endif

#endif // C_SGLXAPI_H


