import os
import sys

char_type = "char"

app_path = "." if len(sys.argv) < 2 else sys.argv[1]
qustions_path = os.path.join(app_path, "question_lists")
questions_output = os.path.join(app_path, "qdb.h")

lists = {}

prefix = "// This file was generated with generate_questions.py\n#ifndef QDB\n#define QDB\n#include <furi.h>\n"
struct = (
    f"typedef {char_type} q_char;\n"
    + "typedef struct QuestionList {const char * name; uint16_t size; q_char** questions;} QuestionList;"
)

for file in [os.path.join(qustions_path, entry) for entry in os.listdir(qustions_path)]:
    if not os.path.isfile(file):
        continue
    with open(file, "r") as f:
        lists[os.path.splitext(os.path.basename(file))[0]] = [
            line for line in f.read().split("\n") if line and not line.isspace()
        ]


def create_arrays():
    def replace_specials(l):
        res = l
        for special in ["\\", '"', "?", "'"]:
            if special in res:
                res = res.replace(special, "\\" + special)
        return res

    arrays = []
    for key in lists:
        array_content = ", ".join(
            [f'"{replace_specials(q).upper()}"' for q in lists[key]]
        )
        arrays.append(
            f"static q_char * q_arr_{key}[] = " + "{ " + array_content + " };"
        )
    return "\n".join(arrays)


def create_structs():
    structs = []
    for key in lists:
        structs.append("{ " + f'"{key.upper()}", {len(lists[key])}, q_arr_{key}' + " }")
    structs.append("{ " + f'"ALL", {sum([len(lists[x]) for x in lists])}, NULL' + " }")
    return "static struct QuestionList Q_GROUPS[] = { " + ", ".join(structs) + " };"


suffix = "static const uint8_t NUMBER_OF_GROUPS = {};\nq_char** q_shuffle(const uint8_t gi);\n#endif\n".format(
    len(lists) + 1
)

code = [prefix, struct, create_arrays(), create_structs(), suffix]

with open(questions_output, "w") as f:
    f.write("\n".join(code))
