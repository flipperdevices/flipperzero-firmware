# Overview
This is a simple Docker environment for compiling and running WolfSSL. Use `run.sh` to build everything (Docker container, WolfSSL, etc.). This script takes in arguments that can be passed to `./configure`. For example: `run.sh --enable-all`

When the compilation and tests succeed, you will be dropped in to a shell environment within the container. This can be useful to build other things within the environment.

# FAQ
## permission denied while trying to connect to the Docker daemon socket at unix:///var/run/docker.sock
You need to be added to the `docker` group to run Docker containers. Run `sudo usermod -aG docker $USER`. You may need to restart the Docker daemon.

## Unable to access symlinked files outside of WolfSSL
The volume mounted in the Docker container needs to have all files that your compilation will need. To solve this, you have a couple options:
1. Change the `WOLFSSL_DIR` variable in the `run.sh` to one higher up (by adding `/..` to the path). Then update the `docker build` to include the correct path to the Dockerfile and the `docker run` argument to the working directory (`-w`) to the WolfSSL source directory
2. Move the external repository to within the WolfSSL directory. For example create an `external` folder which has your files. This route may have complications when stashing Git work.