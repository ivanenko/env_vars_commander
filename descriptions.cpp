/*
Wfx plugin for working with Linux environment variables

Copyright (C) 2019 Ivanenko Danil (ivanenko.danil@gmail.com)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
        Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <cstring>
#include "descriptions.h"

// std::map<int, char> example = { {1,'a'}, {2, 'b'}, {3, 'c'} };

// based on help.ubuntu.com article
pair descr[] = {
        // File-location related variables
        {"PATH", "When you type a command to run, the system looks for it in the \ndirectories specified by PATH in the order specified"},
        {"CDPATH", "The search path for the cd command"},
        {"MANPATH", "List of directories for the system to search manual pages in"},
        {"LD_LIBRARY_PATH", "List of directories where the system searches for runtime libraries \nin addition to those hard-defined by ld and in /etc/ld.so.conf.d/*.conf files"},
        {"TMPDIR", "The directory used for temporary file creation by several programs"},
        {"HISTFILE", "The name of the file in which command history is saved"},
        {"HISTFILESIZE", "The maximum number of lines contained in the history file"},
        {"HISTSIZE", "The number of commands to remember in the command history. \nThe default value is 500"},
        {"HISTSIZE", "The number of commands to remember in the command history. \nThe default value is 500"},
        {"HISTSIZE", "The number of commands to remember in the command history. \nThe default value is 500"},

        // Locale setting variables
        {"LANG", "The basic language setting used by applications on the system, \nunless overridden by one of the other locale environment variables"},
        {"LC_CTYPE", "The character set used to display and input text"},
        {"LC_NUMERIC", "How non-monetary numeric values are formatted on screen"},
        {"LC_TIME", "How date and time values are formatted"},
        {"LC_COLLATE", "How to sort various information items (e.g. defines the order of \nthe alphabet so items can be ordered alphabetically by the sort command)"},
        {"LC_MONETARY", "How monetary numeric values are formatted"},
        {"LC_MESSAGES", "Which language is to display messages to the end user"},
        {"LC_PAPER", "Definitions of paper formats and standards"},
        {"LC_NAME", "How names are formatted"},
        {"LC_ADDRESS", "How to display address information"},
        {"LC_TELEPHONE", "How telephone numbers are structured"},
        {"LC_MEASUREMENT", "What units of measurement are used"},
        {"LC_IDENTIFICATION", "Metadata about the locale information"},
        {"LC_ALL", "Override over all the other locale environment variables"},

        // Preferred application variables
        {"PAGER", "The name of the utility used to display long text by commands such as man"},
        {"EDITOR", "The name of the user's preferred text editor. Used by programs \nsuch as the mutt mail client and sudoedit"},
        {"VISUAL", "Similar to the \"EDITOR\" environment variable, applications typically \ntry the value in this variable first before falling back to \"EDITOR\" if it isn't set"},
        {"BROWSER", "The name of the user's preferred web browser"},

        // Graphical desktop-related variables
        {"DISPLAY", "This variable is used to indicate to graphical applications \nwhere to display the actual graphical user interface"},
        {"XDG_DATA_HOME", "Indicates to applications that conform to the freedesktop.org \nspecifications, where to place the user's private data. This variable is typically unset since a sensible default fall-back value was defined by the specifications"},
        {"XDG_CONFIG_HOME", "Where to place the user's configuration information"},
        {"XDG_DATA_DIRS", "A colon-separated list (similar to \"PATH\") of directories \nwhere data is searched for"},
        {"XDG_CONFIG_DIRS", "A colon-separated list (similar to \"PATH\") of directories \nwhere configuration information is searched for"},
        {"XDG_CACHE_HOME", "A location used by applications that conform to the freedesktop.org \nspecifications to cache temporary data"},

        // Program execution variables
        {"LD_PRELOAD", "This variable can be used to inject a custom dynamic library \ninto an application's memory when it loads"},

        // Compilation and software development related variables
        {"CC", "The name of the C compiler to use"},
        {"CXX", "The name of the C++ compiler to use"},
        {"CFLAGS", "TA list of debugging/optimization flags to pass to the C compiler"},
        {"CXXFLAGS", "A list of debugging/optimization flags to pass to the C++ compiler"},
        {"CXXFLAGS", "A list of debugging/optimization flags to pass to the C++ compiler"},
        {"CPPFLAGS", "A list of flags to pass to the C/C++ pre-processor/compiler"},
        {"LIBRARY_PATH", "A list of directories (separated by colons) in which \nlibrary files should be searched for"},
        {"INCLUDE", "A colon-separated list of directories in which header files \nshould be searched for"},
        {"CPATH", "A colon-separated list of directories in which header files \nshould be searched for"},

        // Other environment variables
        {"USER", "The name of the currently logged-in user. This variable is set by \nthe system. You probably shouldn't change its value manually"},
        {"LOGNAME", "Similar to \"USER\", some programs prefer to read this variable rather than USER"},
        {"HOME", "The location of the currently logged-in user's home directory"},
        {"PWD", "The current working directory of the shell"},
        {"SHELL", "The user's preferred command-line shell as it is set in the /etc/passwd file"},
        {"HOSTALIASES", "The name of a file containing host-name aliases for use with various network programs"},
        {"TZDIR", "The path of the directory containing time-zone information files"},
        {"COLUMNS", "The number of text columns in the terminal window. Try adjusting \nthis variable if lines don't seem to wrap properly in the console"},
        {"LINES", "The number of text lines on the console window. Try adjusting this \nvariable if you're getting strange results when scrolling text"},

        {NULL, NULL}
};

char* find_descr(const char* varName)
{
    pair *p = descr;
    while(p->var){
        if(strcmp(p->var, varName)==0){
            return p->description;
        }
        p++;
    }

    return NULL;
}

