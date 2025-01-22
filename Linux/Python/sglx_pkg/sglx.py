# -*- coding: utf-8 -*-

from sys import version_info
from ctypes import *

if version_info >= (3,8):
    sglx = CDLL( "libSglxApi.so", winmode=0 )
else:
    sglx = CDLL( "libSglxApi.so" )


# Usage ------------------
# A client application first creates a connection handle:
# hSglx = c_sglx_createHandle().
#
# The returned handle can then be passed to any of the API functions.
# When done, client calls c_sglx_close() and c_sglx_destroyHandle().
#
# Almost all functions return either true=1=SUCCESS, or false=0=FAIL.
# Get the reason for the most recent failure with c_sglx_getError().
# ------------------------


# Status callback for use with par2 and verifySHA1 calls.
# sglx_demo_callback( status_string )
#
T_sglx_callback = CFUNCTYPE( None, c_char_p )

@T_sglx_callback
def sglx_demo_callback( status ):
    print( status )

# After a call returning (nval) strings, retrieve the ith
# string with this function. Index (ith) is zero-based.
# c_char_p = c_sglx_getstr( byref(len), hSglx, ith )
#
c_sglx_getstr = sglx.c_sglx_getstr
c_sglx_getstr.restype = c_char_p
c_sglx_getstr.argtypes = [POINTER(c_int), c_void_p, c_int]

# After a call returning (nval) int, retrieve the ith
# int with this function. Index (ith) is zero-based.
# c_int = c_sglx_getint( hSglx, ith )
#
c_sglx_getint = sglx.c_sglx_getint
c_sglx_getint.restype = c_int
c_sglx_getint.argtypes = [c_void_p, c_int]

# After a call returning (nval) double, retrieve the ith
# double with this function. Index (ith) is zero-based.
# c_double = c_sglx_getdbl( hSglx, ith )
#
c_sglx_getdbl = sglx.c_sglx_getdbl
c_sglx_getdbl.restype = c_double
c_sglx_getdbl.argtypes = [c_void_p, c_int]

# Before any call that sets key-value (kv) params, first create them
# with a series of calls to this function. (1) Initialize the params
# by calling c_sglx_setkv( hSglx, "", "" ) or c_sglx_setkv( hSglx, 0, 0 ).
# (2) For each kv pair, call c_sglx_setkv( hSglx, key, val ). (3) Call
# the desired set() function.
# c_sglx_setkv( hSglx, key, val )
#
c_sglx_setkv = sglx.c_sglx_setkv
c_sglx_setkv.restype = None
c_sglx_setkv.argtypes = [c_void_p, c_char_p, c_char_p]

# Create a new connection handle to be passed to c_sglx_connect()
# and to all subsequent API calls. This call allocates memory
# owned by the DLL. Release it with c_sglx_destroyHandle() when
# you are completely done with the connection.
#
# The returned handle is an opaque (void*).
# hSglx = c_sglx_createHandle()
#
c_sglx_createHandle = sglx.c_sglx_createHandle
c_sglx_createHandle.restype = c_void_p
c_sglx_createHandle.argtypes = []

# Destroy handle and release memory resources.
# c_sglx_destroyHandle( hSglx )
#
c_sglx_destroyHandle = sglx.c_sglx_destroyHandle
c_sglx_destroyHandle.restype = None
c_sglx_destroyHandle.argtypes = [c_void_p]

# Get latest error message.
# c_char_p = c_sglx_getError( hSglx )
#
c_sglx_getError = sglx.c_sglx_getError
c_sglx_getError.restype = c_char_p
c_sglx_getError.argtypes = [c_void_p]

# Connect to SpikeGLX server.
# Note: local machine address: (host="localhost".encode('utf-8'), port=4142).
# ok = c_sglx_connect( hSglx, addr, port )
#
c_sglx_connect = sglx.c_sglx_connect
c_sglx_connect.restype = c_bool
c_sglx_connect.argtypes = [c_void_p, c_char_p, c_int]

# Close connection and release network resources.
# ok = c_sglx_close( hSglx )
#
c_sglx_close = sglx.c_sglx_close
c_sglx_close.restype = c_bool
c_sglx_close.argtypes = [c_void_p]

# Hide console/log window to reduce screen clutter.
# ok = c_sglx_consoleHide( hSglx )
#
c_sglx_consoleHide = sglx.c_sglx_consoleHide
c_sglx_consoleHide.restype = c_bool
c_sglx_consoleHide.argtypes = [c_void_p]

# Show console/log window.
# ok = c_sglx_consoleShow( hSglx )
#
c_sglx_consoleShow = sglx.c_sglx_consoleShow
c_sglx_consoleShow.restype = c_bool
c_sglx_consoleShow.argtypes = [c_void_p]

# Retrieve a listing of files in idir data directory.
# Get main data directory by setting idir=0. If successful,
# nval is the count of path strings. See c_sglx_getstr().
# ok = c_sglx_enumDataDir( byref(nval), hSglx, idir )
#
c_sglx_enumDataDir = sglx.c_sglx_enumDataDir
c_sglx_enumDataDir.restype = c_bool
c_sglx_enumDataDir.argtypes = [POINTER(c_int), c_void_p, c_int]

# Get binary stream data as linear array.
# Samp count = MIN(max_samps,available).
# Each sample contains N int16 channels, N depends upon channel_subset.
# If filtered IM stream buffers are enabled, you may fetch from them with js=-2.
# Fetching starts at index start_samp.
# channel_subset is an array of specific channels to fetch, optionally,
#      -1 = all acquired channels, or,
#      -2 = all saved channels.
# n_cs is the channel_subset count.
# downsample is an integer, every nth sample.
#
# Return:
# - headCt    = index of first sample, or zero if error.
# - data      = address of these native data.
# - n_data    = data length (number of shorts).
# Client should not try to free data, this is managed by DLL.
# headCt = c_sglx_fetch( byref(data), byref(n_data), hSglx, js, ip, start_samp, max_samps, channel_subset, n_cs, downsample )
#
c_sglx_fetch = sglx.c_sglx_fetch
c_sglx_fetch.restype = c_ulonglong
c_sglx_fetch.argtypes = [POINTER(POINTER(c_short)), POINTER(c_int), c_void_p, c_int, c_int, c_ulonglong, c_int, POINTER(c_int), c_int, c_int]

# Get binary stream data as linear array.
# Samp count = MIN(max_samps,available).
# Each sample contains N int16 channels, N depends upon channel_subset.
# If filtered IM stream buffers are enabled, you may fetch from them with js=-2.
# channel_subset is an array of specific channels to fetch, optionally,
#      -1 = all acquired channels, or,
#      -2 = all saved channels.
# n_cs is the channel_subset count.
# downsample is an integer, every nth sample.
#
# Return:
# - headCt    = index of first sample, or zero if error.
# - data      = address of these native data.
# - n_data    = data length (number of shorts).
# Client should not try to free data, this is managed by DLL.
# headCt = c_sglx_fetchLatest( byref(data), byref(n_data), hSglx, js, ip, max_samps, channel_subset, n_cs, downsample )
#
c_sglx_fetchLatest = sglx.c_sglx_fetchLatest
c_sglx_fetchLatest.restype = c_ulonglong
c_sglx_fetchLatest.argtypes = [POINTER(POINTER(c_short)), POINTER(c_int), c_void_p, c_int, c_int, c_int, POINTER(c_int), c_int, c_int]

# Get ith global data directory.
# Get main data directory by setting idir=0.
# ok = c_sglx_getDataDir( byref(dir), hSglx, idir )
#
c_sglx_getDataDir = sglx.c_sglx_getDataDir
c_sglx_getDataDir.restype = c_bool
c_sglx_getDataDir.argtypes = [POINTER(c_char_p), c_void_p, c_int]

# Get geomMap for given logical imec probe.
# Returned as a struct of key-value pairs.
# Header fields:
#     head_partNumber
#     head_numShanks
#     head_shankPitch   ; microns
#     head_shankWidth   ; microns
# Channel 5, e.g.:
#     ch5_s   ; shank index
#     ch5_x   ; microns from left edge of shank
#     ch5_z   ; microns from center of tip-most electrode row
#     ch5_u   ; used-flag (in CAR operations)
#
# Note: Fields are in ascending alphanumeric order!
# ok = c_sglx_getGeomMap( byref(nval), hSglx, ip )
#
c_sglx_getGeomMap = sglx.c_sglx_getGeomMap
c_sglx_getGeomMap.restype = c_bool
c_sglx_getGeomMap.argtypes = [POINTER(c_int), c_void_p, c_int]

# Get gains for given probe and channel.
# ok = c_sglx_getImecChanGains( byref(APgain), byref(LFgain), hSglx, ip, chan )
#
c_sglx_getImecChanGains = sglx.c_sglx_getImecChanGains
c_sglx_getImecChanGains.restype = c_bool
c_sglx_getImecChanGains.argtypes = [POINTER(c_double), POINTER(c_double), c_void_p, c_int, c_int]

# Get shankMap for NI stream. If successful the data are returned
# as nval strings. The first string is the header: "ns nc ns" giving
# the shank's maximum count of {shanks, cols, rows}. The remaining
# strings are in channel order: "s, c, r, u" giving that channel's
# {shank, col, row}-index and its used-flag (used in CAR operations).
# See c_sglx_getstr().
# ok = c_sglx_getNIShankMap( byref(nval), hSglx )
#
c_sglx_getNIShankMap = sglx.c_sglx_getNIShankMap
c_sglx_getNIShankMap.restype = c_bool
c_sglx_getNIShankMap.argtypes = [POINTER(c_int), c_void_p]

# Get the most recently used run parameters.
# These are a set of 'key=value' strings.
# If successful, nval is the string count.
# See c_sglx_getstr().
# ok = c_sglx_getParams( byref(nval), hSglx )
#
c_sglx_getParams = sglx.c_sglx_getParams
c_sglx_getParams.restype = c_bool
c_sglx_getParams.argtypes = [POINTER(c_int), c_void_p]

# Get imec parameters common to all enabled probes.
# These are a set of 'key=value' strings.
# If successful, nval is the string count.
# See c_sglx_getstr().
# ok = c_sglx_getParamsImecCommon( byref(nval), hSglx )
#
c_sglx_getParamsImecCommon = sglx.c_sglx_getParamsImecCommon
c_sglx_getParamsImecCommon.restype = c_bool
c_sglx_getParamsImecCommon.argtypes = [POINTER(c_int), c_void_p]

# Get imec parameters for given logical probe.
# These are a set of 'key=value' strings.
# If successful, nval is the string count.
# See c_sglx_getstr().
# ok = c_sglx_getParamsImecProbe( byref(nval), hSglx, ip )
#
c_sglx_getParamsImecProbe = sglx.c_sglx_getParamsImecProbe
c_sglx_getParamsImecProbe.restype = c_bool
c_sglx_getParamsImecProbe.argtypes = [POINTER(c_int), c_void_p, c_int]

# Get parameters for selected OneBox.
# These are a set of 'key=value' strings.
# If successful, nval is the string count.
# See c_sglx_getstr().
#
# To reference a OneBox configured as a recording stream
# set ip to its stream-id; if ip >= 0, slot is ignored.
# Any selected OneBox can also be referenced by setting
# ip = -1, and giving its slot index.
# ok = c_sglx_getParamsOneBox( byref(nval), hSglx, ip, slot )
#
c_sglx_getParamsOneBox = sglx.c_sglx_getParamsOneBox
c_sglx_getParamsOneBox.restype = c_bool
c_sglx_getParamsOneBox.argtypes = [POINTER(c_int), c_void_p, c_int, c_int]

# Get string with format:
# (probeID,nShanks,partNumber)()...
# - A parenthesized entry for each selected probe.
# - probeID: zero-based integer.
# - nShanks: integer {1,4}.
# - partNumber: string, e.g., NP1000.
# - If no probes, return '()'.
# ok = c_sglx_getProbeList( byref(list), hSglx )
#
c_sglx_getProbeList = sglx.c_sglx_getProbeList
c_sglx_getProbeList.restype = c_bool
c_sglx_getProbeList.argtypes = [POINTER(c_char_p), c_void_p]

# Get run base name.
# ok = c_sglx_getRunName( byref(name), hSglx )
#
c_sglx_getRunName = sglx.c_sglx_getRunName
c_sglx_getRunName.restype = c_bool
c_sglx_getRunName.argtypes = [POINTER(c_char_p), c_void_p]

# For the selected substream, return the number of channels of
# each type that stream is acquiring. If successful, nval is the
# type count. See c_sglx_getint().
#
# js = 0: NI channels: {MN,MA,XA,DW}.
# js = 1: OB channels: {XA,DW,SY}.
# js = 2: IM channels: {AP,LF,SY}.
# ok = c_sglx_getStreamAcqChans( byref(nval), hSglx, js, ip )
#
c_sglx_getStreamAcqChans = sglx.c_sglx_getStreamAcqChans
c_sglx_getStreamAcqChans.restype = c_bool
c_sglx_getStreamAcqChans.argtypes = [POINTER(c_int), c_void_p, c_int, c_int]

# Return index of first sample in selected stream file,
# or zero if unavailable.
# samples = c_sglx_getStreamFileStart( hSglx, js, ip )
#
c_sglx_getStreamFileStart = sglx.c_sglx_getStreamFileStart
c_sglx_getStreamFileStart.restype = c_ulonglong
c_sglx_getStreamFileStart.argtypes = [c_void_p, c_int, c_int]

# Return multiplier converting 16-bit binary channel to volts.
# ok = c_sglx_getStreamI16ToVolts( byref(mult), hSglx, js, ip, chan )
#
c_sglx_getStreamI16ToVolts = sglx.c_sglx_getStreamI16ToVolts
c_sglx_getStreamI16ToVolts.restype = c_bool
c_sglx_getStreamI16ToVolts.argtypes = [POINTER(c_double), c_void_p, c_int, c_int, c_int]

# Return largest positive integer value for selected stream.
# ok = c_sglx_getStreamMaxInt( byref(maxint), hSglx, js, ip )
#
c_sglx_getStreamMaxInt = sglx.c_sglx_getStreamMaxInt
c_sglx_getStreamMaxInt.restype = c_bool
c_sglx_getStreamMaxInt.argtypes = [POINTER(c_int), c_void_p, c_int, c_int]

# Get number (np) of js-type substreams.
# For the given js, ip has range [0..np-1].
# ok = c_sglx_getStreamNP( byref(np), hSglx, js )
#
c_sglx_getStreamNP = sglx.c_sglx_getStreamNP
c_sglx_getStreamNP.restype = c_bool
c_sglx_getStreamNP.argtypes = [POINTER(c_int), c_void_p, c_int]

# Return number of samples since current run started,
# or zero if not running or error.
# samples = c_sglx_getStreamSampleCount( hSglx, js, ip )
#
c_sglx_getStreamSampleCount = sglx.c_sglx_getStreamSampleCount
c_sglx_getStreamSampleCount.restype = c_ulonglong
c_sglx_getStreamSampleCount.argtypes = [c_void_p, c_int, c_int]

# Return sample rate of selected stream in Hz, or zero if error.
# rate = c_sglx_getStreamSampleRate( hSglx, js, ip )
#
c_sglx_getStreamSampleRate = sglx.c_sglx_getStreamSampleRate
c_sglx_getStreamSampleRate.restype = c_double
c_sglx_getStreamSampleRate.argtypes = [c_void_p, c_int, c_int]

# Get a list containing the indices of the acquired channels
# that are being saved. If successful, nval is the list length.
# See c_sglx_getint().
# ok = c_sglx_getStreamSaveChans( byref(nval), hSglx, js, ip )
#
c_sglx_getStreamSaveChans = sglx.c_sglx_getStreamSaveChans
c_sglx_getStreamSaveChans.restype = c_bool
c_sglx_getStreamSaveChans.argtypes = [POINTER(c_int), c_void_p, c_int, c_int]

# js = 1: Get OneBox SN and slot.
# js = 2: Get probe  SN and type.
# SN = serial number string.
# ok = c_sglx_getStreamSN( byref(slot_or_type), byref(SN), hSglx, js, ip )
#
c_sglx_getStreamSN = sglx.c_sglx_getStreamSN
c_sglx_getStreamSN.restype = c_bool
c_sglx_getStreamSN.argtypes = [POINTER(c_int), POINTER(c_char_p), c_void_p, c_int, c_int]

# Get voltage range of selected data stream.
# ok = c_sglx_getStreamVoltageRange( byref(vMin), byref(vMax), hSglx, js, ip )
#
c_sglx_getStreamVoltageRange = sglx.c_sglx_getStreamVoltageRange
c_sglx_getStreamVoltageRange.restype = c_bool
c_sglx_getStreamVoltageRange.argtypes = [POINTER(c_double), POINTER(c_double), c_void_p, c_int, c_int]

# Return number of seconds since SpikeGLX application was launched,
# or zero if error.
# seconds = c_sglx_getTime( hSglx )
#
c_sglx_getTime = sglx.c_sglx_getTime
c_sglx_getTime.restype = c_double
c_sglx_getTime.argtypes = [c_void_p]

# Get SpikeGLX version string.
# c_char_p = c_sglx_getVersion( hSglx )
#
c_sglx_getVersion = sglx.c_sglx_getVersion
c_sglx_getVersion.restype = c_char_p
c_sglx_getVersion.argtypes = [c_void_p]

# Test if console window is hidden.
# ok = c_sglx_isConsoleHidden( byref(hid), hSglx )
#
c_sglx_isConsoleHidden = sglx.c_sglx_isConsoleHidden
c_sglx_isConsoleHidden.restype = c_bool
c_sglx_isConsoleHidden.argtypes = [POINTER(c_bool), c_void_p]

# Test if SpikeGLX has completed its startup initialization
# and is ready to run.
# ok = c_sglx_isInitialized( byref(ready), hSglx )
#
c_sglx_isInitialized = sglx.c_sglx_isInitialized
c_sglx_isInitialized.restype = c_bool
c_sglx_isInitialized.argtypes = [POINTER(c_bool), c_void_p]

# Test if SpikeGLX is currently acquiring data.
# ok = c_sglx_isRunning( byref(running), hSglx )
#
c_sglx_isRunning = sglx.c_sglx_isRunning
c_sglx_isRunning.restype = c_bool
c_sglx_isRunning.argtypes = [POINTER(c_bool), c_void_p]

# Test if SpikeGLX is currently running AND saving data.
# ok = c_sglx_isSaving( byref(saving), hSglx )
#
c_sglx_isSaving = sglx.c_sglx_isSaving
c_sglx_isSaving.restype = c_bool
c_sglx_isSaving.argtypes = [POINTER(c_bool), c_void_p]

# Test if graphs currently sorted in user order.
# This query is sent only to the main Graphs window.
# ok = c_sglx_isUserOrder( byref(user_order), hSglx, js, ip )
#
c_sglx_isUserOrder = sglx.c_sglx_isUserOrder
c_sglx_isUserOrder.restype = c_bool
c_sglx_isUserOrder.argtypes = [POINTER(c_bool), c_void_p, c_int, c_int]

# Return sample in dst stream corresponding to given sample in src stream,
# or zero if error.
# dstSample = c_sglx_mapSample( hSglx, dstjs, dstip, srcSample, srcjs, srcip )
#
c_sglx_mapSample = sglx.c_sglx_mapSample
c_sglx_mapSample.restype = c_ulonglong
c_sglx_mapSample.argtypes = [c_void_p, c_int, c_int, c_ulonglong, c_int, c_int]

# Set one or more NI lines high/low.
#   - lines is a string list of lines to set, e.g.:
#       'Dev6/port0/line2,Dev6/port0/line5'
#       'Dev6/port1/line0:3'
#       'Dev6/port1:2'
#   - bits is a uint32 value, each bit maps to a line:
#       The lowest 8 bits map to port 0.
#       The next higher 8 bits map to port 1, etc.
# ok = c_sglx_ni_DO_set( hSglx, lines, bits )
#
c_sglx_ni_DO_set = sglx.c_sglx_ni_DO_set
c_sglx_ni_DO_set.restype = c_bool
c_sglx_ni_DO_set.argtypes = [c_void_p, c_char_p, c_uint]

# General sequence:
# 1. NI_Wave_Load      : Load wave from SpikeGLX/_Waves folder.
# 2. NI_Wave_Arm       : Set triggering parameters.
# 3. NI_Wave_StartStop : Start if software trigger, stop when done.
#
# Set trigger method.
# - trigTerm is a string naming any trigger-capable terminal on your
#   device, e.g., '/dev1/pfi2'. NI-DAQ requires names of terminals to
#   start with a '/' character. This is indeed different than channel
#   names which do not start with a slash.
#   (1) Give a correct terminal string to trigger playback upon
#       receiving a rising edge at that terminal.
#   (2) Give any string that does NOT start with a '/' to trigger
#       playback via the NI_Wave_StartStop command.
# - Multiple trigger events can NOT be given. For each trigger
#   event after the first, you must first call NI_Wave_StartStop
#   AND NI_Wave_Arm to stop and then rearm the task.
#
# outChan is a string naming any wave-capable analog output
# channel on your device, e.g., 'dev1/ao1'.
# ok = c_sglx_ni_wave_arm( hSglx, outChan, trigTerm )
#
c_sglx_ni_wave_arm = sglx.c_sglx_ni_wave_arm
c_sglx_ni_wave_arm.restype = c_bool
c_sglx_ni_wave_arm.argtypes = [c_void_p, c_char_p, c_char_p]

# General sequence:
# 1. NI_Wave_Load      : Load wave from SpikeGLX/_Waves folder.
# 2. NI_Wave_Arm       : Set triggering parameters.
# 3. NI_Wave_StartStop : Start if software trigger, stop when done.
#
# Load a wave descriptor already placed in SpikeGLX/_Waves.
# - Pass 'mywavename' to this function; no path; no extension.
# - The playback loop_modes are: {1=loop until stopped, 0=once only}.
#
# outChan is a string naming any wave-capable analog output
# channel on your device, e.g., 'dev1/ao1'.
# ok = c_sglx_ni_wave_load( hSglx, outChan, wave, loop )
#
c_sglx_ni_wave_load = sglx.c_sglx_ni_wave_load
c_sglx_ni_wave_load.restype = c_bool
c_sglx_ni_wave_load.argtypes = [c_void_p, c_char_p, c_char_p, c_bool]

# General sequence:
# 1. NI_Wave_Load      : Load wave from SpikeGLX/_Waves folder.
# 2. NI_Wave_Arm       : Set triggering parameters.
# 3. NI_Wave_StartStop : Start if software trigger, stop when done.
#
# Start (optionally) or stop wave playback.
# - If you selected software triggering with NI_Wave_Arm,
#   then set start_bool=1 to start playback.
# - In all cases, set start_bool=0 to stop playback.
# - It is best to stop playback before changing wave parameters.
# - After playback or if looping mode is interrupted, the voltage
#   remains at the last output level.
#
# outChan is a string naming any wave-capable analog output
# channel on your device, e.g., 'dev1/ao1'.
# ok = c_sglx_ni_wave_startstop( hSglx, outChan, start )
#
c_sglx_ni_wave_startstop = sglx.c_sglx_ni_wave_startstop
c_sglx_ni_wave_startstop.restype = c_bool
c_sglx_ni_wave_startstop.argtypes = [c_void_p, c_char_p, c_bool]

# Set one or more OneBox AO (DAC) channel voltages.
# - chn_vlt is a string with format: (chan,volts)(chan,volts)...()
# - The chan values are integer AO indices in range [0,11].
# - You can only use AO channels already listed on the OBX setup tab.
# - Voltages are double values in range [-5.0,5.0] V.
# - DAC is 16-bit; theoretical resolution is (10 V)/(2^16) ~ .0001526 V.
# - Practical resolution, given noise, appears to be ~ 0.002 V.
# - AO channels are disabled at run start/end; voltage ~ 1.56 V.
#
# To reference a OneBox configured as a recording stream
# set ip to its stream-id; if ip >= 0, slot is ignored.
# Any selected OneBox can also be referenced by setting
# ip = -1, and giving its slot index.
# ok = c_sglx_obx_AO_set( hSglx, ip, slot, chn_vlt )
#
c_sglx_obx_AO_set = sglx.c_sglx_obx_AO_set
c_sglx_obx_AO_set.restype = c_bool
c_sglx_obx_AO_set.argtypes = [c_void_p, c_int, c_int, c_char_p]

# General sequence:
# 1. OBX_Wave_Load      : Load wave from SpikeGLX/_Waves folder.
# 2. OBX_Wave_Arm       : Set triggering parameters.
# 3. OBX_Wave_StartStop : Start if software trigger, stop when done.
#
# Set trigger method, and playback loop mode.
# - Trigger values...Playback starts:
#     -2   : By calling OBX_Wave_StartStop.
#     -1   : When TTL rising edge sent to SMA1.
#     0-11 : When TTL rising edge sent to that XA (ADC) channel.
# - To use an ADC channel, you must name it as an XA channel on
#   the OBX setup tab of the Acquisition Configuration dialog.
# - Multiple trigger events (either hardware or software) can be
#   given without needing to rearm.
# - The playback loop modes are: {1=loop until stopped, 0=once only}.
#
# To reference a OneBox configured as a recording stream
# set ip to its stream-id; if ip >= 0, slot is ignored.
# Any selected OneBox can also be referenced by setting
# ip = -1, and giving its slot index.
# ok = c_sglx_obx_wave_arm( hSglx, ip, slot, trig, loop )
#
c_sglx_obx_wave_arm = sglx.c_sglx_obx_wave_arm
c_sglx_obx_wave_arm.restype = c_bool
c_sglx_obx_wave_arm.argtypes = [c_void_p, c_int, c_int, c_int, c_bool]

# General sequence:
# 1. OBX_Wave_Load      : Load wave from SpikeGLX/_Waves folder.
# 2. OBX_Wave_Arm       : Set triggering parameters.
# 3. OBX_Wave_StartStop : Start if software trigger, stop when done.
#
# Load a wave descriptor already placed in SpikeGLX/_Waves.
# - Pass 'mywavename' to this function; no path; no extension.
#
# To reference a OneBox configured as a recording stream
# set ip to its stream-id; if ip >= 0, slot is ignored.
# Any selected OneBox can also be referenced by setting
# ip = -1, and giving its slot index.
# ok = c_sglx_obx_wave_load( hSglx, ip, slot, wave )
#
c_sglx_obx_wave_load = sglx.c_sglx_obx_wave_load
c_sglx_obx_wave_load.restype = c_bool
c_sglx_obx_wave_load.argtypes = [c_void_p, c_int, c_int, c_char_p]

# General sequence:
# 1. OBX_Wave_Load      : Load wave from SpikeGLX/_Waves folder.
# 2. OBX_Wave_Arm       : Set triggering parameters.
# 3. OBX_Wave_StartStop : Start if software trigger, stop when done.
#
# Start (optionally) or stop wave playback.
# - If you selected software triggering with OBX_Wave_Arm,
#   then set start_bool=1 to start playback.
# - In all cases, set start_bool=0 to stop playback.
# - It is best to stop playback before changing wave parameters.
# - Waves only play at AO (DAC) channel-0.
# - To use the waveplayer, you must name channel AO-0 on
#   the OBX setup tab of the Acquisition Configuration dialog.
# - After playback or if looping mode is interrupted, the voltage
#   remains at the last output level.
#
# To reference a OneBox configured as a recording stream
# set ip to its stream-id; if ip >= 0, slot is ignored.
# Any selected OneBox can also be referenced by setting
# ip = -1, and giving its slot index.
# ok = c_sglx_obx_wave_startstop( hSglx, ip, slot, start )
#
c_sglx_obx_wave_startstop = sglx.c_sglx_obx_wave_startstop
c_sglx_obx_wave_startstop.restype = c_bool
c_sglx_obx_wave_startstop.argtypes = [c_void_p, c_int, c_int, c_bool]

# Direct emission to specified site (-1=dark).
# ip:    imec probe index.
# color: {0=blue, 1=red}.
# site:  [0..13], or, -1=dark.
# ok = c_sglx_opto_emit( hSglx, ip, color, site )
#
c_sglx_opto_emit = sglx.c_sglx_opto_emit
c_sglx_opto_emit.restype = c_bool
c_sglx_opto_emit.argtypes = [c_void_p, c_int, c_int, c_int]

# Get array of 14 (double) site power attenuation factors.
# ip:    imec probe index.
# color: {0=blue, 1=red}.
#
# If successful, nval is the 14. See c_sglx_getdbl().
# ok = c_sglx_opto_getAttenuations( byref(nval), hSglx, ip, color )
#
c_sglx_opto_getAttenuations = sglx.c_sglx_opto_getAttenuations
c_sglx_opto_getAttenuations.restype = c_bool
c_sglx_opto_getAttenuations.argtypes = [POINTER(c_int), c_void_p, c_int, c_int]

# Create, verify, or repair Par2 redundancy files for 'file'.
#
# Arguments:
# op:      {'c'=create, 'v'=verify, 'r'=repair}.
# file:    Path to a .par2 or .bin file to which 'op' is applied.
#
# Status/progress lines are reported to optional callback.
# ok = c_sglx_par2( callback, hSglx, ord(op), filename )
#
c_sglx_par2 = sglx.c_sglx_par2
c_sglx_par2.restype = c_bool
c_sglx_par2.argtypes = [T_sglx_callback, c_void_p, c_char, c_char_p]

# Pause Graphs window displays.
# Note: The displays are updated at ~10 Hz.
# ok = c_sglx_pause_graphs( hSglx, pause )
#
c_sglx_pause_graphs = sglx.c_sglx_pause_graphs
c_sglx_pause_graphs.restype = c_bool
c_sglx_pause_graphs.argtypes = [c_void_p, c_bool]

# Set anatomy data string with Pinpoint format:
# [probe-id,shank-id](startpos,endpos,R,G,B,rgnname)(startpos,endpos,R,G,B,rgnname)...()
#    - probe-id: SpikeGLX logical probe id.
#    - shank-id: [0..n-shanks].
#    - startpos: region start in microns from tip.
#    - endpos:   region end in microns from tip.
#    - R,G,B:    region color as RGB, each [0..255].
#    - rgnname:  region name text.
# ok = c_sglx_setAnatomy_Pinpoint( hSglx, shankdat )
#
c_sglx_setAnatomy_Pinpoint = sglx.c_sglx_setAnatomy_Pinpoint
c_sglx_setAnatomy_Pinpoint.restype = c_bool
c_sglx_setAnatomy_Pinpoint.argtypes = [c_void_p, c_char_p]

# Set audio output on/off. Note that this command has
# no effect if not currently running.
# ok = c_sglx_setAudioEnable( hSglx, enable )
#
c_sglx_setAudioEnable = sglx.c_sglx_setAudioEnable
c_sglx_setAudioEnable.restype = c_bool
c_sglx_setAudioEnable.argtypes = [c_void_p, c_bool]

# Set subgroup of parameters for audio-out operation. Parameters
# are key-value pairs. See c_sglx_setkv(). This call stops current
# output. Call c_sglx_setAudioEnable() to restart it.
# ok = c_sglx_setAudioKVParams( hSglx, group )
#
c_sglx_setAudioKVParams = sglx.c_sglx_setAudioKVParams
c_sglx_setAudioKVParams.restype = c_bool
c_sglx_setAudioKVParams.argtypes = [c_void_p, c_char_p]

# Set ith global data directory.
# Set required parameter idir to zero for main data directory.
# ok = c_sglx_setDataDir( hSglx, idir, dir )
#
c_sglx_setDataDir = sglx.c_sglx_setDataDir
c_sglx_setDataDir.restype = c_bool
c_sglx_setDataDir.argtypes = [c_void_p, c_int, c_char_p]

# If a run is in progress, set metadata to be added to
# the next output file-set. Metadata are key-value pairs.
# See c_sglx_setkv().
# ok = c_sglx_setKVMetadata( hSglx )
#
c_sglx_setKVMetadata = sglx.c_sglx_setKVMetadata
c_sglx_setKVMetadata.restype = c_bool
c_sglx_setKVMetadata.argtypes = [c_void_p]

# The inverse of c_sglx_getParams, this sets run parameters.
# Parameters are key-value pairs. See c_sglx_setkv(). The call
# will error if a run is currently in progress.
#
# Note: You can set any subset of [DAQSettings].
# ok = c_sglx_setKVParams( hSglx )
#
c_sglx_setKVParams = sglx.c_sglx_setKVParams
c_sglx_setKVParams.restype = c_bool
c_sglx_setKVParams.argtypes = [c_void_p]

# The inverse of c_sglx_getParamsImecCommon, this sets parameters
# common to all enabled probes. Parameters are key-value pairs.
# See c_sglx_setkv(). The call will error if a run is currently
# in progress.
#
# Note: You can set any subset of [DAQ_Imec_All].
# ok = c_sglx_setKVParamsImecCommon( hSglx )
#
c_sglx_setKVParamsImecCommon = sglx.c_sglx_setKVParamsImecCommon
c_sglx_setKVParamsImecCommon.restype = c_bool
c_sglx_setKVParamsImecCommon.argtypes = [c_void_p]

# The inverse of c_sglx_getParamsImecProbe, this sets parameters
# for a given logical probe. Parameters are key-value pairs.
# See c_sglx_setkv(). The call will error if file writing is
# currently in progress.
#
# Note: You can set any subset of fields under [SerialNumberToProbe]/SNjjj.
# ok = c_sglx_setKVParamsImecProbe( hSglx, ip )
#
c_sglx_setKVParamsImecProbe = sglx.c_sglx_setKVParamsImecProbe
c_sglx_setKVParamsImecProbe.restype = c_bool
c_sglx_setKVParamsImecProbe.argtypes = [c_void_p, c_int]

# The inverse of c_sglx_getParamsOneBox, this sets params
# for a selected OneBox. Parameters are key-value pairs.
# See c_sglx_setkv(). The call will error if a run is currently
# in progress.
#
# To reference a OneBox configured as a recording stream
# set ip to its stream-id; if ip >= 0, slot is ignored.
# Any selected OneBox can also be referenced by setting
# ip = -1, and giving its slot index.
#
# Note: You can set any subset of fields under [SerialNumberToOneBox]/SNjjj.
# ok = c_sglx_setKVParamsOneBox( hSglx, ip, slot )
#
c_sglx_setKVParamsOneBox = sglx.c_sglx_setKVParamsOneBox
c_sglx_setKVParamsOneBox.restype = c_bool
c_sglx_setKVParamsOneBox.argtypes = [c_void_p, c_int, c_int]

# Set multi-drive run-splitting on/off.
# ok = c_sglx_setMultiDriveEnable( hSglx, enable )
#
c_sglx_setMultiDriveEnable = sglx.c_sglx_setMultiDriveEnable
c_sglx_setMultiDriveEnable.restype = c_bool
c_sglx_setMultiDriveEnable.argtypes = [c_void_p, c_bool]

# For only the next trigger (file writing event) this overrides
# all auto-naming, giving you complete control of where to save
# the files, the file name, and what g- and t-indices you want
# (if any). For example, regardless of the run's current data dir,
# run name and indices, if you set: 'otherdir/yyy_g5/yyy_g5_t7',
# SpikeGLX will save the next files in flat directory yyy_g5/:
#    - otherdir/yyy_g5/yyy.g5_t7.nidq.bin,meta
#    - otherdir/yyy_g5/yyy.g5_t7.imec0.ap.bin,meta
#    - otherdir/yyy_g5/yyy.g5_t7.imec0.lf.bin,meta
#    - otherdir/yyy_g5/yyy.g5_t7.imec1.ap.bin,meta
#    - otherdir/yyy_g5/yyy.g5_t7.imec1.lf.bin,meta
#    - etc.
#
# - The destination directory must already exist...No parent directories
# or probe subfolders are created in this naming mode.
# - The run must already be in progress.
# - Neither the custom name nor its indices are displayed in the Graphs
# window toolbars. Rather, the toolbars reflect standard auto-names.
# - After writing this file set, the override is cleared and auto-naming
# will resume as if you never called c_sglx_setNextFileName. You have to call
# c_sglx_setNextFileName before each trigger event to create custom trial series.
# For example, you can build a software-triggered t-series using sequence:
#    + c_sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t0' )
#    + c_sglx_setRecordingEnable( 1 )
#    + c_sglx_setRecordingEnable( 0 )
#    + c_sglx_setNextFileName( 'otherdir/yyy_g0/yyy_g0_t1' )
#    + c_sglx_setRecordingEnable( 1 )
#    + c_sglx_setRecordingEnable( 0 )
#    + etc.
# ok = c_sglx_setNextFileName( hSglx, name )
#
c_sglx_setNextFileName = sglx.c_sglx_setNextFileName
c_sglx_setNextFileName.restype = c_bool
c_sglx_setNextFileName.argtypes = [c_void_p, c_char_p]

# Set gate (file writing) on/off during run.
#
# When auto-naming is in effect, opening the gate advances
# the g-index and resets the t-index to zero. Auto-naming is
# on unless c_sglx_setNextFileName has been used to override it.
# ok = c_sglx_setRecordingEnable( hSglx, enable )
#
c_sglx_setRecordingEnable = sglx.c_sglx_setRecordingEnable
c_sglx_setRecordingEnable.restype = c_bool
c_sglx_setRecordingEnable.argtypes = [c_void_p, c_bool]

# Set the run name for the next time files are created
# (either by trigger, c_sglx_setRecordingEnable() or by
# c_sglx_startRun()).
# ok = c_sglx_setRunName( hSglx, name )
#
c_sglx_setRunName = sglx.c_sglx_setRunName
c_sglx_setRunName.restype = c_bool
c_sglx_setRunName.argtypes = [c_void_p, c_char_p]

# During a run, set frequency and duration of Windows
# beep signaling file closure. hertz=0 disables the beep.
# ok = c_sglx_setTriggerOffBeep( hSglx, hertz, millisec )
#
c_sglx_setTriggerOffBeep = sglx.c_sglx_setTriggerOffBeep
c_sglx_setTriggerOffBeep.restype = c_bool
c_sglx_setTriggerOffBeep.argtypes = [c_void_p, c_int, c_int]

# During a run set frequency and duration of Windows
# beep signaling file creation. hertz=0 disables the beep.
# ok = c_sglx_setTriggerOnBeep( hSglx, hertz, millisec )
#
c_sglx_setTriggerOnBeep = sglx.c_sglx_setTriggerOnBeep
c_sglx_setTriggerOnBeep.restype = c_bool
c_sglx_setTriggerOnBeep.argtypes = [c_void_p, c_int, c_int]

# Start data acquisition run. Last-used parameters remain
# in effect. An error is flagged if already running. The
# name parameter is optional; set "" to use existing.
# ok = c_sglx_startRun( hSglx, name )
#
c_sglx_startRun = sglx.c_sglx_startRun
c_sglx_startRun.restype = c_bool
c_sglx_startRun.argtypes = [c_void_p, c_char_p]

# Unconditionally stop current run, close data files
# and return to idle state.
# ok = c_sglx_stopRun( hSglx )
#
c_sglx_stopRun = sglx.c_sglx_stopRun
c_sglx_stopRun.restype = c_bool
c_sglx_stopRun.argtypes = [c_void_p]

# Using standard auto-naming, set both the gate (g) and
# trigger (t) levels that control file writing.
#   -1 = no change.
#    0 = set low.
#    1 = increment and set high.
# E.g., c_sglx_triggerGT( -1, 1 ) = same g, increment t, start writing.
#
# - c_sglx_triggerGT only affects the 'Remote controlled' gate type
# and/or the 'Remote controlled' trigger type.
# - The 'Enable Recording' button, when shown, is a master override
# switch. c_sglx_triggerGT is blocked until you click the button or call
# c_sglx_setRecordingEnable.
# ok = c_sglx_triggerGT( hSglx, g, t )
#
c_sglx_triggerGT = sglx.c_sglx_triggerGT
c_sglx_triggerGT.restype = c_bool
c_sglx_triggerGT.argtypes = [c_void_p, c_int, c_int]

# Verifies the SHA1 sum of the file specified by filename.
# If filename is relative, it is appended to the run dir.
# Absolute path/filenames are also supported. Status/progress
# lines are reported to optional callback.
#
# Return true if verified (and no errors).
# ok = c_sglx_verifySha1( callback, hSglx, filename )
#
c_sglx_verifySha1 = sglx.c_sglx_verifySha1
c_sglx_verifySha1.restype = c_bool
c_sglx_verifySha1.argtypes = [T_sglx_callback, c_void_p, c_char_p]


