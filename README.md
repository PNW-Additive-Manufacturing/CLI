# CLI
Command line interface in C for the PNW 3d printing service!

## Using the Application
1. Using [Cygwin](https://www.cygwin.com/), install `cmake` `make` `gcc-core` `gcc-g++` `libcurl-devel` `libcurl4` `curl`.
2. Set a system environment variable `CYGWIN_HOME` to the Cygwin installation directory.
3. Add `C:\cygwin64\bin`to the `PATH` user environment variable.
4. Use the `make` command to build the program.
5. Execute one of the following commands in the root directory.
    * `cli.exe` [starts the monitor]
    * `cli.exe login/logout`
    * `cli.exe start --machine NAME --file PATH`
    * `cli.exe stop/resume/pause --machine NAME`
6. Optionally, instead of logging in, an environment variable, `PNW3dAccessToken`, can set to the the value of the access token. If it isn't set, the program will use the value of the `.accessToken` file that is created by the login function.