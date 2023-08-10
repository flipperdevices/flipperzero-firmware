#!/bin/bash
SRC=vagrant
DST=wolfssl

# synchronize without .git and .gitignore to avoid output files
rsync --exclude .git --exclude-from .gitignore -rvt /$SRC/ ~/$DST/

# synchronize everything else needed.
rsync -rvt /$SRC/.git         ~/$DST/
rsync -rvt /$SRC/IDE          ~/$DST/
rsync -rvt /$SRC/mcapi        ~/$DST/
rsync -rvt /$SRC/mplabx       ~/$DST/
rsync -rvt /$SRC/certs        ~/$DST/
rsync -rvt /$SRC/configure.ac ~/$DST/
