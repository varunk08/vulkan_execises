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
 
Key concepts:
cmake is written in c++
each command is a class 
there is a global top level cmake object generated. it has one globalGenerator object.
The global generator object has many local generator objects
each local generator object is for a single directory and represents the config of one CMakeLists file
for example, for visual studio, the global generator generates the .sln file while each local generator generates a .proj

"targets" can be libraries or executables, the result of the build

include(file1. cmake) can be used to include and process the commands in file1.cmake

when a variable is set, it is visible to the current CMakeLists file and to all subdirectories
when a new scope is entered all the existing variable values are copied, a new scope is created
any variables created in the child scope does not affect the parent scope
use PARENT_SCOPE to affect parent scope as well: set(variable 2 PARENT_SCOPE)

variables set from the cmake gui must be used as cache entries
cmake generates a cache file in the directory of the binary that is built
option (VAR_NAME "What do you want this var to be?") sets a variable in the cache
set (VAR_NAME ON CACHE BOOL "set to true?") the set() command can also put variables on the cache
variable types are BOOL, PATH, FILEPATH, STRING
mark_as_advanced command makes a variable advanced. it is hidden from the user in gui
a variable can still be overridden with the set command without using the CACHE option
the cache values are checked only if the current cmMakeFile object doesn't have that variable
the FORCE option can change a cache value from the cmakelists file


Build configuration:
cmake supports by default Debug, Release, MinSizeRel and RelWithDebInfo
The CMAKE_CONFIGURATION_TYPES variable is used to tell cmake which configurations are put in the workspace (visual studio)
CMAKE_BUILD_TYPE is used to specify only one configuration while cmake is run (make files)




 