Test case for showing PDF Paths being broken via Color Conversion function.

## Prerequisites

1. Docker installed and working
2. Adobe PDF Library for linux x86_64, version 18.0.4PlusP2w

This requires the reader to supply their own copy of the Adobe PDF Library for Linux (x86_64), version 18.0.4PlusP2w. This library should be extracted into the apdfl directory:

```shell
$ chmod +x /path/to/APDFL18.0.4PlusP2w_x86_64-linux_gcc48.bsx
$ /path/to/APDFL18.0.4PlusP2w_x86_64-linux_gcc48.bsx

APDFL Self Extracting Installer
...

End User License Agreement
....EULA goes here...

Do you accept the terms and continue installation? [y,n]
> y
Enter location you would like to install APDFL
(Press ENTER to install in current directory)
> apdfl
Installing APDFL in /path/to/color-converter/apdfl
.
Installation of files complete!

Getting Started
	C++
		Documentation: https://docs.datalogics.com/apdfl18/CPlusPlus/index.html
		Samples: https://github.com/datalogics/apdfl-cplusplus-samples
Need Assistance?
	Please contact evalsupport@datalogics.com for help getting started.
$
```

## Building & Running
Once these pre-requisites have been met, you should be able to build and run the test with the following:

```shell
$ cd /path/to/color-converter
$ docker buildx build --platform=linux/amd64 -t color-converter .
```

To run the program in the container and get the output file out, run the following:

```shell
$ docker run -v $PWD/fixtures:/color-converter/fixtures -v $PWD/output:/color-converter/output color-converter fixtures/SVG_Test_input.pdf output/SVG_Test_output.pdf
```

The output will then be at the location `output/SVG_Test_output.pdf`.

### How does this image/container work?

The input file is being mounted into docker using a volume: `-v $PWD/fixtures:/color-converter/fixtures`, which makes it available in the working directory of the container. 
Subsequently, the output directory is also mounted as a volume: `-v $PWD/output:/color-converter/output`, allowing us to access the output PDF once the conversion process is done.

The container image is setup with the entrypoint set to run the ColorConverter program, so we only need to provide our input filepath and output filepath to the container - these are
the last two arguments to the `docker run` command, `fixtures/SVG_Test_input.pdf` and `output/SVG_Test_output.pdf`. You can change the input and output files as desired, as long as they
are correctly made available to the docker container at runtime, as described in the previous paragraph.

