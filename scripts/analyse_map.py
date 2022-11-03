#!/usr/bin/env python3

from datetime import datetime
from mapser import MapFileAnalyzer
import argparse
import mariadb
import sys
import os


def parseArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("db_user", help="MariaDB user")
    parser.add_argument("db_pass", help="MariaDB password")
    parser.add_argument("db_host", help="MariaDB hostname")
    parser.add_argument("db_port", type=int, help="MariaDB port")
    parser.add_argument("db_name", help="MariaDB database")
    parser.add_argument("map_file", help="Map file(.map)")
    args = parser.parse_args()
    return args


def mariadbConnect(args):
    try:
        conn = mariadb.connect(
            user=args.db_user,
            password=args.db_pass,
            host=args.db_host,
            port=args.db_port,
            database=args.db_name,
        )
    except mariadb.Error as e:
        print(f"Error connecting to MariaDB: {e}")
        sys.exit(1)
    return conn


def getSectionsSize(mapser):
    arr = {}
    for segment in mapser.memory_segments:
        arr[segment["name"]] = segment["size"]
    return arr


def makeHeader(mapser):
    outArr = {}
    sectionsSize = getSectionsSize(mapser)
    outArr["datetime"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    outArr["commit"] = os.getenv("COMMIT_HASH", default=None)
    outArr["commit_msg"] = os.getenv("COMMIT_MSG", default=None)
    outArr["branch_name"] = os.getenv("BRANCH_NAME", default=None)
    outArr["bss_size"] = sectionsSize["bss"]
    outArr["text_size"] = sectionsSize["text"]
    outArr["rodata_size"] = sectionsSize["rodata"]
    outArr["data_size"] = sectionsSize["data"]
    outArr["free_flash_size"] = sectionsSize["free_flash"]
    outArr["pullrequest_id"] = os.getenv("PULL_ID", default=None)
    outArr["pullrequest_name"] = os.getenv("PULL_NAME", default=None)
    return outArr


def createTables(cur, conn):
    headerTable = "CREATE TABLE IF NOT EXISTS `header` ( \
            `id` int(10) unsigned NOT NULL AUTO_INCREMENT, \
            `datetime` datetime NOT NULL, \
            `commit` varchar(40) NOT NULL, \
            `commit_msg` text NOT NULL, \
            `branch_name` text NOT NULL, \
            `bss_size` int(10) unsigned NOT NULL, \
            `text_size` int(10) unsigned NOT NULL, \
            `rodata_size` int(10) unsigned NOT NULL, \
            `data_size` int(10) unsigned NOT NULL, \
            `free_flash_size` int(10) unsigned NOT NULL, \
            `pullrequest_id` int(10) unsigned DEFAULT NULL, \
            `pullrequest_name` text DEFAULT NULL, \
            PRIMARY KEY (`id`), \
            KEY `header_id_index` (`id`) )"
    dataTable = "CREATE TABLE IF NOT EXISTS `data` ( \
            `header_id` int(10) unsigned NOT NULL, \
            `id` int(10) unsigned NOT NULL AUTO_INCREMENT, \
            `section` text NOT NULL, \
            `address` int(10) unsigned NOT NULL, \
            `size` int(10) unsigned NOT NULL, \
            `name` text NOT NULL, \
            `lib` text NOT NULL, \
            `obj_name` text NOT NULL, \
            PRIMARY KEY (`id`), \
            KEY `data_id_index` (`id`), \
            KEY `data_header_id_index` (`header_id`), \
            CONSTRAINT `data_header_id_foreign` FOREIGN KEY (`header_id`) REFERENCES `header` (`id`) )"
    cur.execute(headerTable)
    cur.execute(dataTable)
    conn.commit()


def insertHeader(header, cur, conn):
    query = "INSERT INTO `header` ( \
            datetime, commit, commit_msg, branch_name, bss_size, text_size, \
            rodata_size, data_size, free_flash_size, pullrequest_id, pullrequest_name) \
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    cur.execute(
        query,
        (
            header["datetime"],
            header["commit"],
            header["commit_msg"],
            header["branch_name"],
            header["bss_size"],
            header["text_size"],
            header["rodata_size"],
            header["data_size"],
            header["free_flash_size"],
            header["pullrequest_id"],
            header["pullrequest_name"],
        ),
    )
    conn.commit()
    return cur.lastrowid


def setAmapStylePath(line, arr):
    if line["library"].endswith(".a"):
        arr.append(line["path"] + line["library"]) # lib
        arr.append(line["object"]) # object
    else:
        arr.append("")
        arr.append(line["path"] + line["object"])
    return arr


def parseMap(mapser, headerID):
    arr = []
    for line in mapser.symbols_information:
        lineArr = []
        lineArr.append(headerID)
        lineArr.append("." + line["segment"])  # section
        lineArr.append(line["origin"])  # address hex
        lineArr.append(line["size"])  # size
        lineArr.append(line["symbol"])  # name
        lineArr = setAmapStylePath(line, lineArr)
        arr.append(tuple(lineArr))
    return arr


def insertData(data, cur, conn):
    query = "INSERT INTO `data` ( \
            header_id, section, address, size, \
            name, lib, obj_name) \
            VALUES (?, ?, ?, ?, ? ,?, ?)"
    cur.executemany(query, data)
    conn.commit()


def main():
    args = parseArgs()
    mapser = MapFileAnalyzer()
    dbConn = mariadbConnect(args)
    dbCurs = dbConn.cursor()
    mapser.load(args.map_file)
    createTables(dbCurs, dbConn)
    headerID = insertHeader(makeHeader(mapser), dbCurs, dbConn)
    insertData(parseMap(mapser, headerID), dbCurs, dbConn)
    dbCurs.close()


if __name__ == "__main__":
    main()
