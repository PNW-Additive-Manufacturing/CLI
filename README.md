# PNW3D CLI
Command line interface in C for the PNW3D Printing Service! This application is meant for Maintainers and above to monitor and control machines.

## Building the Application
1. Using [Cygwin](https://www.cygwin.com/), install `cmake` `make` `gcc-core` `gcc-g++` `libcurl-devel` `libcurl4` `curl`.
2. Set a system environment variable `CYGWIN_HOME` to the Cygwin installation directory.
3. Add `C:\cygwin64\bin`to the `PATH` user environment variable.
4. Make sure you are using the Cygwin GCC Compiler! You should see /usr/bin/gcc when running `which gcc` on your terminal.
5. Use the `make` command to build the program.

## Using the PNW3D CLI
1. Execute one of the following commands in the root directory.
    * `cli.exe` [starts the monitor]
    * `cli.exe login/logout`
    * `cli.exe start --machine NAME --file PATH`
    * `cli.exe stop/resume/pause --machine NAME`
2. Optionally, instead of logging in, an environment variable, `PNW3dAccessToken`, can set to the the value of the access token. If it isn't set, the program will use the value of the `.accessToken` file that is created by the login function.