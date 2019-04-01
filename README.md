# Refrigerator-Monitoring-System - A multi threaded application on Beagle Bone 

## Temperature Task - TMP102 sensor
This is implemented as a thread which measures the temperature at regular intervals  
	-**Temperature Alert system**  
		- This turns on an LED2 when the fridge temperature reaches an higher threshold value of 29C  
		  and turns off when the lower threshold of 27C is reached  
		- This is implemented with the use of interupts on BBG and temperature sensor  

## Light Task - APDS-9301
This is implemented as a thread which measures the brightness at regular intervals  
	- **Fridge Door Open Alert System**  
		- This turns on an LED3 when the brightness inside the fridge is higher than 70 lux  
		- Also indicates when there is a change of state either from Dark to Low or Low to Dark  

## Synchronized Logger Thread
This is implemented as a thread which helps to log message to a file  
	- Message Queue IPC is used along with locks  
	- The looger task supports four log levels  
		- DEBUG, INFO, WARN, ERROR (increasing order of severity)  
	- Further, it **enables the logger level above which the log messages should be logged**  
		eg: if the logger level is choosen as WARN, all log messages above or equal to WARN level will be logged   
	
## Remote Request Thread
This is implemented as a thread which reponds to requests from outside and is implemented using TCP sockets  
	- Remote requests supported  
		- Temperature in Celcius, Kelvin, Fahrenheit  
		- Brightness inside the fridge  
		- The state of the fridge door (opened or closed)  
	- Helps to **support multiple remote clients simultaneously**  
	

## Heart beat (main thread)
This is the main task which helps to check the heartbeat of the other threads spawned  
	- The threads send heartbeat to this task at regular intervals saying that they are alive  
	- This helps to indicated if any thread is dead  
	- Also this supports signals to kill any thread explicity  
		- sending SIGUSR1 from main thread kills Temperature thread  
		- sending SIGUSR2 from main thread kills Light thread  
		- sending SIGALRM from main thread kills Logger thread  
		- sending SIGTERM is for killing the main thread  
		- killing tasks through these signals, exits the threads gracefully by doing cleanup job  

## Power On Self Test (POST)
This is performed at the time of booting to ensure all the connected hardwares are working properly  
If any sensor fails on POST,that particular thread is killed and the remaining application works as expected  
Further, an LED glows to indicate that particular sensor is non operational  
	- LED 0 - Light sensor  
	- LED 1 - Temperature sensor  

## Unit Testing on BB  
Unit testing has been performed for Lux sensor module, Temperature sensor module and Logger module    


# Folder Structure
**SRC**  
	- contains all the source files  
**INC**  
	- contains all the include files  
**unit**   
	- contains the unit test file

# How to execute the application?  
**To make the executable for application on BB** 
	-- enable the macro NO_UNIT_TEST in common.h   
	-- make bbg  
**To make the executable for unit testing on BB**  
	-- disable the macro NO_UNIT_TEST in common.h
	-- make test_bbg  
**To clean up the executables**  
	-- make clean  
Note:  
	To view debug messages on terminal, enable the macro DEBUG in common.h