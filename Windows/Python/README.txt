
1. There is nothing to build.
2. Copy SglxApi.dll from the API to the sglx_pkg folder.
3. Edit DemoRemoteAPI.py to set variable: 'sglx_addr.'
3. Start SpikeGLX.
4. Test out functions in __main__ of DemoRemoteAPI.py.

Generally, to create your own project, just place sglx_pkg at the top level with your own files alongside:

myProject/
    + sglx_pkg
    + myfile.py
    + myfile2.py


