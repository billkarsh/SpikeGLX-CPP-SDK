
1. There is nothing to build.
2. Copy these DLLs from the API to the sglx_pkg folder:
    + libgcc_s_seh-1.dll
    + libstdc++-6.dll
    + libwinpthread-1.dll
    + SglxApi.dll
3. Edit DemoRemoteAPI.py to set variable: 'sglx_addr.'
3. Start SpikeGLX.
4. Test out functions in __main__ of DemoRemoteAPI.py.

Generally, to create your own project, just place sglx_pkg at the top level with your own files alongside:

myProject/
    + sglx_pkg
    + myfile.py
    + myfile2.py


