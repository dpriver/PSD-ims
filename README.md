# PSD-IMS


### About
This project was conceived as an assignment for the subject "Distributed Systems 
programming" from the "Computer Engineering program" of the Complutense 
University of Madrid.

The objective is to design and carry out an Instant Message Service based on 
[gsoap](http://www.cs.fsu.edu/~engelen/soap.html "gsoap") framework.


### Dependencies
There are some dependencies to have in mind.
I can remember gsoap and mysql... if that is not enought to compile and execute
PSD-ims, please open an issue so I can recheck them.


### Compile
A set of handmade makefiles are used to compile this project. Please, be patient 
if the proyect does not compile for you, and report here the errors you get.

The following bash commands are normally used.

```bash
cd PSD-ims
make all
```


### Use
Once the proyect has been sucesfully compiled, two new directories should have 
appeared, build/ and bin/.
build/ contains the object (.o) files, and bin/ the executables. This executables 
are "client" and "server".

The first thing to do, is to create the database. PSD-ims uses mysql for that.
If you have it already installed, execute the bash commands
```bash
scripts/sql/db-psd.sql
```
Now that the database is created, we can launch the server
```bash
cd bin
./server <port> <db_user> <db_pass>
```
and the client
```bash
./client <url>:<port>
```

If you are working locally, "localhost" can be used as url
