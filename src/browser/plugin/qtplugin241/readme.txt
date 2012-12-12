Browser Plugin v2.4

The QtBrowserPlugin solution is useful for implementing plugins
for web browser.



Notes:

This module can not function if built as a DLL, it must be
statically linked with the plugin. Hence, DLL building is not
supported, and the -library option to configure will be ignored.
For the same reason, the source code that needs to be statically
linked is licensed under a BSD style license (ref. the comment
headers of the files in the src directory).

Version 2.4 is known to not work, even experimentally, on Mac OS
X.

