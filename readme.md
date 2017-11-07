This is a repo which will serve as a testground for vulkan.

Tutorials following:
https://vulkan-tutorial.com/Introduction



CMAKE NOTES:
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

