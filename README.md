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

The last instruction of the Dockerfile is to run the color conversion (as a part of the build) on the supplied SVG test fixture, and output it to `/color-converter/SVG_Test_output.pdf`.

## Assessing the output

To retrieve the file for inspection, you can download it using docker cp:

```shell
$ docker cp $(docker create --name cc-container color-converter):/color-converter/SVG_Test_output.pdf . && \
$ docker rm cc-container
```

## Running the program manually

You can run an instance of the container and run bash within it, like so:

```shell
$ docker run -it color-converter /bin/bash
```

Then inside the container, you can run the command on the given input, and an output filename:

```shell
$ ColorConverter/bin/ColorConverter SVG_Test_output.pdf new-output-filename.pdf
```

