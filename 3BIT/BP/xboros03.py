#
#	Bakalarska praca - Systemy prevodnikov a jejich aplikace
#	Aplikacia - Infix to Postfix Converter
#	Autor - Adrian Boros (xboros03)
#

import re
import enum
import tkinter as tk
from tkinter import messagebox, filedialog

HEIGHT = 500
WIDTH = 800

# Typy tokenov v IFJ18
class Typ(enum.Enum):
    IDENTIFIER = 1
    INTEGER = 2
    FLOAT = 3
    KEYWORD = 4
    ASSIGNMENT = 5
    PLUS = 6
    MINUS = 7
    MULTIPLY = 8
    DIVIDE = 9
    NOT_EQUAL = 10
    LESS_THAN = 11
    LESS_THAN_EQUAL = 12
    MORE_THAN = 13
    MORE_THAN_EQUAL = 14
    END_OF_LINE = 15
    RBRACE = 16
    LBRACE = 17
    TEXT_LITERAL = 18
    LEXICAL_ERROR = 19


# Pole KEYWORDS obsahuje klucove slova ktore sa vyskytuju v jazyku IFJ18
KEYWORDS = ["inputs", "inputi", "inputf", "print", "length", "substr", "ord", "chr",
            "def", "do", "else", "end", "if", "not", "nil", "then", "while"]


# Trieda Token reprezentuje token ktory vracia lexikalny analyzator
class Token:
    def __init__(self, type, alias, value):
        self.type = type
        self.alias = alias
        self.value = value

# Trieda reprezentujuca lexikalny analyzator
class LexicalAnalyzer(object):
    def __init__(self, stream):
        self.stream = stream
        self.current = None
        self.offset = -1
        self.match = None

        self.getChar()

    # Metoda ktora vrati jeden znak zo zadaneho retazca
    def getChar(self):
        if self.offset + 1 >= len(self.stream):
            return None
        result = self.stream[self.offset]
        self.offset += 1
        self.current = self.stream[self.offset]
        return result

    # Metoda pomocou ktorej sa preskocia medzery vo vstupnom retazci
    def skipWhiteSpaces(self):
        while self.current.isspace():
            self.getChar()
            if self.current == '\n':
                break

    # Metoda vyhlada podla regularneho vyrazu zhodu vo vstupnom retazci
    # Ak ziadna zhoda nie je vracia sa None
    def findMatch(self, to_match):
        pattern = re.compile(to_match)
        self.match = re.match(pattern, self.stream[self.offset:])
        if self.match is not None:
            result = self.match.group(0)
            self.offset += len(result)
            if self.offset >= len(self.stream):
                self.current = '\n'
            else:
                self.current = self.stream[self.offset]
            return result
        else:
            return None

    # Metoda identifikuje vo vstupnom retazci jednotlive symboly ktore vracia ako Token
    def scan(self):
        if self.current is None:
            return None

        if self.current.isspace():
            self.skipWhiteSpaces()

        # Jedna sa o identifikator alebo klucove slovo
        if self.current.isalpha() or self.current == '_':
            find = self.findMatch('[a-z_][a-zA-Z_0-9]*[?!]?')
            if find is not None:
                if find not in KEYWORDS:
                    return Token(Typ.IDENTIFIER.name, Typ.IDENTIFIER.value, find)
                else:
                    return Token(Typ.KEYWORD.name, Typ.KEYWORD.value, find)
            else:
                return Token(Typ.LEXICAL_ERROR.name, Typ.LEXICAL_ERROR.value, None)
        # Cele alebo desatinne cislo
        elif self.current.isdigit():
            find = self.findMatch('[0-9]+(\\.[0-9]+)?')
            if find is not None:
                number = float(find)
                if number - int(number) == 0:
                    return Token(Typ.INTEGER.name, Typ.INTEGER.value, find)
                else:
                    return Token(Typ.FLOAT.name, Typ.FLOAT.value, find)
            else:
                return Token(Typ.LEXICAL_ERROR.name, Typ.LEXICAL_ERROR.value, None)
        # Znak '='
        elif self.current == '=':
            if self.getChar() is None:
                self.current = '\n'
            return Token(Typ.ASSIGNMENT.name, Typ.ASSIGNMENT.value, '=')
        # Znak '+'
        elif self.current == '+':
            if self.getChar() is None:
                self.current = '\n'
            return Token(Typ.PLUS.name, Typ.PLUS.value, '+')
        # Znak '-'
        elif self.current == '-':
            if self.getChar() is None:
                self.current = '\n'
            return Token(Typ.MINUS.name, Typ.MINUS.value, '-')
        # Znak '*'
        elif self.current == '*':
            if self.getChar() is None:
                self.current = '\n'
            return Token(Typ.MULTIPLY.name, Typ.MULTIPLY.value, '*')
        # Znak '/'
        elif self.current == '/':
            if self.getChar() is None:
                self.current = '\n'
            return Token(Typ.DIVIDE.name, Typ.DIVIDE.value, '/')
        # Lava zatvorka
        elif self.current == '(':
            if self.getChar() is None:
                self.current = '\n'
            return Token(Typ.LBRACE.name, Typ.LBRACE.value, '(')
        # Prava zatvorka
        elif self.current == ')':
            if self.getChar() is None:
                self.current = '\n'
            return Token(Typ.RBRACE.name, Typ.RBRACE.value, ')')
        # Znak '!='
        elif self.current == '!':
            find = self.findMatch('!=')
            if find is not None:
                return Token(Typ.NOT_EQUAL.name, Typ.NOT_EQUAL.value, '!=')
            else:
                if self.getChar() is None:
                    self.current = '\n'
                return Token(Typ.LEXICAL_ERROR.name, Typ.LEXICAL_ERROR.value, None)
        # Znak '<' alebo '<='
        elif self.current == '<':
            find = self.findMatch('<=')
            if find is not None:
                return Token(Typ.LESS_THAN_EQUAL.name, Typ.LESS_THAN_EQUAL.value, '<=')
            else:
                if self.getChar() is None:
                    self.current = '\n'
                return Token(Typ.LESS_THAN.name, Typ.LESS_THAN.value, '<')
        # Znak '>' alebo '>='
        elif self.current == '>':
            find = self.findMatch('>=')
            if find is not None:
                return Token(Typ.MORE_THAN_EQUAL.name, Typ.MORE_THAN_EQUAL.value, '>=')
            else:
                if self.getChar() is None:
                    self.current = '\n'
                return Token(Typ.MORE_THAN.name, Typ.MORE_THAN.value, '>')
        # Jedna sa o textovy retazec
        elif self.current == '"':
            find = self.findMatch('"(.*)"')
            return Token(Typ.TEXT_LITERAL.name, Typ.TEXT_LITERAL.value, find)
        elif self.current == '\n':
            self.current = None
            return Token(Typ.END_OF_LINE.name, Typ.END_OF_LINE.value, '')
        # Ak ani jedna z moznosti nesedi jedna sa o chybu
        else:
            return Token(Typ.LEXICAL_ERROR.name, Typ.LEXICAL_ERROR.value, None)


class SyntaxAnalyzer(object):
    def __init__(self):
        self.checkList = []
        self.express = ""
        self.current = ""
        self.top = ""

    # Metoda ziska token od lexikalneho analyzatoru a nasledne ak ma k dispozicii aspon 2 tokeny
    # vykona kontrolu ci je kombinacia jednotlivych prvkov syntakticky spravna
    def getToken(self, token):
        self.checkList.append(token)
        if len(self.checkList) == 1:
            self.top = token.alias
            if self.top in range(5, 16):
                return False
        else:
            self.current = token.alias
            if self.top == 1 and self.current not in range(5, 17):
                return False
            elif self.top in [2, 3] and self.current not in range(6, 17):
                return False
            elif self.top in range(5, 15) and self.current not in [1, 2, 3, 17]:
                return False
            elif self.top == 17 and self.current not in [1, 2, 3, 17]:
                return False
            elif self.top == 16 and self.current not in range(5, 17):
                return False
            # elif self.top in range(5, 15) and self.current == 16:
            #   return False
            else:
                self.top = self.current

    # Metoda vrati skontrolovany vyraz
    def exp_to_conv(self):
        str_exp = ""
        self.checkList = self.checkList[:-1]
        for exp in self.checkList:
            if exp.type == Typ.KEYWORD.name:
                return ""
            else:
                str_exp += exp.value
        return str_exp

    # Metoda skontroluje ci sa jedna o vyraz, ak nie preskakuje sa na druhy riadok na vstupu
    def is_expression(self, token):
        if token.type == Typ.KEYWORD.name:
            self.checkList.clear()
            return False

    # Metoda skontroluje ci sa vo vyraze nachadza aspon jedna zatvorka
    def bracket_occurence(self):
        for ch in self.checkList:
            if ch.type == Typ.RBRACE.name or ch.type == Typ.LBRACE.name:
                return True


# Trieda ktora skontroluje ci sedi pocet pravych a lavych zatvoriek
class BracketController(object):
    def __init__(self, expression):
        self.expression = expression

    def areBracketsBalanced(self):
        s = []
        for i in range(len(self.expression)):
            if self.expression[i] == '(':
                s.append(self.expression[i])
                continue
            if len(s) == 0 and len(self.expression) == 0:
                return False
            if self.expression[i] == ')':
                if len(s) == 0:
                    return False
                else:
                    s.pop()
        if len(s) == 0:
            return True
        else:
            return False


# Trieda ktora konvertuje infixovy vyraz na postfixovy
class ExpressionConverter:
    def __init__(self, capacity):
        self.capacity = capacity
        self.size = -1
        self.array = []
        self.output = []
        self.precedence = {'=': 1, '!=': 1, '<': 2, '>': 2, '<=': 2, '>=': 2, '+': 3, '-': 3, '*': 4, '/': 4}

    # Kontrola ci je zasobnik prazdny
    def isEmpty(self):
        if self.size == -1:
            return True
        else:
            return False

    # Ulozenie symbolu na zasobnik
    def push(self, val):
        self.array.append(val)
        self.size += 1

    # Odstrani symbol z vrcholu zasobnika
    def pop(self):
        if self.isEmpty():
            return "$"
        else:
            self.size -= 1
            return self.array.pop()

    # Vrati symbol z vrcholu zasobnika
    def top(self):
        if self.isEmpty():
            return False
        else:
            return self.array[-1]

    # Kontrola ci sa jedna o operand vo vyraze
    def isOperand(self, ch):
        return ch.isalpha() or ch.isdigit() or ch == '.' or ch == '_' or ch == '?'

    # Metoda na porovnanie priorit jednotlivych operatorov
    def notGreater(self, i):
        try:
            a = self.precedence[i]
            b = self.precedence[self.top()]
            if a <= b:
                return True
            else:
                return False
        except KeyError:
            return False

    # Prevod vyrazu
    def infixToPostfix(self, exp):
        idx = 0
        while idx < len(exp):
            current_ch = exp[idx]
            try:
                next_ch = exp[idx + 1]
            except IndexError:
                next_ch = ''
            if self.isOperand(current_ch):
                self.output.append(current_ch)
            elif current_ch == '(':
                self.push(current_ch)
            elif current_ch == ')':
                o = self.pop()
                while o != '(':
                    self.output.append(' ')
                    self.output.append(o)
                    o = self.pop()
            else:
                self.output.append(' ')
                if current_ch in ['<', '>', '!'] and next_ch == '=':
                    current_ch += next_ch
                while len(self.array) and self.notGreater(current_ch):
                    self.output.append(self.pop())
                    self.output.append(' ')
                self.push(current_ch)
            if current_ch in ['<=', '>=', '!=']:
                idx += 2
            else:
                idx += 1
        while not self.isEmpty():
            self.output.append(' ')
            self.output.append(self.pop())

        return "".join(self.output)


# Trieda reprezentujuca system prevodnikov
class TransducerSystem:
    def __init__(self, root_elem, entry_elem, output, input_frame):
        self.expression = None
        self.root = root_elem
        self.input = entry_elem
        self.output = output
        self.frame_text_input = input_frame

        # Prvky GUI
        convert_button = tk.Button(self.root, text='Convert', command=self.conversion)
        convert_button.configure(font=("Comic Sans MS", 18), bg='#616161', foreground='white')
        convert_button.place(relx=0.5, rely=0.85, relheight=0.1, relwidth=0.3, anchor='n')

        open_file_button = tk.Button(self.frame_text_input, text='Open file', command=self.openFile)
        open_file_button.configure(font=("Comic Sans MS", 13), bg='#616161', foreground='white')
        open_file_button.place(relx=0.83, relheight=1, relwidth=0.35, anchor='n')
        self.root.mainloop()

    # Otvorenie suboru
    def openFile(self):
        x = filedialog.askopenfilename(initialdir='./', title="Select file", filetypes=[("Text files", ".txt")])
        if x != "":
            text = open(x).read().splitlines()
            for i in text:
                if i != '':
                    self.input.insert(tk.END, i)
                    self.input.insert(tk.END, '\n')

    def conversion(self):
        no_output = True
        syntax_analyzer = SyntaxAnalyzer()
        lines = self.input.get("1.0", tk.END).splitlines()
        i = 0
        for item in lines:
            error_occurence = False
            if item == "":
                continue
            lex_analyzer = LexicalAnalyzer(item)
            returned_token = lex_analyzer.scan()
            while returned_token is not None:
                if returned_token.type == Typ.LEXICAL_ERROR.name:
                    message = "Invalid token in expression: " + item
                    messagebox.showerror("Lexical error", message)
                    self.output['text'] = ""
                    no_output = False
                    error_occurence = True
                    syntax_analyzer.checkList = []
                    break
                if syntax_analyzer.getToken(returned_token) is False:
                    if returned_token.type != Typ.KEYWORD.name:
                        message = "Incorrect syntax in expression: " + item
                        messagebox.showerror("Syntax error", message)
                        self.output['text'] = ""
                        no_output = False
                        error_occurence = True
                        syntax_analyzer.checkList = []
                        break
                if syntax_analyzer.is_expression(returned_token) is False:
                    break
                returned_token = lex_analyzer.scan()
            if error_occurence:
                continue
            expression = syntax_analyzer.exp_to_conv()
            if syntax_analyzer.bracket_occurence() is True:
                bracketController = BracketController(expression)
                if bracketController.areBracketsBalanced() is False:
                    message = "Wrong bracket parenthesses in expression: " + item
                    messagebox.showerror("Syntax error", message)
                    self.output['text'] = ""
                    no_output = False
                    continue
            syntax_analyzer.checkList = []
            converter = ExpressionConverter(len(expression))
            result = converter.infixToPostfix(expression)
            if result != "":
                if i == 0:
                    self.output['text'] = result
                    i += 1
                    no_output = False
                else:
                    no_output = False
                    text = self.output.cget("text") + '\n' + result
                    self.output.configure(text=text)
                    self.output['text'] = text
        if no_output is True:
            messagebox.showinfo("Warning", "There are no expressions in source code.")
            self.output['text'] = ""

# Prvky grafickeho uzivatelskeho rozhrania
root = tk.Tk()

root.title('Infix to Postfix converter')

canvas = tk.Canvas(root, height=HEIGHT, width=WIDTH)
canvas.pack()

background_image = tk.PhotoImage(file='img1.gif')
background_label = tk.Label(root, image=background_image)
background_label.place(x=0, y=0, relwidth=1, relheight=1)

frame_text_input = tk.Frame(root, bg='#343434', bd=8)
frame_text_input.place(relx=0.05, rely=0.08, relwidth=0.4, relheight=0.1)

source_label = tk.Label(frame_text_input, bg='#343434', foreground='white')
source_label.configure(font=("Comic Sans MS", 15))
source_label.place(relwidth=0.65, relheight=1)
source_label['text'] = "Insert your code or "

frame_input = tk.Frame(root, bg='#343434', bd=8)
frame_input.place(relx=0.05, rely=0.2, relwidth=0.4, relheight=0.6)

entry = tk.Text(frame_input)
entry.configure(font=("Courier", 12))
entry.place(relwidth=1, relheight=1)

frame_output = tk.Frame(root, bg='#343434', bd=8)
frame_output.place(relx=0.55, rely=0.2, relwidth=0.4, relheight=0.6)

frame_output_text = tk.Frame(root, bg='#343434', bd=8)
frame_output_text.place(relx=0.55, rely=0.08, relwidth=0.4, relheight=0.1)

source_label = tk.Label(frame_output_text, bg='#343434', foreground='white')
source_label.configure(font=("Comic Sans MS", 15))
source_label.place(relx=-0.03, relwidth=0.65, relheight=1)
source_label['text'] = "Conversion result:"

info_frame = tk.Frame(root, bg='#343434', bd=4)
info_frame.place(relx=0.8, rely=0.95, relwidth=0.8, relheight=0.6)

info_label = tk.Label(info_frame, bg='#343434', foreground='white')
info_label.configure(font=("Comic Sans MS", 8), anchor='nw', justify='left')
info_label.place(relwidth=0.65, relheight=1)
info_label['text'] = "Copyright 2020 Adrian Boros"

output_label = tk.Label(frame_output)
output_label.configure(font=("Courier", 12), bg='white', anchor='nw', justify='left')
output_label.place(relwidth=1, relheight=1)

app = TransducerSystem(root, entry, output_label, frame_text_input)
