#!/usr/bin/env python3

import os
import re
import sys
from slack_sdk import WebClient
from slack_sdk.errors import SlackApiError


def checkCommitMessage(msg):
    regex = re.compile(r"^'?\[FL-\d+\]")
    if regex.match(msg):
        return True
    return False


def reportSlack(commit_hash, slack_token, slack_channel, message):
    client = WebClient(token=slack_token)
    try:
        client.chat_postMessage(channel="#" + slack_channel, text=message)
    except SlackApiError as e:
        print(e)
        sys.exit(1)


def main():
    commit_msg = os.getenv("COMMIT_MSG")
    commit_hash = os.getenv("COMMIT_HASH")
    commit_sha = os.getenv("COMMIT_SHA")
    slack_token = os.getenv("QA_REPORT_SLACK_TOKEN")
    slack_channel = os.getenv("QA_REPORT_SLACK_CHANNEL")
    commit_link = (
        "<https://github.com/flipperdevices/flipperzero-firmware/commit/"
        + commit_hash
        + "|"
        + commit_sha
        + ">"
    )
    message = "Commit " + commit_link + " merged to dev without 'FL' ticket!"
    if not checkCommitMessage(commit_msg):
        reportSlack(commit_hash, slack_token, slack_channel, message)


if __name__ == "__main__":
    main()
