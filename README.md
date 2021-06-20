# Csc360 Coursework A1_Xuanheng He

### Feature a
I output "vsh%" repeatedly in console to ask user for commands. For simple commands, I call funtion named "Output_One_Cmd", which will create a child process for these commands.

### Feature b
I read all of the absolute paths in .vshrc file to an array, then for each command input by user, I search through the array and use "access" method to see if there exist a correct path for the command. All the options will be stored in the "ref" array, then copy to "args" array then use "execve" method to excute the command. If no correct path found, an error message will be printed. When "exit" is entered, vsh will terminate.

### Feature c
I extract filename after "::"  from user's input, then I call the function named "Output_in_File", which will create a child process to execute the command, and use "dup2" to store the output in the file. A suitable error message will be printed if missing filename after "::" or no command provided.

### Feature d
I extract filename befor "::"  from user's input, then I call a function named "input_in_File", which will create a child process to execute the command, and use "dup2" method to send the file as the command input. A suitable error message will be printed if missing filename before "::" or file corresponding to the filename does not exist.

### Feature e
I calle "gettimeofday" function before execute the command and after execute the command. Then I check if the command last argument is "##", if it is true, output the "wallclock" time.

### Feature f
I first check if the command has both a input file(before "::") and a output file(after "::"), if it is true, I called funtion "combined_commands", which will create a child process to excute the command, and use "dup2" method to send the input file as the command input and store the output in the output file. If the command last argument is "##", output the "wallclock" time as well.
