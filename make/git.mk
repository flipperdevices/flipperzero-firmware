GIT_COMMIT		= $(shell git describe --always  --exclude '*' || echo 'unknown')
GIT_BRANCH		= $(shell git branch --show-current || echo 'unknown')
GIT_BRANCH_NUM	= $(shell git rev-list --count $(GIT_BRANCH) || echo 'nan')

CFLAGS			+= -DGIT_COMMIT="\"$(GIT_COMMIT)\"" -DGIT_BRANCH="\"$(GIT_BRANCH)\"" -DGIT_BRANCH_NUM="\"$(GIT_BRANCH_NUM)\"" 