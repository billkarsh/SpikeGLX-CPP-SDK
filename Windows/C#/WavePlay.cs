using System;
using System.Threading;
using C_Sglx_namespace;

namespace WavePlay_namespace {

    public class Demos {

        // Edit the server address/port here
        static string addr = "localhost";
        static int port = 4142;


        // Plays wave 'jwave' at NI channel AO-0.
        // Playback is triggered by software command.
        //
        public static void wp_ni_soft_start()
        {
            // Handle to SpikeGLX
            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                // Load the wave plan, select infinite looping
                string wave_name = "jwave";
                string outChan   = "PXI1Slot6_2/ao0";
                int    looping   = 1;
                ok = C_Sglx.c_sglx_ni_wave_load(hSglx, outChan, wave_name, looping);
                if (ok == 0)
                    goto error;

                // Select software triggering
                string trigTerm = "software";
                ok = C_Sglx.c_sglx_ni_wave_arm(hSglx, outChan, trigTerm);
                if (ok == 0)
                    goto error;

                // Start playback now
                int start = 1;
                ok = C_Sglx.c_sglx_ni_wave_startstop(hSglx, outChan, start);
                if (ok == 0)
                    goto error;

                // This section demonstrates a method to capture your
                // wave plan in action. The best sleep parameters will
                // depend upon the duration of your wave plan and how
                // fast your SpikeGLX graphs are sweeping
                //
                // Let this play for 1 second
                // Then sleep the SpikeGLX Graphs Window for 2 seconds
                // Then resume Graphs for 5 seconds
                Thread.Sleep(1000);
                C_Sglx.c_sglx_pause_graphs(hSglx, 1);
                Thread.Sleep(2000);
                C_Sglx.c_sglx_pause_graphs(hSglx, 0);
                Thread.Sleep(5000);

                // Stop playback
                start = 0;
                ok = C_Sglx.c_sglx_ni_wave_startstop(hSglx, outChan, start);
            }

error:
            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Plays wave 'jwave' at OneBox channel AO-0.
        // Playback is triggered by software command.
        //
        public static void wp_soft_start()
        {
            // Handle to SpikeGLX
            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                // For demo purposes we assume the OneBox is not recording...
                // So we refer to it using its slot index
                int ip = -1,
                    slot = 21;

                // Load the wave plan
                string wave_name = "jwave";
                ok = C_Sglx.c_sglx_obx_wave_load(hSglx, ip, slot, wave_name);
                if (ok == 0)
                    goto error;

                // Select software triggering and infinite looping
                int trigger = -2,
                    looping = 1;
                ok = C_Sglx.c_sglx_obx_wave_arm(hSglx, ip, slot, trigger, looping);
                if (ok == 0)
                    goto error;

                // Start playback now, output is always at AO-0
                int start = 1;
                ok = C_Sglx.c_sglx_obx_wave_startstop(hSglx, ip, slot, start);
                if (ok == 0)
                    goto error;

                // This section demonstrates a method to capture your
                // wave plan in action. The best sleep parameters will
                // depend upon the duration of your wave plan and how
                // fast your SpikeGLX graphs are sweeping
                //
                // Let this play for 1 second
                // Then sleep the SpikeGLX Graphs Window for 2 seconds
                // Then resume Graphs for 5 seconds
                Thread.Sleep(1000);
                C_Sglx.c_sglx_pause_graphs(hSglx, 1);
                Thread.Sleep(2000);
                C_Sglx.c_sglx_pause_graphs(hSglx, 0);
                Thread.Sleep(5000);

                // Stop playback
                start = 0;
                ok = C_Sglx.c_sglx_obx_wave_startstop(hSglx, ip, slot, start);
            }

error:
            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }


        // Plays wave 'jwave' at OneBox channel AO-0.
        // Playback is triggered when OneBox channel AI-1 goes high.
        // User needs to list AI-1 in the XA box of OBX Setup tab.
        // We will configure NI device to send TTL rising edge from line-4.
        // User needs to wire NI line-4 to OneBox AI-1.
        //
        public static void wp_trig_start()
        {
            // Handle to SpikeGLX
            IntPtr hSglx = C_Sglx.c_sglx_createHandle();
            int ok = C_Sglx.c_sglx_connect(hSglx, addr, port);

            if (ok == 1)
            {
                // OneBox assumed to be recording stream ip=0...
                // So the slot number is ignored in this case
                int ip   = 0,
                    slot = -1;

                // Load the wave plan
                string wave_name = "jwave";
                ok = C_Sglx.c_sglx_obx_wave_load(hSglx, ip, slot, wave_name);
                if (ok == 0)
                    goto error;

                // Select AI-1 triggering and no looping
                int trigger = 1,
                    looping = 0;
                ok = C_Sglx.c_sglx_obx_wave_arm(hSglx, ip, slot, trigger, looping);
                if (ok == 0)
                    goto error;

                // Configure NI line-4
                string digOutTerm = "/PXI1Slot6_2/line4";
                uint digBits = 0x10; // binary 1 at bit-4, zero elsewhere
                ok = C_Sglx.c_sglx_ni_DO_set(hSglx, digOutTerm, 0);
                if (ok == 0)
                    goto error;

                // Start playback now, output is always at AO-0
                ok = C_Sglx.c_sglx_ni_DO_set(hSglx, digOutTerm, digBits);
                if (ok == 0)
                    goto error;

                // Reset trigger after 50 ms
                Thread.Sleep(50);
                ok = C_Sglx.c_sglx_ni_DO_set(hSglx, digOutTerm, 0);
                if (ok == 0)
                    goto error;

                // This section demonstrates a method to capture your
                // wave plan in action. The best sleep parameters will
                // depend upon the duration of your wave plan and how
                // fast your SpikeGLX graphs are sweeping
                //
                // Let this play for 1 second
                // Then sleep the SpikeGLX Graphs Window for 2 seconds
                // Then resume Graphs
                Thread.Sleep(1000);
                C_Sglx.c_sglx_pause_graphs(hSglx, 1);
                Thread.Sleep(2000);
                C_Sglx.c_sglx_pause_graphs(hSglx, 0);

                // Stop playback
                int start = 0;
                ok = C_Sglx.c_sglx_obx_wave_startstop(hSglx, ip, slot, start);
            }

error:
            if (ok == 0)
                Console.WriteLine("error [{0}]\n", C_Sglx.cs_sglx_getError(hSglx));

            C_Sglx.c_sglx_close(hSglx);
            C_Sglx.c_sglx_destroyHandle(hSglx);
        }
    }
}


