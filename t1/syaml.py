#!/usr/bin/env python3

#
# Skeleton for ECE650 Test 1
# DO NOT DISTRIBUTE
#
# Complete the two provided functions. Implement as much functionality as you
# can. Make sure to not change the interface.
#
# More details are in the question on LEARN.
#
# You are allowed to use a Python IDE and Python Documentation available at
# https://docs.python.org/3/
#
# You are very likely to need to add extra functions and/or classes to organize
# your code. We will mark your code for functionality only. Using extra
# functions is not required, but is highly advised
#
import re


def print_syaml(data):
    """Print data in SYAML format. Raise an exception if this is not possible"""
    res = []
    if isinstance(data, dict) or isinstance(data, list):
        print("---")
        if isinstance(data, list):
            for item in data:
                if isinstance(item, str):
                    print("- " + str(item))
                elif isinstance(item, dict):
                    for key_n, value_n in item.items():
                        if isinstance(key_n, str) and isinstance(value_n, str):
                            res.append(key_n + ": " + value_n)
                        else:
                            raise Exception('Wrong type of element in dictionary')

                    for idx in range(len(res)):
                        if idx == 0:
                            print("- " + res[idx])
                        else:
                            print("  " + res[idx])
                    res = []
                else:
                    raise Exception('Wrong type of element in list: ' + str(item.__class__))
        elif isinstance(data, dict):
            for key, value in data.items():
                if isinstance(key, str) and isinstance(value, str):
                    print(key + ": " + value)
                elif isinstance(key, str) and isinstance(value, list):
                    print(key + ":")
                    for item in data:
                        if isinstance(item, str):
                            print(" " * len(key) + "- " + str(item))
                else:
                    raise Exception(
                        'Wrong type of element in dictionary: ' + "key: " + str(key.__class__) + " value: " + str(
                            value.__class__))
        print("...")
    else:
        raise Exception("Wrong type of data: " + str(data.__class__))


def parse_syaml(lines):
    """Parse SYAML document. See LEARN for details."""
    parse_lst = []
    res_lst = []
    res_dict = {}
    res = None
    symbol = ":-'\"\n"
    if not isinstance(lines, list):
        raise Exception("Expecting a list of strings")
    if not (len(lines) >= 2 and lines[0] == "---\n" and lines[-1] == "...\n"):
        raise Exception("Begin or end document is missing")
    for line in lines:
        parse_lst.append(line.strip())
    parse_lst = parse_lst[1:-1]
    for item in parse_lst:
        if re.findall("(.+): (.+)", item):
            re_res = re.findall("(.+): (.+)", item)
            for s in re_res[0][0]:
                if s in symbol:
                    raise Exception("Unexpected symbol")
            for s in re_res[0][1]:
                if s in symbol:
                    raise Exception("Unexpected symbol")
            res_dict[re_res[0][0]] = re_res[0][1]
            res = res_dict
        elif re.findall("- (.+)", item):
            if item[0] != "-":
                raise Exception("Unexpected format")
            re_res = re.findall("- (.+)", item)
            for s in re_res[0]:
                if s in symbol:
                    raise Exception("Unexpected symbol")
            res_lst.append(re_res[0])
            res = res_lst
    if not res:
        raise Exception("Unexpected format")
    return res


# This is a simple test function provided from your convenience
if __name__ == "__main__":
    d1 = {
        "one": [
            "un",
            "uno"
        ],
        "two": ["deux", "dos"]
    }

    print_syaml(d1)

    t1 = ["---\n",
          "one: un\n",
          "two: deux\n",
          "three: trois\n",
          "...\n"]

    t2 = ["---\n",
          "- one\n",
          "- two\n",
          "- three\n",
          "...\n"]

    d2 = parse_syaml(t1)
    print(d2)
    print_syaml(d2)
