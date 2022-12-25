# Flipp Pomodoro

Your productivity best friend.

## Development

Run application build in docker environment
```shell
# This will leave container runnning with a tag flipp-pomodoro-flipper-development-1
docker compose -f "docker-compose.development.yml" up -d --build

# You can exec bash from inside the contaier https://docs.docker.com/engine/reference/commandline/exec/
# Once you inside the contaier

# One time run:
bash tools/install.sh

# Every build run
bash tools/build.sh
# Build artifacts will be copied to dist directory.
```