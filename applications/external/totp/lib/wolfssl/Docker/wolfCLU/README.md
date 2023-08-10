This is a small container that has wolfCLU installed for quick access. To build your own run the following:
```
docker build --pull --build-arg DUMMY=$(date +%s) -t wolfclu . 
```

To run the container, you can use:
```
docker run -it --rm -v $(pwd):/ws -w /ws wolfclu version
```
This command will allow you to use the certs/keys in your local directory.
