DistrRTgen 3.0 BETA

Very very short readme lines :

Added in $HOME/.distrrtgen/distrrtgen.conf a HostName param.
You will need libssl-dev, libcurl-dev in order to compile the binary

# make configure
# make

And complete the params of distrrtgen.conf.

====================================================


DistrRTgen 1.0 readme

DistrRTgen is a Distributed Rainbow Table Generation client. Our goal was to enable our community to contribute to the generation of 
Rainbow Tables as much or as little as suited them, and without having to create whole particular tables.
This small application makes your contribution simple, and lets the server do all the dirty work, automatically putting your CPU(s) 
to good work when idle.


How it works:

DistrRTgen is based on Zhu Shuanglei's Project Rainbow Crack. Your client connect to a server, which delegates work between all 
clients and designates you part of a table to generate. Your computer generates it's designated 'part', and uploads it to the 
server, upon which you are designated the next part.
What then?
All parts are compiled into complete tables. We will upload the tables to FreeRainbowTables.com, where they will be available for 
you, and everybody else to download in whole; for free. You are able to view the progress of all tables in the table-set at any point within the application.



INSTALLATION

To compile, you need the openssl headers installed and g++.
If everything needed is installed, you simply type "make" to compile it.
"make configure" will create a default configuration file in ~/.distrrtgen/distrrtgen.conf
You need to edit this file and put in your username and password for you distrrtgen account. You also have the option of correcting the amount of processors detected on the system


ARGUMENTS
To run the client, type "./distrrtgen".
-q will run the client in quiet mode, only showing warnings and errors.
-Q will run the client in extra quiet mode, only showing errors
To create a freerainbowtables account, download the Windows GUI version and use it to register a new account. 
Then use the same credentials for this linux client.


ANTICIPATED QUESTIONS:

Why do I need to register?
Simply to help us keep track of what is generated by whom. You do not have to enter and email address or any other personal 

information - simply select a username and password. Also, by keeping track of you contribution, we can credit you for your work.

I have more than one computer on which I wish to generate tables. Can I run more than one instance of DistrRTgen under one User 

Account?
Yes. You need only register ONE ACCOUNT for as many computers as you wish to run the application on.

What about Dual-Core computers?
DistrRTgen will function on Dual-Core computers, and take advantage of both processors.

What happens if I stop generating?
Should you fail to complete a part within 48 hours, it is forfeited and redesignated. However, as parts are fairly small (usually 

less than 2 hours work), waste is minimal. You can continue to generate at any time.

Is there a Linux client available?
Yes! This is it!

Are there any prerequisites?
See under "INSTALLATION"

CREDITS
Thanks alot to Jonatan B for helping me with the Linux Makefile. He did a great job!
