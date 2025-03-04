## Discrete Element Method Model of Vacuum Packed Particle Dampers

This work provides an extension to YadeDEM that enables modelling the Vacuum Packed Particle Damper systems.

Software was tested on Linux Debian 8

https://cdimage.debian.org/mirror/cdimage/archive/8.0.0-live/amd64/iso-hybrid/

Software was compiletd from sources YADE DEM 1.14.0

https://launchpad.net/yade/trunk/yade-1.00.0/+download/yade-1.14.0.tar.gz

Instalation:
- Install YADE DEM 1.14 from the source code
- Download the files from the repository and place them in the appropriate directories
- Recompile the source code
- Use the included example scripts to run the VPP damper analysis



## Mathematica postprocessing 
Download the file postprocessing.m and the required files from 10.5281/zenodo.14917354

## Analysis file 
A sample file with a programmed analysis of the VPP damper has been placed in the repository.

To conduct an analysis of the sample damper, after successfully compiling the program, run the code in the console.
./install/bin/yade-1.14.0 -j4 vpp_auto_batch.py 

In the file *yade_batch*, you will find an example of how to start the analysis with various parameters, one by one.

## Images from analysis
![Screenshot of the sample analysis.](https://github.com/chpawel/VPP/blob/main/images/sample_A.PNG)

![Screenshot of the sample analysis.](https://github.com/chpawel/VPP/blob/main/images/sample_B.PNG)
