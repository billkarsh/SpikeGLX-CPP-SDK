
1. Set up the API by running make-install.sh in the API folder.
2. Edit DemoRemoteAPI.py to set variable: 'sglx_addr.'
3. Start SpikeGLX.
4. Test out functions in __main__ of DemoRemoteAPI.py.

If the library isn't found, edit LD_LIBRARY_PATH (see GettingStarted.txt).

Generally, to create your own project, just place sglx_pkg at the top level with your own files alongside:

myProject/
    + sglx_pkg
    + myfile.py
    + myfile2.py


