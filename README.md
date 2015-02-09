##Overview:
This program is a simplified version of a terminal (bash for instance).
It takes in and executes commands as a terminal would.

ex)

``$ pwd``
``home/curuser/Documents/rshell``
#Rshell

##Running:
1) ``git clone https://github.com/tristanzickovich/rshell.git``

2) ``cd rshell``

3) ``make``

4) ``bin/rshell``


##Use:
* Enter commands
* Multiple commands can be entered on a single line separated by a ';'
* Rshell also supports the "&&" and "||" connectors:<br />
`echo first || echo second && echo third`
* Comments can be added to a line using the '#' character
* To exit, type the command `exit`
* Any of the following characters entered before commands will be ignored:<br />
space, tab, ';' semi-colon, '&' ampersand, '|' vertical bar
* The above are also handled if appended to the end of your command

## Known Bugs and Limitations:
* There are some memory leaks that have yet to be resolved 

#Ls:

##Running:
1) ``git clone https://github.com/tristanzickovich/rshell.git``

2) ``cd rshell``

3) ``make``

4) ``bin/ls``


##Use:
* Run executable with flags: `-a`, `-l`, `-R`, or any combo
* Unknown flags and duplicate flags will be ignored
* Directories will be printed in blue, executables in green,
	and hidden files with a grey background (colored text and background
	will be combined where applicable).

## Known Bugs and Limitations
* No known memory leaks or bugs
