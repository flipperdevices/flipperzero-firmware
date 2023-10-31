#!/bin/sh
#
# Create configure and makefile stuff...
#

# Git hooks should come before autoreconf.
if test -d .git; then
  if ! test -d .git/hooks; then
    mkdir .git/hooks
  fi
  ln -s -f ../../pre-commit.sh .git/hooks/pre-commit
  ln -s -f ../../pre-push.sh .git/hooks/pre-push
fi

# If this is a source checkout then call autoreconf with error as well
if test -d .git; then
  WARNINGS="all,error"
else
  WARNINGS="all"
fi

autoreconf --install --force --verbose

