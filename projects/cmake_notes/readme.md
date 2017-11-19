CMAKE NOTES:
(Notes from the Mastering Cmake book).

Generate project files for other IDEs
Handles cross platform building, system introspection

Many benefits for Single platform, multi-machine development
Can search for programs, libraries, header files, environment variables and registry settings
Can build in directory other than src directory
Has commands to generate new source files from famous auto gen library headers
Can select optional components at config time
Can create many projects from a simple text files
Can easily switch between shared and static libraries, knows all linker flags
File dependencies and parallel builds are handled.

Benefits for cross-platform software dev:
can test machine byte order
can configure files with system dependent information

Cmake can be installed from binaries or built from src

Commands are of the form command(args)
command names are case in-sensitive
variables are referenced using a ${VAR} syntax
set (foo a b c) will set foo to a b c
can be passed in to a command: command (${foo}), is equivalent to command (a b c)
system environment variables can be accessed as $ENV{VAR}
[HKEY_CURRENT_USER\\Software\\path1\\path2;key] syntax for referencing registry keys can
also be used

two main directories: src and bin. src has the src files along with the CMakeLists files

Running from command line:
from a directory where the build files will be written, type
cmake -G "Visual Studio 14 2015 Win64" {relative path to src}

Specifying the compiler:
The generator can specify the compiler, an environment variable, cache entry

Dependency analysis:
depend.make stores depend info for all object files in the directory
flags.make compile flags used for src files
dependInfo.make used to keep dependency info up to date
build.make rules for building the dependencies
 
 Editing CMakeLists file:
 within visual studio the CMakeLists files are listed as part of the project and you can edit them simply by double clicking on them.
 