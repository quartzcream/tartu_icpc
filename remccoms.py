#!/usr/bin/python3
import sys

def source_code(char):
    if char == '/':
        return comment_begin, ''
    return source_code, char

def comment_begin(char):
    if char == '/':
        return inline_comment, ''
    if char == '*':
        return block_comment, ''
    return source_code, '/'+char

def inline_comment(char):
    if char == '\n':
         return source_code, char
    return inline_comment, ''

def block_comment(char):
    if char == '*':
        return end_block_comment, ''
    if(char == '\n'):
        return block_comment, '\n'
    return block_comment, ''

def end_block_comment(char):
    if char == '/':
        return source_code, ''
    if(char == '\n'):
        return block_comment, '\n'
    return block_comment, ''

def remove_comments(content):
    def gen_content():
        parser = source_code
        for character in content:
            parser, text = parser(character)
            yield text

    return ''.join(gen_content())

input_str = sys.stdin.read()
print(remove_comments(input_str))
