#include "version.h"
#include <api-hal-boot.h>

struct Version_ {
    const char  *git_hash;
    const char  *git_branch;
    const char  *git_branch_num;
    const char  *build_date;
};

/* version of current running firmware (bootloader/flipper) */
static const struct Version version __attribute__((used)) = {
    .git_hash       = GIT_COMMIT,
    .git_branch     = GIT_BRANCH,
    .git_branch_num = GIT_BRANCH_NUM,
    .build_date     = BUILD_DATE,
};


const struct Version *version_get(void) {
    return &version;
}

const char *version_get_githash(const struct Version *v) {
    return v ? v->git_hash : version.git_hash;
}

const char *version_get_gitbranch(const struct Version *v) {
    return v ? v->git_branch : version.git_branch;
}

const char *version_get_gitbranchnum(const struct Version *v) {
    return v ? v->git_branch_num : version.git_branch_num;
}

const char *version_get_builddate(const struct Version *v) {
    return v ? v->build_date : version.build_date;
}

const char *version_get_version(const struct Version *v) {
    return v ? v->version : version.version;
}

