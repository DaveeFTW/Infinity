SDK for 4.01 M33

This update adds the function sctrlHENLoadModuleOnReboot which may be used for programs that 
need to have reboot control (e.g. shells).

A sample is included in samples/bootload.

Copy EBOOT.PBP and rdriver.prx to /PSP/GAME4XX/bootload (the sample will not work in other path).
The sample demonstrates how to load homebrew, isos, and psx games, while still having the control
of returning to your application.

-----

psp-packer, this tool is to compress prx's and pbp's. This is only for 3.xx and 4.xx kernel.
What the tool can compress and its compatibility:

kernel prx: yes, compatible with 3.71 M33-3 and higher
PBP user: yes, compatible with 3.71 M33-1 or 3 (i don't remember) and higher
PBP kernel and PBP vsh: no
user prx: yes, compatible with 4.01 M33 and higher
vsh prx: no.

The usage of the tool is very simple:

psp-packer myprx.prx -> it will compress myprx.prx in place
psp-packer myprx.prx myprx_packed.prx -> it will compress myprx.prx into myprx_packed.prx

If you want the prx to have a sce_header, add the "-s" switch before the other parameters.

Note: pbps need to have 0x200 flags. If the user pbp has not those flags, the program will ask
to convert it, this shouldn't cause any problem to the program.

Windows binary is included. In linux x86, just type make and alternativelly make install.
It won't work on big endian systems.
