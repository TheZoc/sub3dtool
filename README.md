# Sub3dtool
Convert subtitle to 3D (ASS Formar) to use with VLC or MPlayer

This is a fork of the [original project](https://code.google.com/p/sub3dtool/), with fixes so it runs on Windows.

This fork uses [this getopt port](http://www.codeproject.com/Articles/157001/Full-getopt-Port-for-Unicode-and-Multibyte-Microso) with few minor modifications to be used in this project.

## Compiling

To compile from source code, clone this repository and compile it.

### Linux and OS X

```
mkdir sub3dtool
cd sub3dtool
git clone https://github.com/TheZoc/sub3dtool.git .
make
./sub3dtool
```

### Windows
You will need [Visual Studio 2013](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx) to use the supplied solution file.
Just download and compile using it, hassle free.

## Basic Usage
To convert a subtitle to Side-by-Side, just run:
```
./sub3dtool input.srt --3dsbs -o output.ass
```

Note: on Windows, you will need to omit the ```./``` at the start of the command line.

*input.srt* is the path to the file that needs to be converted
*output.ass* is the name of the output file you want as result
*--3dsbs* will convert the subtitle to 3DSBS Side-by-Side format.

