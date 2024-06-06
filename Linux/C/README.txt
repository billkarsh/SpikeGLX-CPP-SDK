
1. Set up the API by running make-install.sh in the API folder.
2. Edit DemoRemoteAPI.c to set variable: 'addr.'
3. Edit main.c to call the desired demo function.
4. Read make.sh; note that we are copying API files into this folder.
5. Run make.sh to produce test program 'a.out.'
6. Start SpikeGLX.
7. Run a.out.

If the library isn't found, edit LD_LIBRARY_PATH (see GettingStarted.txt).
