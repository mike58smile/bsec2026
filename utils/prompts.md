# Serial plotter
This program plots up to three axes of integer data collected from serial port. The data sent from the remote device needs to be in a certain format: every line must begin with '$' and end with ';'. The values must be space separated.
Example of sending data from mbed to this application:
pc.printf("$%d %d;", rawData, filteredData);
        wait_ms(10);

