#!/usr/bin/env python
"""first_challenge.py: First challenge of the class :)"""

__author__ = "hivini"

def get_length(lst):
    """ Gets the number of elements in a list, allows nested lists.

    This is the answer of the following challenge:
        https://edabit.com/challenge/xmwdk2qwyZEt7ph49
    """
    counter = 0
    for element in lst:
        if isinstance(element, list):
            counter += get_length(element)
        else:
            counter += 1
    return counter


if __name__ == "__main__":
    lst = [1, [2, 3, 4], 5, [6, [7, 8, 9, [10, 11]]]]
    print("Simple test result (see source code): {0}".format(get_length(lst)))
