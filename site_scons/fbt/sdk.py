import os

from typing import List, Set, ClassVar
import operator

from dataclasses import dataclass, field

from cxxheaderparser.parser import CxxParser

# 'Fixing' complaints about typedefs
CxxParser._fundamentals.discard("wchar_t")

from cxxheaderparser.types import (
    EnumDecl,
    Field,
    ForwardDecl,
    FriendDecl,
    Function,
    Method,
    Typedef,
    UsingAlias,
    UsingDecl,
    Variable,
    Pointer,
    Type,
    PQName,
    NameSpecifier,
    FundamentalSpecifier,
    Parameter,
    Array,
    Value,
    Token,
    FunctionType,
)

from cxxheaderparser.parserstate import (
    State,
    EmptyBlockState,
    ClassBlockState,
    ExternBlockState,
    NamespaceBlockState,
)


@dataclass(frozen=True)
class ApiEntryFunction:
    name: str
    returns: str
    params: str

    csv_type: ClassVar[str] = "Function"

    def dictify(self):
        return dict(name=self.name, type=self.returns, params=self.params)


@dataclass(frozen=True)
class ApiEntryVariable:
    name: str
    var_type: str

    csv_type: ClassVar[str] = "Variable"

    def dictify(self):
        return dict(name=self.name, type=self.var_type, params=None)


@dataclass(frozen=True)
class ApiHeader:
    name: str

    csv_type: ClassVar[str] = "Header"

    def dictify(self):
        return dict(name=self.name, type=None, params=None)


@dataclass
class ApiEntries:
    # These are sets, to avoid creating duplicates when we have multiple
    # declarations with same signature
    functions: Set[ApiEntryFunction] = field(default_factory=set)
    variables: Set[ApiEntryVariable] = field(default_factory=set)
    headers: Set[ApiHeader] = field(default_factory=set)


class SymbolManager:
    def __init__(self):
        self.api = ApiEntries()
        # self.root_headers = []
        self.name_hashes = set()

    # Calculate hash of name and raise exception if it already is in the set
    def _name_check(self, name: str):
        name_hash = gnu_sym_hash(name)
        if name_hash in self.name_hashes:
            raise Exception(f"Hash collision on {name}")
        self.name_hashes.add(name_hash)

    def add_function(self, function_def: ApiEntryFunction):
        if function_def in self.api.functions:
            return
        self._name_check(function_def.name)
        self.api.functions.add(function_def)

    def add_variable(self, variable_def: ApiEntryVariable):
        if variable_def in self.api.variables:
            return
        self._name_check(variable_def.name)
        self.api.variables.add(variable_def)

    def add_header(self, header: str):
        self.api.headers.add(ApiHeader(header))


def gnu_sym_hash(name: str):
    h = 0x1505
    for c in name:
        h = (h << 5) + h + ord(c)
    return str(hex(h))[-8:]


class SdkCollector:
    def __init__(self):
        self.symbol_manager = SymbolManager()

    def add_header_to_sdk(self, header: str):
        self.symbol_manager.add_header(header)

    def process_source_file_for_sdk(self, file_path: str):
        visitor = SdkCxxVisitor(self.symbol_manager)
        with open(file_path, "rt") as f:
            content = f.read()
        parser = CxxParser(file_path, content, visitor, None)
        parser.parse()

    def get_api(self):
        return self.symbol_manager.api


def stringify_array_dimension(size_descr):
    if not size_descr:
        return ""
    return stringify_descr(size_descr)


def stringify_array_descr(type_descr):
    assert isinstance(type_descr, Array)
    return (
        stringify_descr(type_descr.array_of),
        stringify_array_dimension(type_descr.size),
    )


def stringify_descr(type_descr):
    if isinstance(type_descr, (NameSpecifier, FundamentalSpecifier)):
        return type_descr.name
    elif isinstance(type_descr, PQName):
        return "::".join(map(stringify_descr, type_descr.segments))
    elif isinstance(type_descr, Pointer):
        # Hack
        if isinstance(type_descr.ptr_to, FunctionType):
            return stringify_descr(type_descr.ptr_to)
        return f"{stringify_descr(type_descr.ptr_to)}*"
    elif isinstance(type_descr, Type):
        return (
            f"{'const ' if type_descr.const else ''}"
            f"{'volatile ' if type_descr.volatile else ''}"
            f"{stringify_descr(type_descr.typename)}"
        )
    elif isinstance(type_descr, Parameter):
        return stringify_descr(type_descr.type)
    elif isinstance(type_descr, Array):
        # Hack for 2d arrays
        if isinstance(type_descr.array_of, Array):
            argtype, dimension = stringify_array_descr(type_descr.array_of)
            return (
                f"{argtype}[{stringify_array_dimension(type_descr.size)}][{dimension}]"
            )
        return f"{stringify_descr(type_descr.array_of)}[{stringify_array_dimension(type_descr.size)}]"
    elif isinstance(type_descr, Value):
        return " ".join(map(stringify_descr, type_descr.tokens))
    elif isinstance(type_descr, FunctionType):
        return f"{stringify_descr(type_descr.return_type)} (*)({', '.join(map(stringify_descr, type_descr.parameters))})"
    elif isinstance(type_descr, Token):
        return type_descr.value
    elif type_descr is None:
        return ""
    else:
        raise Exception("unsupported type_descr: %s" % type_descr)


class SdkCxxVisitor:
    def __init__(self, symbol_manager: SymbolManager):
        self.api = symbol_manager

    def on_variable(self, state: State, v: Variable) -> None:
        # print("on_variable", v)
        if not v.extern:
            # print("SKIPPING VAR", v)
            return
        self.api.add_variable(
            ApiEntryVariable(
                stringify_descr(v.name),
                stringify_descr(v.type),
            )
        )

    def on_function(self, state: State, fn: Function) -> None:
        # print("on_function", fn)
        if fn.inline or fn.has_body:
            # print("SKIPPING FN", fn.name)
            return
        self.api.add_function(
            ApiEntryFunction(
                stringify_descr(fn.name),
                stringify_descr(fn.return_type),
                ", ".join(map(stringify_descr, fn.parameters))
                + (", ..." if fn.vararg else ""),
            )
        )

    def on_define(self, state: State, content: str) -> None:
        pass

    def on_pragma(self, state: State, content: str) -> None:
        pass

    def on_include(self, state: State, filename: str) -> None:
        pass

    def on_empty_block_start(self, state: EmptyBlockState) -> None:
        pass

    def on_empty_block_end(self, state: EmptyBlockState) -> None:
        pass

    def on_extern_block_start(self, state: ExternBlockState) -> None:
        pass

    def on_extern_block_end(self, state: ExternBlockState) -> None:
        pass

    def on_namespace_start(self, state: NamespaceBlockState) -> None:
        pass

    def on_namespace_end(self, state: NamespaceBlockState) -> None:
        pass

    def on_forward_decl(self, state: State, fdecl: ForwardDecl) -> None:
        pass

    def on_typedef(self, state: State, typedef: Typedef) -> None:
        pass

    def on_using_namespace(self, state: State, namespace: List[str]) -> None:
        pass

    def on_using_alias(self, state: State, using: UsingAlias) -> None:
        pass

    def on_using_declaration(self, state: State, using: UsingDecl) -> None:
        pass

    def on_enum(self, state: State, enum: EnumDecl) -> None:
        pass

    def on_class_start(self, state: ClassBlockState) -> None:
        pass

    def on_class_field(self, state: State, f: Field) -> None:
        pass

    def on_class_method(self, state: ClassBlockState, method: Method) -> None:
        pass

    def on_class_friend(self, state: ClassBlockState, friend: FriendDecl) -> None:
        pass

    def on_class_end(self, state: ClassBlockState) -> None:
        pass


@dataclass(frozen=True)
class SdkVersion:
    major: int = 0
    minor: int = 0

    csv_type: ClassVar[str] = "Version"

    def __str__(self):
        return f"{self.major}.{self.minor}"

    @staticmethod
    def from_str(s: str) -> "SdkVersion":
        major, minor = s.split(".")
        return SdkVersion(int(major), int(minor))

    def dictify(self):
        return dict(name=str(self), type=None, params=None)


import csv
from enum import Enum, auto


class VersionBump(Enum):
    NONE = auto()
    MAJOR = auto()
    MINOR = auto()


# Class that stored all known API entries, both enabled and disabled.
# Also keep track of API versioning
# Allows comparison with newly-generated API
class SdkCache:
    CSV_FIELD_NAMES = ("entry", "status", "name", "type", "params")
    CSV_TYPES = (ApiEntryFunction, ApiEntryVariable, ApiHeader, SdkVersion)

    def __init__(self, cache_file: str):
        self.cache_file_name = cache_file
        self.version = SdkVersion(0, 0)
        self.sdk = ApiEntries()
        self.disabled_entries = set()
        self.new_entries = set()
        self.loaded_dirty = False
        self.loaded_dirty_version = False

        self.version_action = VersionBump.NONE
        self.load_cache()

    def is_buildable(self) -> bool:
        return (
            self.version != SdkVersion(0, 0)
            and self.version_action == VersionBump.NONE
            and not self.loaded_dirty_version
            and not self.loaded_dirty
            and not self.new_entries
        )

    def _filter_enabled(self, sdk_entries):
        return sorted(
            filter(lambda e: e not in self.disabled_entries, sdk_entries),
            key=operator.attrgetter("name"),
        )

    def get_functions(self):
        return self._filter_enabled(self.sdk.functions)

    def get_variables(self):
        return self._filter_enabled(self.sdk.variables)

    def get_headers(self):
        return self._filter_enabled(self.sdk.headers)

    def _get_entry_status(self, entry) -> str:
        if entry in self.disabled_entries:
            return "-"
        elif entry in self.new_entries:
            return "?"
        else:
            return "+"

    def _format_entry(self, obj):
        obj_dict = obj.dictify()
        obj_dict.update(dict(entry=obj.csv_type, status=self._get_entry_status(obj)))
        return obj_dict

    def save(self) -> None:
        version_is_clean = True
        if self.loaded_dirty:
            # There are still new entries and version was already updated
            version_is_clean = False

        if self.version_action == VersionBump.MINOR:
            self.version = SdkVersion(self.version.major, self.version.minor + 1)
            version_is_clean = False
        elif self.version_action == VersionBump.MAJOR:
            self.version = SdkVersion(self.version.major + 1, 0)
            version_is_clean = False

        if version_is_clean:
            print(f"API version {self.version} is up to date")
        else:
            self.new_entries.add(self.version)
            print(
                f"API version is still WIP: {self.version}. Review the changes and re-run command to update."
            )
            print("Entries to review for version ", end="")
            print("\n".join(map(str, self.new_entries)))

        str_cache_entries = [self.version]
        name_getter = operator.attrgetter("name")
        str_cache_entries.extend(sorted(self.sdk.headers, key=name_getter))
        str_cache_entries.extend(sorted(self.sdk.functions, key=name_getter))
        str_cache_entries.extend(sorted(self.sdk.variables, key=name_getter))

        with open(self.cache_file_name, "w", newline="") as f:
            writer = csv.DictWriter(f, fieldnames=SdkCache.CSV_FIELD_NAMES)
            writer.writeheader()

            for entry in str_cache_entries:
                writer.writerow(self._format_entry(entry))

    def _process_entry(self, entry_dict: dict) -> None:
        # print(f"{entry_dict=}")
        entry_class = entry_dict["entry"]
        entry_status = entry_dict["status"]
        entry_name = entry_dict["name"]
        entry_type = entry_dict["type"]
        entry_params = entry_dict["params"]

        entry = None
        if entry_class == "Version":
            self.version = SdkVersion.from_str(entry_name)
            if entry_status == "?":
                self.loaded_dirty_version = True
        elif entry_class == "Header":
            self.sdk.headers.add(entry := ApiHeader(entry_name))
        elif entry_class == "Function":
            self.sdk.functions.add(
                entry := ApiEntryFunction(entry_name, entry_type, entry_params)
            )
        elif entry_class == "Variable":
            self.sdk.variables.add(entry := ApiEntryVariable(entry_name, entry_type))
        else:
            print(entry_dict)
            raise Exception("Unknown entry type: %s" % entry_class)

        if entry is None:
            return

        if entry_status == "-":
            self.disabled_entries.add(entry)
        elif entry_status == "?":
            self.new_entries.add(entry)

    def load_cache(self) -> None:
        if not os.path.exists(self.cache_file_name):
            raise Exception("CANNOT LOAD SYMBOL CACHE! FILE DOES NOT EXIST")

        with open(self.cache_file_name, "r") as f:
            reader = csv.DictReader(f)
            for row in reader:
                # print(row)
                self._process_entry(row)

        self.loaded_dirty = bool(self.new_entries)

    def sync_sets(self, known_set, new_set):
        new_entries = new_set - known_set
        if new_entries:
            print(f"New: {new_entries}")
            known_set |= new_entries
            self.new_entries |= new_entries
            if self.version_action == VersionBump.NONE:
                self.version_action = VersionBump.MINOR
            # self.version = SdkVersion(self.version.major, self.version.minor + 1)
        removed_entries = known_set - new_set
        if removed_entries:
            print(f"Removed: {removed_entries}")
            known_set -= removed_entries
            # If any of removed entries was part of active API, that's a major bump
            if any(filter(lambda e: e not in self.disabled_entries, removed_entries)):
                self.version_action = VersionBump.MAJOR
                # self.loaded_dirty = True

    def validate_api(self, api: ApiEntries) -> None:
        self.sync_sets(self.sdk.headers, api.headers)
        self.sync_sets(self.sdk.functions, api.functions)
        self.sync_sets(self.sdk.variables, api.variables)
