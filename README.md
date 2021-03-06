# GeoFLOW Data Converter

### Overview

This project converts [GeoFLOW](https://github.com/NOAA-GSL/GeoFLOW) (Geo FLuid Object Workbench) data files to other data formats like [NetCDF UGRID](https://github.com/ugrid-conventions/ugrid-conventions). 

GeoFLOW is an object-oriented software development effort by researchers in the High Performance Computing Branch of the Global Systems Laboratory (GSL) at NOAA that:
- Used for evaluating different aspects of Dynamical Cores in regional and global weather prediction
- Applies Spectral Element Methods (SEM) to the solution of partial differential equations relevant to geophysical sciences. SEM is a Finite Element Method (FEM) that uses high degree piecewise polynomials as basis functions
- Examines in detail tradeoffs in accuracy vs. performance
- Investigates the architectural, engineering and scientific challenges that arise with exascale computing

For the purposes of this converter, a GeoFLOW dataset consists of grid variable files (x-axis, y-axis, z-axis) and field variable files structured in the GeoFLOW data format. The GeoFLOW data converter reads in each of these files and converts them into a set of NetCDF UGRID-compliant `.nc` files. See the Appendix below for more info on GeoFLOW data files.

### Repo Contents

- `gf-data-converter-job.sh`: The batch job script used when running on the Hera supercomputer
- `include/src`: Source code for the project
- `Makefile`: Makefile used when compiling on a Desktop system
- `Makefile-hera`: Makefile used when compiling on the Hera supercomputer
- `README.md`: Installation and usage instructions
- `README-json.md`: How to configure the input `.json` that contains specifications for various meta-data needed to run the converter
- `README-vapor.md`: How to use NCAR's Vapor tool to read GeoFLOW UGRID files
- `test-data`: Example test datasets for 2D and 3D spherical data and for box grid data. For each example, there is an input `.json` file, an input folder with the GeoFLOW-formatted data files, and an expected output folder with the converted NetCDF UGRID-compliant `.nc` files.
- `TODO.md`: A list of future improvements to the code

# Installation and Usage

The GeoFLOW Data Converter can be run on a Desktop or on NOAA's Hera supercomputer. The latter must be used when converting GeoFLOW datasets with a large memory footprint.

## Option A: Running on a Desktop System

Tested on Ubuntu 18.04.

### Install Libraries

1. Install a C++ compiler, supporting the C++11 standard or newer.

2. Install NetCDF ([NetCDF-C++ API](http://unidata.github.io/netcdf-cxx4/index.html)).
```
sudo apt-get install libnetcdf-c++4-dev-1
```

### Get Code

Download repository:
```
git clone https://github.com/NOAA-GSL/geoflow-data-converter.git
```

### Compile and Run

1. Enter directory:
```
cd geoflow-data-converter
```

2. Build source:
```
make
```

3. Edit your input `.json` file as described in the `README-json.md` file.

4. Run program (replace `JSON_FILENAME` with your input `.json` filename):
```
./bin/main JSON_FILENAME
```

## Option B: Running on a NOAA RDHPCS (Research & Development HPC System) System
Tested on NOAA's Hera supercomputer. For instructions on logging onto Hera and other Hera commands, see the [RDHPCS docs](https://rdhpcs-common-docs.rdhpcs.noaa.gov/wiki/index.php/Start).

### Install Libraries

1. Once logged into your user account on Hera, set up a path to the custom module files used by the GeoFLOW project. There are two different ways to do this:

- **Option A**: From a terminal, each time you log in and get a new shell:
```
module use /scratch2/BMC/gsd-hpcs/modman/modulefiles/base
```
- **Option B**: Place the module path inside your `~/.bashrc` file so the path setup only needs to be done once:
```
export HERA_MODULEPATH_ROOT="/scratch2/BMC/gsd-hpcs/modman/modulefiles/base"
module use ${HERA_MODULEPATH_ROOT}
```

2. Load the modules manually. This step can be skipped as it is integrated into the job script described below. However, if you are running into any issues with compilation, this step is a good way to test if the required modules are installed.

After running the above command that sets up the path pointing to custom modules, run the following command to list the available modules:
```
module avail
```
This should output similar to the following which confirms the custom module path has been loaded:
```
---------------- /scratch2/BMC/gsd-hpcs/modman/modulefiles/base ----------------
   cmake/3.20.2 (D)    gcc/11.1.0    llvm/12.0.0    nvhpc/21.5    pgi/21.5
```

Load the `gcc/11.1.0` module and list the available modules again:
```
module load gcc/11.1.0
module avail
```

This should output similar to:
```
-------------------------------------------------- /scratch2/BMC/gsd-hpcs/modman/modulefiles/compiler/gcc/11.1.0 --------------------------------------------------
   blis/0.8.1    boost/1.76.0    gptl/8.0.3    hdf5/1.12.1    hwloc/2.4.1    libevent/2.1.12    netcdf/4.8.1    openblas/0.3.15    openmpi/4.1.1    ucx/1.10.1

--------------------------------------------------------- /scratch2/BMC/gsd-hpcs/modman/modulefiles/base ----------------------------------------------------------
   cmake/3.20.2 (D)    gcc/11.1.0 (L)    llvm/12.0.0
```

### Get Code

Download the repository:
```
git clone https://github.com/NOAA-GSL/geoflow-data-converter.git
```

### Compile and Run

1. Enter directory:
```
cd geoflow-data-converter
```

2. Edit your input `.json` file as described in the `README-json.md` file.

3. Edit the batch job script `gf-data-converter.sh` to point to your input `.json` file by replacing the filename `ugrid-3D.json` in the following line:
```
./bin/main ugrid-3D.json
```

4. Submit a batch job to Hera by running the job script. The script 
- specifies required sbatch arguments
- purges all modules
- loads modules needed for compilation
- compiles the application
- runs the application
```
sbatch gf-data-converter-job.sh
```

# Running Test Data

A set of simple test datasets are available in the `test-data` folder. In each case, the output `.nc` files will be placed in a folder called `output`. The expected output for each test are located in separate folders in the `test-data` folder (each folder starts with the name `expected-output-data-`).

- To test the 2D spherical dataset on a Desktop system, run:
```
./bin/main test-data/ugrid-2D.json
```

- To test the 3D spherical dataset on a Desktop system, run:
```
./bin/main test-data/ugrid-3D.json
```

- To test the box dataset on a Desktop system, run:
```
./bin/main test-data/ugrid-box.json
```

# Appendix A: GeoFLOW Dataset Assumptions
The following assumptions must hold true for the input GeoFLOW files read in by the data converter.
- There are a total of 3 separate grid variable files - one each for x,y,z coordinate variable.
- Each field variable lives in a separate file.
- If a field variable has multiple timesteps, each timestep worth of data lives in a separate file.
- Each GeoFLOW file starts with a header that contains the information described in the `GHeaderInfo` struct (not including the derived auxiliary data), followed by the data values for that variable. The location of each data value in a field variable file corresponds to the x,y,z data values at the same location in the grid files.
- Each field variable file is of the format `name.xxxxxx.out` where `name` is the name of the variable and `xxxxxx` is a number identifying the timestep of the variable. The timesteps start with `000000` and increase in increments of 1. The following shows an example list of GeoFLOW dataset files. This dataset has 3 grid variables and 2 field variables (with 2 timesteps each).
```
xgrid.000000.out
ygrid.000000.out
zgrid.000000.out
dtotal.000000.out
dtotal.000001.out
T.000000.out
T.000001.out
```

# Appendix B: View ASCII Version of NetCDF File
To view a human-readable version of a NetCDF file, run the following command (replace `NETCDF_FILENAME` with your `.nc` filename):
```
ncdump NETCDF_FILENAME
```

# Appendix C: Useful Commands on Hera

- Submitting a job (your `~/.bashrc` environment will automatically be used):
```
sbatch NAME_OF_JOB_SCRIPT

Example: sbatch gf-data-converter.sh
```

- Showing jobs and their job IDs associated with your user name that are waiting or running:
```
squeue --users=YOUR_USER_NAME

Example: squeue --users=Shilpi.Gupta
```

- Canceling a job:
```
scancel JOB_ID
```

- Getting current memory usage of a job:
```
report-mem -j JOB_ID
```

- Showing all jobs associated with your user name since a certain date:
```
sacct -S YYYY-MM-DD -u USER_NAME

Example: sacct -S 2020-01-01 -u Shilpi.Gupta
```
