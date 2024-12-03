# CLI
CLI in C!

## Using the Application
1. Using [Cygwin](https://www.cygwin.com/), install `cmake` `make` `gcc-core` `gcc-g++` `libcurl-devel` `libcurl4` `curl`.
2. Set a system environment variable `CYGWIN_HOME` to the Cygwin installation directory.
3. Use the `make` command to build the program. 
4. Execute one of the following commands in the root directory.
    * `cli.exe` [starts the monitor]
    * `cli.exe login/logout`
    * `cli.exe start --machine NAME --file PATH`
    * `cli.exe stop/resume/pause --machine NAME`