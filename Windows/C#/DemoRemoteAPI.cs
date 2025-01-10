// =================================
// Random assortment of remote calls
// =================================

using System;
using C_Sglx_namespace;

namespace DemoRemoteAPI_namespace {

    public class Tryit {

        // Edit the server address/port here
        static string addr = "localhost";
        static int port = 4142;

        public static void justConnect()
        {
            Console.WriteLine("\nCalling connect...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();

            // Using default loopback address and port

            if (1 == C_Sglx.c_sglx_connect(hSglx, addr, port))
                Console.WriteLine("version <{0}>\n", C_Sglx.cs_sglx_getVersion(hSglx));
            else
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Simple query.
        //
        public static void console_test()
        {
            Console.WriteLine("\nConsole test...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                byte hid;

                ok = C_Sglx.c_sglx_isConsoleHidden(out hid, hSglx);
                if (ok == 1)
                    Console.WriteLine("Console hidden: {0}\n", hid);
            }

            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // SHA1.
        //
        public static void verify_test()
        {
            Console.WriteLine("\nVerify test...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                string filename = "D:\\AAA\\TT_g0\\TT_g0_imec0\\TT_g0_t0.imec0.ap.bin";
                ok = C_Sglx.c_sglx_verifySha1(C_Sglx.cs_demoCallback, hSglx, filename);
            }

            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Par2.
        //
        public static void par2_test()
        {
            Console.WriteLine("\nPar2 test...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                string filename = "D:\\AAA\\TT_g0\\TT_g0_imec0\\TT_g0_t0.imec0.ap.bin";
                ok = C_Sglx.c_sglx_par2(C_Sglx.cs_demoCallback, hSglx, 'c', filename);
            }

            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Query.
        //
        public static void getParams_test()
        {
            Console.WriteLine("\nGet params...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                int nval;

                ok = C_Sglx.c_sglx_getParams(out nval, hSglx);
                if (ok == 1)
                {
                    for (int i = 0; i < nval; ++i)
                        Console.WriteLine(C_Sglx.cs_sglx_getstr(hSglx, i));
                }
            }

            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Structure.
        //
        public static void getShankMap_test()
        {
            Console.WriteLine("\nGet shank map...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                int nval;

                ok = C_Sglx.c_sglx_getNIShankMap(out nval, hSglx);
                if (ok == 1)
                {
                    for (int i = 0; i < nval; ++i)
                        Console.WriteLine(C_Sglx.cs_sglx_getstr(hSglx, i));
                }
            }

            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Set the user notes for the next run; read that back.
        //
        public static void set_get_userNotes_test()
        {
            Console.WriteLine("\nSet, get notes...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                // Make sure we also set a new run name.
                string  run;

                ok = C_Sglx.cs_sglx_getRunName(out run, hSglx);
                if (ok == 0)
                    goto error;

                C_Sglx.c_sglx_setkv(hSglx, "", "");
                C_Sglx.c_sglx_setkv(hSglx, "snsNotes", "Test setting of run comment field.");
                C_Sglx.c_sglx_setkv(hSglx, "snsRunName", run + "_v2");

                ok = C_Sglx.c_sglx_setKVParams(hSglx);
                if (ok == 0)
                    goto error;

                int nval;

                ok = C_Sglx.c_sglx_getParams(out nval, hSglx);
                if (ok == 0)
                    goto error;

                for (int i = 0; i < nval; ++i)
                {
                    string s = C_Sglx.cs_sglx_getstr(hSglx, i);

                    if (s.StartsWith("snsNotes"))
                    {
                        Console.WriteLine(s + "\n");
                        break;
                    }
                }
            }
error:
            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Fetch and plot latest 1 second of NI channel AI0.
        // Run SpikeGLX_NISIM.exe which generates a 1 Hz sine wave on AI0.
        //
        public static void plot_NI_1sec_test()
        {
            Console.WriteLine("\nPlot NI AI0...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                // NI stream is (js,ip) = (0,0)

                double srate = C_Sglx.c_sglx_getStreamSampleRate(hSglx, 0, 0);
                ulong headCt;
                short[] data;
                int[] channels = { 0 };
                int nC = 1;

                if (srate == 0)
                {
                    ok = 0;
                    goto error;
                }

                Console.WriteLine("Sample rate {0}\n", srate);

                headCt = C_Sglx.cs_sglx_fetchLatest(out data, hSglx, 0, 0, (int)srate, channels, 1);
                if (headCt == 0)
                {
                    ok = 0;
                    goto error;
                }

                int nt = data.Length / nC;

                Console.WriteLine("Head count {0}, samples {1}\n", headCt, nt);

                for (int it = 0; it < nt; it += 250)
                {
                    int spc = 40 + (80 * data[it*nC]) / 32768;
                    string s = "";

                    for (int i = 0; i < spc; ++i)
                        s += ' ';
                    s += '*';

                    Console.WriteLine(s);
                }
            }

error:
            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
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
        // direct readout of closed-loop latency. We measure the typical
        // closed-loop latency to be 5.5 ms using the Cpp API.
        //
        public static void latency_test()
        {
            Console.WriteLine("\nLatency test...\n\n");

            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                double mv2i16 = 1.0 / (1200.0 / 250 / 1024);
                ulong fromCt;
                short[] data;
                int[] channels = new int[385];
                string line = "Dev4/port0/line5";
                uint bits = 0;
                int js = 2,
                    ip = 0,
                    nC = 385,
                    id = 393 - 384,
                    thresh = (int)(0.45 * mv2i16);

                Console.WriteLine("Threshold {0}\n", thresh);

                for (int i = 384; i < 769; ++i)
                    channels[i - 384] = i;

                fromCt = C_Sglx.c_sglx_getStreamSampleCount(hSglx, js, ip);
                if (fromCt == 0)
                {
                    ok = 0;
                    goto error;
                }

                ok = C_Sglx.c_sglx_ni_DO_set(hSglx, line, bits);
                if (ok == 0)
                    goto error;

                for (;;)
                {
                    ulong headCt;

                    headCt = C_Sglx.cs_sglx_fetch(out data, hSglx, js, ip, fromCt, 120, channels, 1);
                    if (headCt == 0)
                    {
                        ok = 0;
                        goto error;
                    }

                    int tpts = data.Length / nC;

                    if (tpts > 1)
                    {
                        int diff = data[id + (tpts - 1) * nC] - data[id],
                            digOK = 1;

                        if (diff > thresh && bits == 0)
                        {
                            bits = 0xFF;
                            digOK = C_Sglx.c_sglx_ni_DO_set(hSglx, line, bits);
                        }
                        else if (diff < -thresh && bits == 0xFF)
                        {
                            bits = 0;
                            digOK = C_Sglx.c_sglx_ni_DO_set(hSglx, line, bits);
                        }

                        if (digOK == 0)
                            goto error;

                        fromCt = headCt + (ulong)tpts;
                    }
                }
            }

error:
            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }
    }
}


