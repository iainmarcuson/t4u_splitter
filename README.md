t4u_splitter
============

This demultiplexes multiple T4U data streams sent to one host.

Introduction
------------

The T4U electrometer sends all data to its destination host to a single 
UDP port programmed at the factory.  t4u_splitter demultiplexes the data 
stream based on the multiple T4U source IP addresses.  The control 
channels are outgoing TCP connections from the host to the T4U, so those 
connections do not need demultiplexing.

Config File
-----------

The demultiplexing is controlled by a configuration file.  The file 
format is lines of space-separated T4U IP Address/Port pairs.  The port 
is the post on localhost where T4U-Viewer will listen to the data 
stream.

t4u_splitter Usage
------------------

t4u_splitter is invoked as

    ./t4u_splitter <config file>

T4U-Viewer Usage
----------------

The viewer must be invoked on the same computer as the splitter, since 
the splitter outputs the data stream to localhost.

    ./T4U-Viewer --platform xcb -f <T4U config file> -l <splitter output 
    port> --addr <T4U address> -p <EPICS base port>

* `--platform xcb` 
   * The platform argument is mandatory and must be the 
   first argument in the list.
* `-f <T4U config file>`
   * Different T4Us use different calibrations, so this option allows 
   the correct file to be set for the T4U to connect to
* `-l <splitter out port>` 
  * This is the output port specified in the 
  splitter config file for the T4U to connect to
* `--addr <T4U address>` 
  * This is the IP address.  This parameter is 
  needed to connect the command socket.  Additionally, the connection of 
  the command socket starts the transmission of the data stream.
* `-p <EPICS base port>` 
  * This is the first of the two ports that T4U 
  Viewer will listen to for the EPICS connection.  This option needs to 
  be specified so different viewers do not try to bind to the same 
  ports.
