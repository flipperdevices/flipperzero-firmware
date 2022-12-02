#!/usr/bin/env python3

import os

def main():
    print(os.getenv("COMMIT_MSG"))

if __name__ == '__main__':
    main()
