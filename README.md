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


##Use and Limitations:
* Enter commands
* Multiple commands can be entered on a single line separated by a ';'
* Rshell also supports the "&&" and "||" connectors:<br />
`echo first || echo second && echo third`
* Comments can be added to a line using the '#' character
* To exit, type the command `exit`
* Any of the following characters entered before commands will be ignored:<br />
space, tab, ';' semi-colon, '&' ampersand, '|' vertical bar
* The above are also handled if appended to the end of your command

## Known Bugs:
* There are some memory leaks that have yet to be resolved 

#Ls:

##Running:
1) ``git clone https://github.com/tristanzickovich/rshell.git``

2) ``cd rshell``

3) ``make``

4) ``bin/ls``


##Use and Limitations:
* Run executable with flags: `-a`, `-l`, `-R`, or any combo
* Combos of flags can be listed as a single flag. Ex. `-al`, `-Rla`, ... etc.
* Unknown flags and duplicate flags will be ignored
* Bin/ls can be called on specific files, if you wish to use flags as well, 
	the flags must be called first. Ex. `bin/ls -a filename`
* Multiple files can be specified, each will be executed in sequence of which was typed first.
	They will be output one after another. 
* Bad file names will cause the program to exit (therefore files listed after bad file will not be attended to). 
* Files specified that are not directories will prompt that it is not a directory and will exit the program.
* Flags with multiple files will be applied to all files specified
* Directories will be printed in blue, executables in green,
	and hidden files with a grey background (colored text and background
	will be combined where applicable).

## Known Bugs
* No known memory leaks or bugs
