#ifndef SGLXAPI_H
#define SGLXAPI_H

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
typedef void (SGLX_CALL *Tcpp_zer_vi32)( std::vector<int> &vi32 );
typedef void (SGLX_CALL *Tcpp_zer_vdbl)( std::vector<double> &vdbl );
typedef void (SGLX_CALL *Tcpp_zer_vstr)( std::vector<std::string> &vsstr );
typedef void (SGLX_CALL *Tcpp_zer_mstrstr)( std::map<std::string,std::string> &mstrstr );

// insert into client container
typedef void (SGLX_CALL *Tcpp_ins_str)( std::string &str, const char *data );
typedef void (SGLX_CALL *Tcpp_ins_vi32)( std::vector<int> &vi32, int data );
typedef void (SGLX_CALL *Tcpp_ins_vdbl)( std::vector<double> &vdbl, double data );
typedef void (SGLX_CALL *Tcpp_ins_vstr)( std::vector<std::string> &vstr, const char *data );
typedef void (SGLX_CALL *Tcpp_ins_mstrstr)( std::map<std::string,std::string> &mstrstr, const char *key, const char *val );

// resize client container
typedef void (SGLX_CALL *Tcpp_siz_vi16)( std::vector<short> &vi16, int n );

// get C-string from client container
typedef const char* (SGLX_CALL *Tcpp_get_str)( const std::string &str );

// Iterate through vector:
// Server sets opaque iterator to zero on first call;
// itr then managed by this function thereafter.
// Return true if another val available.
typedef bool (SGLX_CALL *Tcpp_itr_vi32)( int &val, const std::vector<int> &vi32, void* &itr );

// Iterate through map:
// Server sets opaque iterator to zero on first call;
// itr then managed by this function thereafter.
// Return true if another (key,val) pair available.
typedef bool (SGLX_CALL *Tcpp_itr_mstrstr)(
    const char*                             &key,
    const char*                             &val,
    const std::map<std::string,std::string> &mstrstr,
    void*                                   &itr );

/* ---------------------------------------------------------------- */
/* Types ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

typedef unsigned long long t_ull;

struct t_sglxconn {
    Tcpp_zer_str        cpp_zer_str;
    Tcpp_zer_vi32       cpp_zer_vi32;
    Tcpp_zer_vdbl       cpp_zer_vdbl;
    Tcpp_zer_vstr       cpp_zer_vstr;
    Tcpp_zer_mstrstr    cpp_zer_mstrstr;
    Tcpp_ins_str        cpp_ins_str;
    Tcpp_ins_vi32       cpp_ins_vi32;
    Tcpp_ins_vdbl       cpp_ins_vdbl;
    Tcpp_ins_vstr       cpp_ins_vstr;
    Tcpp_ins_mstrstr    cpp_ins_mstrstr;
    Tcpp_siz_vi16       cpp_siz_vi16;
    Tcpp_get_str        cpp_get_str;
    Tcpp_itr_vi32       cpp_itr_vi32;
    Tcpp_itr_mstrstr    cpp_itr_mstrstr;
    std::string         err,
                        vers,
                        host;
    int                 port,
                        handle;
    bool                in_checkconn;
};

struct t_sglxshankmap {
    struct t_entry {
        // shank, col, row, used
        short s, c, r, u;
    };
    // N {entries, shanks, cols, rows}
    int ne, ns, nc, nr;
    // entries packed end-to-end
    std::vector<short>  e;
    // start of ith entry
    const t_entry* e_i( int i )
        {return reinterpret_cast<t_entry*>(&e[4*i]);}
};

/* ---------------------------------------------------------------- */
/* Usage ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

// A client application first connects to the server with either:
// - sglx_connect()         (your custom container callbacks), or,
// - sglx_connect_std()     (installs our client-side callbacks).
//
// The t_sglxconn connection record can then be passed to any of
// the API functions. When done, the client should call sglx_close().
//
// Almost all functions return either true=SUCCESS, or false=FAIL.
// The reason for the most recent failure is available in S.err.

/* ---------------------------------------------------------------- */
/* Functions ------------------------------------------------------ */
/* ---------------------------------------------------------------- */

// Connect to SpikeGLX server.
// Fill in connection record.
//
// Client containers must be accessed by the client (calling app).
// Recommended code for these methods is provided in SglxCppClient.cpp.
//
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
    const std::string   &host   = "localhost",
    int                 port    = 4142 );

// Close network connection and release resources.
//
SGLX_EXPORT bool SGLX_CALL sglx_close( t_sglxconn &S );

// Hide console/log window to reduce screen clutter.
//
SGLX_EXPORT bool SGLX_CALL sglx_consoleHide( t_sglxconn &S );

// Show console/log window.
//
SGLX_EXPORT bool SGLX_CALL sglx_consoleShow( t_sglxconn &S );

// Retrieve a listing of files in the ith data directory.
// Get main data directory by setting idir=0 or omitting it.
//
SGLX_EXPORT bool SGLX_CALL sglx_enumDataDir(
    std::vector<std::string>    &vstr,
    t_sglxconn                  &S,
    int                         idir = 0 );

// Get binary stream data as linear array.
// Samp count = MIN(max_samps,available).
// Each sample contains N 16-bit channels, N depends upon channel_subset.
// Fetching starts at index start_samp.
// channel_subset is an array of specific channels to fetch, optionally,
//      -1 = all acquired channels, or,
//      -2 = all saved channels.
// Data are int16 type.
// downsample_ratio is an integer (default = 1).
//
// Return headCt = index of first sample in matrix, or zero if error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_fetch(
    std::vector<short>      &data,
    t_sglxconn              &S,
    int                     js,
    int                     ip,
    t_ull                   start_samp,
    int                     max_samps,
    const std::vector<int>  &channel_subset     = {},
    int                     downsample_ratio    = 1 );

// Get binary stream data as linear array.
// Samp count = MIN(max_samps,available).
// Each sample contains N 16-bit channels, N depends upon channel_subset.
// channel_subset is an array of specific channels to fetch, optionally,
//      -1 = all acquired channels, or,
//      -2 = all saved channels.
// Data are int16 type.
// downsample_ratio is an integer (default = 1).
//
// Return headCt = index of first sample in matrix, or zero if error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_fetchLatest(
    std::vector<short>      &data,
    t_sglxconn              &S,
    int                     js,
    int                     ip,
    int                     max_samps,
    const std::vector<int>  &channel_subset     = {},
    int                     downsample_ratio    = 1 );

// Get ith global data directory.
// Get main data directory by setting idir=0 or omitting it.
//
SGLX_EXPORT bool SGLX_CALL sglx_getDataDir(
    std::string &dir,
    t_sglxconn  &S,
    int         idir = 0 );

// Get gains for given probe and channel.
//
SGLX_EXPORT bool SGLX_CALL sglx_getImecChanGains(
    double      &APgain,
    double      &LFgain,
    t_sglxconn  &S,
    int         ip,
    int         chan );

// Get the most recently used run parameters.
// These are a map of name/value pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParams(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S );

// Get imec parameters common to all enabled probes.
// These are a map of name/value pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecCommon(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S );

// Get imec parameters for given logical probe.
// These are a map of name/value pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParamsImecProbe(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S,
    int                                 ip );

// Get parameters for given logical OneBox.
// These are a map of name/value pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_getParamsOneBox(
    std::map<std::string,std::string>   &mstrstr,
    t_sglxconn                          &S,
    int                                 ip );

// Get string with format:
// (probeID,nShanks,partNumber)()...
// - A parenthesized entry for each selected probe.
// - probeID: zero-based integer.
// - nShanks: integer {1,4}.
// - partNumber: string, e.g., NP1000.
// - If no probes, return '()'.
//
SGLX_EXPORT bool SGLX_CALL sglx_getProbeList( std::string &list, t_sglxconn &S );

// Get run base name.
//
SGLX_EXPORT bool SGLX_CALL sglx_getRunName( std::string &name, t_sglxconn &S );

// For the selected substream, return a vector of the
// number of channels of each type that stream is acquiring.
//
// js = 0: NI channels: {MN,MA,XA,DW}.
// js = 1: OB channels: {XA,DW,SY}.
// js = 2: IM channels: {AP,LF,SY}.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamAcqChans(
    std::vector<int>    &vi32,
    t_sglxconn          &S,
    int                 js,
    int                 ip );

// Return index of first sample in selected stream file,
// or zero if unavailable.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamFileStart(
    t_sglxconn  &S,
    int         js,
    int         ip );

// Return multiplier converting 16-bit binary channel to volts.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamI16ToVolts(
    double      &mult,
    t_sglxconn  &S,
    int         js,
    int         ip,
    int         chan );

// Return largest positive integer value for selected stream.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamMaxInt(
    int         &maxInt,
    t_sglxconn  &S,
    int         js,
    int         ip );

// Get number (np) of js-type substreams.
// For the given js, ip has range [0..np-1].
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamNP( int &np, t_sglxconn &S, int js );

// Return number of samples since current run started,
// or zero if not running or error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_getStreamSampleCount(
    t_sglxconn  &S,
    int         js,
    int         ip );

// Return sample rate of selected stream in Hz, or zero if error.
//
SGLX_EXPORT double SGLX_CALL sglx_getStreamSampleRate(
    t_sglxconn  &S,
    int         js,
    int         ip );

// Get a vector containing the indices of
// the acquired channels that are being saved.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamSaveChans(
    std::vector<int>    &vi32,
    t_sglxconn          &S,
    int                 js,
    int                 ip );

// Get t_sglxshankmap struct.
// Only supported for NI stream (js = 0).
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamShankMap(
    t_sglxshankmap  &shankmap,
    t_sglxconn      &S,
    int             js,
    int             ip );

// js = 1: Get OneBox SN and slot.
// js = 2: Get probe  SN and type.
// SN = serial number string.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamSN(
    int             &slot_or_type,
    std::string     &SN,
    t_sglxconn      &S,
    int             js,
    int             ip );

// Get voltage range of selected data stream.
//
SGLX_EXPORT bool SGLX_CALL sglx_getStreamVoltageRange(
    double      &vMin,
    double      &vMax,
    t_sglxconn  &S,
    int         js,
    int         ip );

// Return number of seconds since SpikeGLX application was launched,
// or zero if error.
//
SGLX_EXPORT double SGLX_CALL sglx_getTime( t_sglxconn &S );

// Get SpikeGLX version string.
//
SGLX_EXPORT bool SGLX_CALL sglx_getVersion( std::string &vers, t_sglxconn &S );

// Test if console window is hidden.
//
SGLX_EXPORT bool SGLX_CALL sglx_isConsoleHidden( bool &hidden, t_sglxconn &S );

// Test if SpikeGLX has completed its startup initialization
// and is ready to run.
//
SGLX_EXPORT bool SGLX_CALL sglx_isInitialized( bool &ready, t_sglxconn &S );

// Test if SpikeGLX is currently acquiring data.
//
SGLX_EXPORT bool SGLX_CALL sglx_isRunning( bool &running, t_sglxconn &S );

// Test if SpikeGLX is currently running AND saving data.
//
SGLX_EXPORT bool SGLX_CALL sglx_isSaving( bool &saving, t_sglxconn &S );

// Test if graphs currently sorted in user order.
// This query is sent only to the main Graphs window.
//
SGLX_EXPORT bool SGLX_CALL sglx_isUserOrder(
    bool        &userOrder,
    t_sglxconn  &S,
    int         js,
    int         ip );

// Return sample in dst stream corresponding to given sample in src stream,
// or zero if error.
//
SGLX_EXPORT t_ull SGLX_CALL sglx_mapSample(
    t_sglxconn  &S,
    int         dstjs,
    int         dstip,
    t_ull       srcSample,
    int         srcjs,
    int         srcip );

// Direct emission to specified site (-1=dark).
// ip:    imec probe index.
// color: {0=blue, 1=red}.
// site:  [0..13], or, -1=dark.
//
SGLX_EXPORT bool SGLX_CALL sglx_opto_emit(
    t_sglxconn  &S,
    int         ip,
    int         color,
    int         site );

// Get vector of 14 (double) site power attenuation factors.
// ip:    imec probe index.
// color: {0=blue, 1=red}.
//
SGLX_EXPORT bool SGLX_CALL sglx_opto_getAttenuations(
    std::vector<double> &vdbl,
    t_sglxconn          &S,
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
    t_sglxconn          &S,
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
    t_sglxconn          &S,
    const std::string   &shankdat );

// Set audio output on/off. Note that this command has
// no effect if not currently running.
//
SGLX_EXPORT bool SGLX_CALL sglx_setAudioEnable( t_sglxconn &S, bool enable );

// Set subgroup of parameters for audio-out operation. Parameters
// are a map of name/value pairs. This call stops current output.
// Call sglx_setAudioEnable() to restart it.
//
SGLX_EXPORT bool SGLX_CALL sglx_setAudioParams(
    t_sglxconn                              &S,
    const std::string                       &group,
    const std::map<std::string,std::string> &mstrstr );

// Set ith global data directory.
// Set required parameter idir to zero for main data directory.
//
SGLX_EXPORT bool SGLX_CALL sglx_setDataDir(
    t_sglxconn          &S,
    int                 idir,
    const std::string   &dir );

// Set digital output high/low. Channel strings have form:
// "Dev6/port0/line2,Dev6/port0/line5".
//
SGLX_EXPORT bool SGLX_CALL sglx_setDigitalOut(
    t_sglxconn          &S,
    bool                hi_lo,
    const std::string   &channels );

// If a run is in progress, set metadata to be added to the
// next output file-set. Metadata must be in the form of a
// map of name/value pairs.
//
SGLX_EXPORT bool SGLX_CALL sglx_setMetadata(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr );

// Set multi-drive run-splitting on/off.
//
SGLX_EXPORT bool SGLX_CALL sglx_setMultiDriveEnable( t_sglxconn &S, bool enable );

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
    t_sglxconn          &S,
    const std::string   &name );

// The inverse of sglx_getParams, this sets run parameters.
// Alternatively, you can pass the parameters to sglx_startRun,
// which calls this in turn. Run parameters are a map of
// name/value pairs. The call will error if a run is currently
// in progress.
//
// Note: You can set any subset of [DAQSettings].
//
SGLX_EXPORT bool SGLX_CALL sglx_setParams(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr );

// The inverse of sglx_getParamsImecCommon, this sets parameters
// common to all enabled probes. Parameters are a map of
// name/value pairs. The call will error if a run is currently
// in progress.
//
// Note: You can set any subset of [DAQ_Imec_All].
//
SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecCommon(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr );

// The inverse of sglx_getParamsImecProbe, this sets parameters
// for a given logical probe. Parameters are a map of
// name/value pairs. The call will error if file writing
// is currently in progress.
//
// Note: You can set any subset of fields under [SerialNumberToProbe]/SNjjj.
//
SGLX_EXPORT bool SGLX_CALL sglx_setParamsImecProbe(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr,
    int                                     ip );

// The inverse of sglx_getParamsOneBox, this sets parameters
// for a given logical OneBox. Parameters are a map of
// name/value pairs. The call will error if a run is currently
// in progress.
//
// Note: You can set any subset of fields under [SerialNumberToOneBox]/SNjjj.
//
SGLX_EXPORT bool SGLX_CALL sglx_setParamsOneBox(
    t_sglxconn                              &S,
    const std::map<std::string,std::string> &mstrstr,
    int                                     ip );

// Set gate (file writing) on/off during run.
//
// When auto-naming is in effect, opening the gate advances
// the g-index and resets the t-index to zero. Auto-naming is
// on unless sglx_setNextFileName has been used to override it.
//
SGLX_EXPORT bool SGLX_CALL sglx_setRecordingEnable( t_sglxconn &S, bool enable );

// Set the run name for the next time files are created
// (either by trigger, sglx_setRecordingEnable() or by sglx_startRun()).
//
SGLX_EXPORT bool SGLX_CALL sglx_setRunName(
    t_sglxconn          &S,
    const std::string   &name );

// During a run, set frequency and duration of Windows
// beep signaling file closure. hertz=0 disables the beep.
//
SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOffBeep(
    t_sglxconn          &S,
    int                 hertz,
    int                 millisec );

// During a run set frequency and duration of Windows
// beep signaling file creation. hertz=0 disables the beep.
//
SGLX_EXPORT bool SGLX_CALL sglx_setTriggerOnBeep(
    t_sglxconn          &S,
    int                 hertz,
    int                 millisec );

// Start data acquisition run. Last-used parameters remain
// in effect. An error is flagged if already running.
//
SGLX_EXPORT bool SGLX_CALL sglx_startRun(
    t_sglxconn          &S,
    const std::string   &name = "" );

// Unconditionally stop current run, close data files
// and return to idle state.
//
SGLX_EXPORT bool SGLX_CALL sglx_stopRun( t_sglxconn &S );

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
SGLX_EXPORT bool SGLX_CALL sglx_triggerGT( t_sglxconn &S, int g, int t );

// Verifies the SHA1 sum of the file specified by filename.
// If filename is relative, it is appended to the run dir.
// Absolute path/filenames are also supported. Status/progress
// lines are reported to optional callback.
//
// Return true if verified (and no errors).
//
SGLX_EXPORT bool SGLX_CALL sglx_verifySha1(
    void                (SGLX_CALL *callback)(const char *status),
    t_sglxconn          &S,
    const std::string   &filename );

#ifdef __cplusplus
}
#endif

#endif // SGLXAPI_H


