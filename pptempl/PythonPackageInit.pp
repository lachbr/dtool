//
//
// PythonPackageInit.pp
//
// This file is imported by Template.*.pp for a toplevel directory
// that defines PYTHON_PACKAGE, to create the appropriate __init__.py
// file for the toplevel of the package.
//

// Make sure that every subdirectory of src that contains an
// __init__.py file also contains a Sources.pp file.
#define initpys $[wildcard $[TOPDIR]/src/*/__init__.py]
#foreach file $[initpys]
  #define dirname $[dir $[file]]
  #if $[not $[wildcard $[dirname]/Sources.pp]]
    #print Directory $[notdir $[standardize $[dirname]]] doesn't have a Sources.pp file!
  #endif
#end file


#output __init__.py

# This file is generated by ppremake.  Do not edit it directly; edit
# dtool/pptempl/PackageInit.pp instead if you need to make changes.

"""This file defines the path to the Python files within this package.
There are two cases:

(1) This is a source tree being run interactively by a developer, in
    which case the Python files are found in package/src/*/*.py.  This
    case also breaks down into two sub-cases: (1a) we are using the
    ctattach tools, in which case we should look for the files in the
    actual source directory according to the ctattach variables, or
    (1b) we are not using the ctattach tools, in which case the files
    are right where we expect them to be.

(2) This is an installed tree being run by an end-user, in which case
    the Python files are found in package/*/*.py.  In this case, this
    file doesn't really need to be installed; an empty __init__.py
    file to define the package would serve just as well.  But the file
    is crafted so that it will do no harm if it is installed.
"""

package = '$[upcase $[PACKAGE]]'

import os
import sys

def deCygwinify(path):
    if os.name in ['nt'] and path[0] == '/':
        # On Windows, we may need to convert from a Cygwin-style path
        # to a native Windows path.

        # Check for a case like /i/ or /p/: this converts
        # to i:/ or p:/.

        dirs = path.split('/')
        if len(dirs) > 2 and len(dirs[1]) == 1:
            path = '%s:\%s' % (dirs[1], '\\'.join(dirs[2:]))

        else:
            # Otherwise, prepend $PANDA_ROOT and flip the slashes.
            pandaRoot = os.getenv('PANDA_ROOT')
            if pandaRoot:
                path = os.path.normpath(pandaRoot + path)

    return path

if os.getenv('CTPROJS'):
    # Ok, this is case (1a): we are using the ctattach tools, and
    # therefore will expect to find the source files in
    # $(package)/src/*/*.py.  Completely replace the search path with
    # this path.
    tree = os.getenv(package)

    if not tree:
        raise StandardError, 'CTPROJS is defined, but you are not attached to %s!' % (package)
    tree = deCygwinify(tree)
    __path__[0] = os.path.join(tree, 'src')

    # Also make sure we import direct.showbase.FindCtaPaths, so we'll
    # be able to find all of the other attached projects.
    if package != 'DIRECT':
        tree = os.getenv('DIRECT')
        if not tree:
            raise StandardError,'CTPROJS is defined, but you are not attached to DIRECT!'
        tree = deCygwinify(tree)
        parent, base = os.path.split(tree)
        if parent not in sys.path:
            sys.path.append(parent)
    
    import direct.showbase.FindCtaPaths

else:
    # We are not using the ctattach tools.
    srcDir = os.path.join(__path__[0], 'src')

    if os.path.isdir(srcDir):
        # The source directory exists; therefore, we are in case (1b).
        __path__[0] = srcDir

    else:
        # The source directory does not exist, so we must be in case
        # (2).  Leave well enough alone.
        pass
    
#end __init__.py