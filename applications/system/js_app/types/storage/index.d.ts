export type AccessMode = "r" | "w" | "rw";
export type OpenMode = "open_existing" | "open_always" | "open_append" | "create_new" | "create_always";
export type Timestamp = number;

export declare class FileInfo {
    path: string;
    isDirectory: boolean;
    size: number;
    accessTime: Timestamp;
}

export declare class FsInfo {
    totalSpace: number;
    freeSpace: number;
}

// file operations

export declare class File {
    close(): boolean;
    isOpen(): boolean;
    read<T extends ArrayBuffer | string>(mode: T extends ArrayBuffer ? "binary" : "ascii", bytes: number): T;
    write(data: ArrayBuffer | string): number;
    seekRelative(bytes: number): boolean;
    seekAbsolute(bytes: number): boolean;
    tell(): number;
    truncate(): boolean;
    size(): number;
    eof(): boolean;
    copyTo(dest: File, bytes: number): boolean;
}

export declare function openFile(path: string, accessMode: AccessMode, openMode: OpenMode): File | undefined;
export declare function fileExists(path: string): boolean;

// directory operations

export declare function readDirectory(path: string): FileInfo[] | undefined;
export declare function directoryExists(path: string): boolean;
export declare function makeDirectory(path: string): boolean;

// common (file/dir) operations
export declare function fileOrDirExists(path: string): boolean;
export declare function stat(path: string): FileInfo | undefined;
export declare function remove(path: string): boolean;
export declare function rmrf(path: string): boolean;
export declare function rename(oldPath: string, newPath: string): boolean;
export declare function copy(oldPath: string, newPath: string): boolean;
export declare function fsInfo(filesystem: string): FsInfo | undefined;
export declare function nextAvailableFilename(dirPath: string, fileName: string, fileExt: string, maxLen: number): string;

// path operations that do not access the filesystem

export declare function arePathsEqual(path1: string, path2: string): boolean;
export declare function isSubpathOf(parentPath: string, childPath: string): boolean;

