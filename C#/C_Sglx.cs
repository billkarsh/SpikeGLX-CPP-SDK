// Add this file to your project/solution.
//
// Simplify calls as follows:
//
// using C_Sglx_namespace;   // at top of file
// C_Sglx.fun( params );     // call function

using System;
using System.Runtime.InteropServices;

// Usage ------------------
// A client application first creates a connection handle:
// hSglx = c_sglx_createHandle().
//
// The returned handle can then be passed to any of the API functions.
// When done, client calls c_sglx_close() and c_sglx_destroyHandle().
//
// Almost all functions return either true=1=SUCCESS, or false=0=FAIL.
// Get the reason for the most recent failure with c_sglx_getError().
//
namespace C_Sglx_namespace
{
    public class C_Sglx
    {
// Status callback for use with par2 and verifySHA1 calls.
//
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate void ProgressCallback(IntPtr status);
        public static void cs_demoCallback(IntPtr status)
        {
            Console.WriteLine(System.Runtime.InteropServices.Marshal.PtrToStringAnsi(status));
        }

// After a call returning (nval) strings, retrieve the ith
// string with this function. Index (ith) is zero-based.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern IntPtr c_sglx_getstr(out int len, IntPtr hSglx, int ith);
        public static string cs_sglx_getstr(IntPtr hSglx, int ith)
        {
            int    len;
            IntPtr intPtr = c_sglx_getstr(out len, hSglx, ith);
            return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(intPtr);
        }

// After a call returning (nval) int, retrieve the ith
// int with this function. Index (ith) is zero-based.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getint(IntPtr hSglx, int ith);

// After a call returning (nval) double, retrieve the ith
// double with this function. Index (ith) is zero-based.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern double c_sglx_getdbl(IntPtr hSglx, int ith);

// Before any call that sets key-value (kv) params, first create them
// with a series of calls to this function. (1) Initialize the params
// by calling c_sglx_setkv( hSglx, "", "" ) or c_sglx_setkv( hSglx, 0, 0 ).
// (2) For each kv pair, call c_sglx_setkv( hSglx, key, val ). (3) Call
// the desired set() function.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern void c_sglx_setkv(IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string key, [MarshalAs(UnmanagedType.LPStr)] string value);

// Create a new connection handle to be passed to c_sglx_connect()
// and to all subsequent API calls. This call allocates memory
// owned by the DLL. Release it with c_sglx_destroyHandle() when
// you are completely done with the connection.
//
// The returned handle is an opaque (void*).
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern IntPtr c_sglx_createHandle();

// Destroy handle and release memory resources.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern void c_sglx_destroyHandle(IntPtr hSglx);

// Get latest error message.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern IntPtr c_sglx_getError(IntPtr hSglx);
        public static string cs_sglx_getError(IntPtr hSglx)
        {
            IntPtr  intPtr = c_sglx_getError(hSglx);
            return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(intPtr);
        }

// Connect to SpikeGLX server.
// Note: local machine address: (host="localhost", port=4142).
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_connect(IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string host, int port);

// Close connection and release network resources.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_close(IntPtr hSglx);

// Hide console/log window to reduce screen clutter.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_consoleHide(IntPtr hSglx);

// Show console/log window.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_consoleShow(IntPtr hSglx);

// Retrieve a listing of files in idir data directory.
// Get main data directory by setting idir=0. If successful,
// nval is the count of path strings. See c_sglx_getstr().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_enumDataDir(out int nval, IntPtr hSglx, int idir);

// Get binary stream data as linear array.
// Samp count = MIN(max_samps,available).
// Each sample contains N int16 channels, N depends upon channel_subset.
// Fetching starts at index start_samp.
// channel_subset is an array of specific channels to fetch, optionally,
//      -1 = all acquired channels, or,
//      -2 = all saved channels.
// n_cs is the channel_subset count.
// downsample is an integer, every nth sample.
//
// Return:
// - headCt    = index of first sample, or zero if error.
// - data      = address of these native data.
// - n_data    = data length (number of shorts).
// Client should not try to free data, this is managed by DLL.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern ulong c_sglx_fetch(out IntPtr _data, out int n_data, IntPtr hSglx, int js, int ip, ulong start_samp, int max_samps, int[] chan_subset, int n_cs, int downsamp);
        public static ulong cs_sglx_fetch(out short[] data, IntPtr hSglx, int js, int ip, ulong start_samp, int max_samps, int[] chan_subset, int downsamp)
        {
            ulong   headCt;
            IntPtr  _data;
            int     n_data;
            headCt  = c_sglx_fetch(out _data, out n_data, hSglx, js, ip, start_samp, max_samps, chan_subset, chan_subset.Length, downsamp);
            data    = new short[n_data];
            if( n_data > 0 )
                System.Runtime.InteropServices.Marshal.Copy(_data, data, 0, n_data);
            return headCt;
        }

// Get binary stream data as linear array.
// Samp count = MIN(max_samps,available).
// Each sample contains N int16 channels, N depends upon channel_subset.
// channel_subset is an array of specific channels to fetch, optionally,
//      -1 = all acquired channels, or,
//      -2 = all saved channels.
// n_cs is the channel_subset count.
// downsample is an integer, every nth sample.
//
// Return:
// - headCt    = index of first sample, or zero if error.
// - data      = address of these native data.
// - n_data    = data length (number of shorts).
// Client should not try to free data, this is managed by DLL.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern ulong c_sglx_fetchLatest(out IntPtr _data, out int n_data, IntPtr hSglx, int js, int ip, int max_samps, int[] chan_subset, int n_cs, int downsamp);
        public static ulong cs_sglx_fetchLatest(out short[] data, IntPtr hSglx, int js, int ip, int max_samps, int[] chan_subset, int downsamp)
        {
            ulong   headCt;
            IntPtr  _data;
            int     n_data;
            headCt  = c_sglx_fetchLatest(out _data, out n_data, hSglx, js, ip, max_samps, chan_subset, chan_subset.Length, downsamp);
            data    = new short[n_data];
            if( n_data > 0 )
                System.Runtime.InteropServices.Marshal.Copy(_data, data, 0, n_data);
            return headCt;
        }

// Get ith global data directory.
// Get main data directory by setting idir=0.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern int c_sglx_getDataDir(out IntPtr intPtr, IntPtr hSglx, int idir);
        public static int cs_sglx_getDataDir(out string dir, IntPtr hSglx, int idir = 0)
        {
            IntPtr  intPtr;
            int     ret = c_sglx_getDataDir(out intPtr, hSglx, idir);
            dir = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(intPtr);
            return ret;
        }

// Get geomMap for given logical imec probe.
// Returned as a struct of key-value pairs.
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
// Note: Fields are in ascending alphanumeric order!
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getGeomMap(out int nval, IntPtr hSglx, int ip);

// Get gains for given probe and channel.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getImecChanGains(out double APgain, out double LFgain, IntPtr hSglx, int ip, int chan);

// Get shankMap for NI stream. If successful the data are returned
// as nval strings. The first string is the header: "ns nc ns" giving
// the shank's maximum count of {shanks, cols, rows}. The remaining
// strings are in channel order: "s, c, r, u" giving that channel's
// {shank, col, row}-index and its used-flag (used in CAR operations).
// See c_sglx_getstr().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getNIShankMap(out int nval, IntPtr hSglx);

// Get the most recently used run parameters.
// These are a set of 'key=value' strings.
// If successful, nval is the string count.
// See c_sglx_getstr().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getParams(out int nval, IntPtr hSglx);

// Get imec parameters common to all enabled probes.
// These are a set of 'key=value' strings.
// If successful, nval is the string count.
// See c_sglx_getstr().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getParamsImecCommon(out int nval, IntPtr hSglx);

// Get imec parameters for given logical probe.
// These are a set of 'key=value' strings.
// If successful, nval is the string count.
// See c_sglx_getstr().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getParamsImecProbe(out int nval, IntPtr hSglx, int ip);

// Get parameters for given logical OneBox.
// These are a set of 'key=value' strings.
// If successful, nval is the string count.
// See c_sglx_getstr().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getParamsOneBox(out int nval, IntPtr hSglx, int ip);

// Get string with format:
// (probeID,nShanks,partNumber)()...
// - A parenthesized entry for each selected probe.
// - probeID: zero-based integer.
// - nShanks: integer {1,4}.
// - partNumber: string, e.g., NP1000.
// - If no probes, return '()'.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern int c_sglx_getProbeList(out IntPtr intPtr, IntPtr hSglx);
        public static int cs_sglx_getProbeList(out string list, IntPtr hSglx)
        {
            IntPtr  intPtr;
            int     ret = c_sglx_getProbeList(out intPtr, hSglx);
            list = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(intPtr);
            return ret;
        }

// Get run base name.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern int c_sglx_getRunName(out IntPtr intPtr, IntPtr hSglx);
        public static int cs_sglx_getRunName(out string name, IntPtr hSglx)
        {
            IntPtr  intPtr;
            int     ret = c_sglx_getRunName(out intPtr, hSglx);
            name = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(intPtr);
            return ret;
        }

// For the selected substream, return the number of channels of
// each type that stream is acquiring. If successful, nval is the
// type count. See c_sglx_getint().
//
// js = 0: NI channels: {MN,MA,XA,DW}.
// js = 1: OB channels: {XA,DW,SY}.
// js = 2: IM channels: {AP,LF,SY}.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getStreamAcqChans(out int nval, IntPtr hSglx, int js, int ip);

// Return index of first sample in selected stream file,
// or zero if unavailable.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern ulong c_sglx_getStreamFileStart(IntPtr hSglx, int js, int ip);

// Return multiplier converting 16-bit binary channel to volts.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getStreamI16ToVolts(out double mult, IntPtr hSglx, int js, int ip, int chan);

// Return largest positive integer value for selected stream.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getStreamMaxInt(out int maxInt, IntPtr hSglx, int js, int ip);

// Get number (np) of js-type substreams.
// For the given js, ip has range [0..np-1].
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getStreamNP(out int np, IntPtr hSglx, int js);

// Return number of samples since current run started,
// or zero if not running or error.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern ulong c_sglx_getStreamSampleCount(IntPtr hSglx, int js, int ip);

// Return sample rate of selected stream in Hz, or zero if error.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern double c_sglx_getStreamSampleRate(IntPtr hSglx, int js, int ip);

// Get a list containing the indices of the acquired channels
// that are being saved. If successful, nval is the list length.
// See c_sglx_getint().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getStreamSaveChans(out int nval, IntPtr hSglx, int js, int ip);

// js = 1: Get OneBox SN and slot.
// js = 2: Get probe  SN and type.
// SN = serial number string.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern int c_sglx_getStreamSN(out int slot_or_type, out IntPtr intPtr, IntPtr hSglx, int js, int ip);
        public static int cs_sglx_getStreamSN(out int slot_or_type, out string SN, IntPtr hSglx, int js, int ip)
        {
            IntPtr  intPtr;
            int     ret = c_sglx_getStreamSN(out slot_or_type, out intPtr, hSglx, js, ip);
            SN = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(intPtr);
            return ret;
        }

// Get voltage range of selected data stream.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_getStreamVoltageRange(out double vMin, out double vMax, IntPtr hSglx, int js, int ip);

// Return number of seconds since SpikeGLX application was launched,
// or zero if error.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern double c_sglx_getTime(IntPtr hSglx);

// Get SpikeGLX version string.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        static extern IntPtr c_sglx_getVersion(IntPtr hSglx);
        public static string cs_sglx_getVersion(IntPtr hSglx)
        {
            IntPtr  intPtr = c_sglx_getVersion(hSglx);
            return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(intPtr);
        }

// Test if console window is hidden.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_isConsoleHidden(out byte hidden, IntPtr hSglx);

// Test if SpikeGLX has completed its startup initialization
// and is ready to run.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_isInitialized(out byte ready, IntPtr hSglx);

// Test if SpikeGLX is currently acquiring data.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_isRunning(out byte running, IntPtr hSglx);

// Test if SpikeGLX is currently running AND saving data.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_isSaving(out byte saving, IntPtr hSglx);

// Test if graphs currently sorted in user order.
// This query is sent only to the main Graphs window.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_isUserOrder(out byte userOrder, IntPtr hSglx, int js, int ip);

// Return sample in dst stream corresponding to given sample in src stream,
// or zero if error.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern ulong c_sglx_mapSample(IntPtr hSglx, int dstjs, int dstip, ulong srcSample, int srcjs, int srcip);

// Direct emission to specified site (-1=dark).
// ip:    imec probe index.
// color: {0=blue, 1=red}.
// site:  [0..13], or, -1=dark.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_opto_emit(IntPtr hSglx, int ip, int color, int site);

// Get array of 14 (double) site power attenuation factors.
// ip:    imec probe index.
// color: {0=blue, 1=red}.
//
// If successful, nval is the 14. See c_sglx_getdbl().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_opto_getAttenuations(out int nval, IntPtr hSglx, int ip, int color);

// Create, verify, or repair Par2 redundancy files for 'file'.
//
// Arguments:
// op:      {'c'=create, 'v'=verify, 'r'=repair}.
// file:    Path to a .par2 or .bin file to which 'op' is applied.
//
// Status/progress lines are reported to optional callback.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_par2([MarshalAs(UnmanagedType.FunctionPtr)] ProgressCallback callback, IntPtr hSglx, char op, [MarshalAs(UnmanagedType.LPStr)] string file);

// Set anatomy data string with Pinpoint format:
// [probe-id,shank-id](startpos,endpos,R,G,B,rgnname)(startpos,endpos,R,G,B,rgnname)…()
//    - probe-id: SpikeGLX logical probe id.
//    - shank-id: [0..n-shanks].
//    - startpos: region start in microns from tip.
//    - endpos:   region end in microns from tip.
//    - R,G,B:    region color as RGB, each [0..255].
//    - rgnname:  region name text.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setAnatomy_Pinpoint(IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string shankdat);

// Set audio output on/off. Note that this command has
// no effect if not currently running.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setAudioEnable(IntPtr hSglx, int enable);

// Set subgroup of parameters for audio-out operation. Parameters
// are key-value pairs. See c_sglx_setkv(). This call stops current
// output. Call c_sglx_setAudioEnable() to restart it.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setAudioKVParams(IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string group);

// Set ith global data directory.
// Set required parameter idir to zero for main data directory.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setDataDir(IntPtr hSglx, int idir, [MarshalAs(UnmanagedType.LPStr)] string dir);

// Set digital output high/low. Channel strings have form:
// "Dev6/port0/line2,Dev6/port0/line5".
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setDigitalOut(IntPtr hSglx, int hi_lo, [MarshalAs(UnmanagedType.LPStr)] string channels);

// If a run is in progress, set metadata to be added to
// the next output file-set. Metadata are key-value pairs.
// See c_sglx_setkv().
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setKVMetadata(IntPtr hSglx);

// The inverse of c_sglx_getParams, this sets run parameters.
// Parameters are key-value pairs. See c_sglx_setkv(). The call
// will error if a run is currently in progress.
//
// Note: You can set any subset of [DAQSettings].
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setKVParams(IntPtr hSglx);

// The inverse of c_sglx_getParamsImecCommon, this sets parameters
// common to all enabled probes. Parameters are key-value pairs.
// See c_sglx_setkv(). The call will error if a run is currently
// in progress.
//
// Note: You can set any subset of [DAQ_Imec_All].
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setKVParamsImecCommon(IntPtr hSglx);

// The inverse of c_sglx_getParamsImecProbe, this sets parameters
// for a given logical probe. Parameters are key-value pairs.
// See c_sglx_setkv(). The call will error if file writing is
// currently in progress.
//
// Note: You can set any subset of fields under [SerialNumberToProbe]/SNjjj.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setKVParamsImecProbe(IntPtr hSglx, int ip);

// The inverse of c_sglx_getParamsOneBox, this sets parameters
// for a given logical OneBox. Parameters are key-value pairs.
// See c_sglx_setkv(). The call will error if a run is currently
// in progress.
//
// Note: You can set any subset of fields under [SerialNumberToOneBox]/SNjjj.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setKVParamsOneBox(IntPtr hSglx, int ip);

// Set multi-drive run-splitting on/off.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setMultiDriveEnable(IntPtr hSglx, int enable);

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
// will resume as if you never called c_sglx_setNextFileName. You have to call
// c_sglx_setNextFileName before each trigger event to create custom trial series.
// For example, you can build a software-triggered t-series using sequence:
//    + c_sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t0' )
//    + c_sglx_setRecordingEnable( 1 )
//    + c_sglx_setRecordingEnable( 0 )
//    + c_sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t1' )
//    + c_sglx_setRecordingEnable( 1 )
//    + c_sglx_setRecordingEnable( 0 )
//    + etc.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setNextFileName(IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string name);

// Set gate (file writing) on/off during run.
//
// When auto-naming is in effect, opening the gate advances
// the g-index and resets the t-index to zero. Auto-naming is
// on unless c_sglx_setNextFileName has been used to override it.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setRecordingEnable(IntPtr hSglx, int enable);

// Set the run name for the next time files are created
// (either by trigger, c_sglx_setRecordingEnable() or by
// c_sglx_startRun()).
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setRunName(IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string name);

// During a run, set frequency and duration of Windows
// beep signaling file closure. hertz=0 disables the beep.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setTriggerOffBeep(IntPtr hSglx, int hertz, int millisec);

// During a run set frequency and duration of Windows
// beep signaling file creation. hertz=0 disables the beep.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_setTriggerOnBeep(IntPtr hSglx, int hertz, int millisec);

// Start data acquisition run. Last-used parameters remain
// in effect. An error is flagged if already running. The
// name parameter is optional; set NULL or "" to use existing.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_startRun(IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string name = "");

// Unconditionally stop current run, close data files
// and return to idle state.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_stopRun(IntPtr hSglx);

// Using standard auto-naming, set both the gate (g) and
// trigger (t) levels that control file writing.
//   -1 = no change.
//    0 = set low.
//    1 = increment and set high.
// E.g., c_sglx_triggerGT( -1, 1 ) = same g, increment t, start writing.
//
// - c_sglx_triggerGT only affects the 'Remote controlled' gate type
// and/or the 'Remote controlled' trigger type.
// - The 'Enable Recording' button, when shown, is a master override
// switch. c_sglx_triggerGT is blocked until you click the button or call
// c_sglx_setRecordingEnable.
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_triggerGT(IntPtr hSglx, int g, int t);

// Verifies the SHA1 sum of the file specified by filename.
// If filename is relative, it is appended to the run dir.
// Absolute path/filenames are also supported. Status/progress
// lines are reported to optional callback.
//
// Return true if verified (and no errors).
//
        [DllImport("SglxApi.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        public static extern int c_sglx_verifySha1([MarshalAs(UnmanagedType.FunctionPtr)] ProgressCallback callback, IntPtr hSglx, [MarshalAs(UnmanagedType.LPStr)] string filename);
    }
}


