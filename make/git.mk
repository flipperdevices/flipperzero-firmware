GIT_COMMIT		= $(shell git describe --always  --exclude '*' || echo 'unknown')
GIT_BRANCH		= $(shell git branch --show-current || echo 'unknown')
CFLAGS			+= -DGIT_COMMIT="\"$(GIT_COMMIT)\"" -DGIT_BRANCH="\"$(GIT_BRANCH)\""