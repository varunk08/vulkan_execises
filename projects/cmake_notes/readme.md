### CMAKE NOTES:
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
cmake doesn't use and existing language like python because the creators didn't want it to require a tool to run

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
 
#### Editing CMakeLists file:
within visual studio the CMakeLists files are listed as part of the project and you can edit them simply by double clicking on them.
 
#### Key concepts:
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


#### Build configuration:
cmake supports by default Debug, Release, MinSizeRel and RelWithDebInfo
The CMAKE_CONFIGURATION_TYPES variable is used to tell cmake which configurations are put in the workspace (visual studio)
CMAKE_BUILD_TYPE is used to specify only one configuration while cmake is run (make files)


#### Writing cmake files:
anything within double quotes is treated as a single argument
backslash escapes characters

Basic Commands:
`project` defines name and language of project
default is c and cpp
subdirectories are added to the project using the `add_subdirectory` command.

for visual studio header files won't show up in the IDE unless it is added as a source file

Flow control:
conditional: `if`
loops: `foreach` and `while`
procedures: `macro` and `function`

the conditional of the `if` is repeated in the `else`, `elseif` and `endif` clauses
if supports some custom cmake operations and only a limited set of logical combinations
expressions within `if` are evaluated in the order they appear. For example:
```
NOT 0 AND 0
= 1 AND 0
= 0
```
Page 38 lists the order of precedence of operators and commands.
The following are considered true expressions: `ON, 1, YES, TRUE, Y` and their opposites false
additionally `NOTFOUND, *-NOTFOUND, IGNORE` are also false expressions

The `macro` and `function` that is defined can be used by any cmakelists files processed after its definition
standard arguments like `ARGC, ARGV, ARGN, ARGV0 and ARGV1` etc are defined
arguments become variables within functions

`macro`s don't push and pop scope. the arguments are string replaced prior to execution
`break` and `return` can be used to exit macros, loops and functions.

regular expressions:
`^` matches at beginning of line or string
`$` matches end of a line or string 
`.` matches single character
`[]` matches any characters inside them
`[^]` matches any characters not inside the brackets
`[-]` matches any character in range on either side of the dash
`*` matches preceding pattern zero or more times
`+` matches preceding pattern one or more times
`?` matches preceding pattern zero or once only
`()` saves a matched expression and uses it in later replacement
`(|)` matches either the left or right side of the bar

checking versions of cmake:
`CMAKE_VERSION` variable is pre-defined and can be used for testing versions
`cmake_minimu_required (VERSION x.x)` can also be used to indicate support

cmake modules:
modules are re-useable code put into cmake files that can be used in our own cmakelists files
they are found in the modules subdirectory of the cmake installation
modules can be included using `include`
main categories of modules: find modules, system introspection modules, utility modules

document at the top of the module about what the module defines
many system introspection modules start with `test` or `check`.
system introspection modules may try to compile code and check the results

SWIG: Simplified wrapper and interface generator
swig can wrap c/cpp code by reading the headers into interface usable with other languages like python and java.

Other modules mentioned in the book: qt, fltk, png, zlib

Policies:
a new feature or change is made to cmake that is not compatible with the older versions of cmake.
 
this can create problems when using older CMakeLists file. 

policies are a mechanism in cmake to help improve backward compatibility and to track compatibility issues between different versions of 
cmake

`cmake_policy(VERSION x.x)` is used to set the policy version

this requests the new behavior for all policies introduced in cmake version 2.6 or earlier

`cmake_minimum_required` command also sets the policy

`cmake_policy (SET CMP0002 OLD/NEW)` can request cmake to use the old/new policy of a particular change

policy stack:
policy settings are scoped using a stack.

a new level of the stack is pushed when entering a new sub directory and popped when leaving it.

this is useful when some projects are built with old policies and are maintained separately.


#### Linking Libraries

Linker flags: `-l` for library, `-Bstatic or -Bshared` for linkage type.  
`target_link_libraries (foo debug libdebug optimized libopt)` chooses the debug or optimized version depending on if the debug build was selected.  

static vs shared:  
linkers can choose what objects to link with the executable in static libraries. this makes their sizes small  
for shared libraries, the atomic unit is the entire library. all objects are present in the shared library whether used by the executable or not.  

which symbols are exported by the shared library (unix: all symbols exported, windows: developer must explicitly tell the linker)
microsoft uses `__declspec (dllexport)` directive. 

two approaches  for building dlls in windows:
1. Cmake automatically defines `${LIB_NAME}_EXPORTS` when building a dll on windows.  
2. Using a .def file. Custom cmake commands run a pre-link program to create this .def file from the compiled object files. For example, use a perl script to run the `dumpbin` program on the obj files and extract all the exportable symbols and write the .def file.  

windows vs unix: dlls on windows are required to be fully resolved, this means they must link every symbol at creation. unix systems allow shared libraries to get symbols from the executable or other shared libraries at run time. this increases load time in unix.  

finding which libraries are used by executable: `ldd` on linux, `depends` on windows. cmake will add run time library path info into the linked executable. this feature can be turned off by settinsg the cache entry `CMAKE_SKIP_RPATH` to false.  


####Shared library versioning:  
If the API of a shared library doesn't change but the implementation changes, old executables linked against the old API will run fine. But if the API changes, then versioning is needed.
Executable has to be updated to use the new symbolic link. `libfoo.so` being the old version and the new versions being `libfoo.so.1` and `libfoo.so.2` and so on.  
Platforms that support `soname` natively use this scheme where the `symbolic link` has to be updated to use the new shared library.  

####Installing Files  
For makefile generators (unix, nmake, borland, mingw) the user simply runs `make install` and the make tool will invoke cmake's installation module. With Gui based systems, the user simply builds the target called `INSTALL`  
Install signatures: `TARGETS, FILES, PROGRAMS, DIRECTORY, SCRIPT and CODE`.  
Target files are categorized as  
1. executables - `RUNTIME`
2. loadable modules - `LIBRARY`
3. shared libraries - `LIBRARY`
4. dynamic-link libraries - `RUNTIME`
5. import libraries - `ARCHIVE`
6. static libraries - `ARCHIVE`  
Example  
````
install (TARGETS myExecutable DESTINATION bin)
install (TARGETS myStaticLib DESTINATION lib/myproject)
install (TARGETS myPlugin DESTINATION lib)
````
An easier way to install a shared lib using keyword arguments:  
````
isntall (TARGETS mySharedLib
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/myproject)
````
Here in windows the dll is installed to bin and the .lib of the same library is installed in lib/myproject  
