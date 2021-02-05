#
#	Name: Adrian Boros
#	File: interpret.py
#	Date: 11.04.2019
#

# Used libraries
import sys
import getopt
import xml.etree.ElementTree as ET
import os
import re
from enum import Enum

# Error values
PARAM_ERROR = 10
FILE_IN_ERR = 11
ERROR_XML_FORMAT = 31
ERROR_XML_SYNTAX = 32
ERROR_SEMANTIC = 52
ERROR_OPERAND = 53
ERROR_VARIABLE = 54
ERROR_FRAME = 55
ERROR_MISS_VALUE = 56
ERROR_BAD_VALUE = 57
ERROR_STRING_WORK = 58

# Global variables
INSTRUCTIONS_TYPE = ["int", "string", "nil", "bool", "label", "var", "type"]
sourceFile = ''
inputFile = ''
labels_list = {}

# Function to print error messages and exit program with exit code
def printErrMessage(msg, err_code):
	print(msg, file = sys.stderr)
	sys.exit(err_code)

# Function which print help
def printHelp():
	print("-------------------NAPOVEDA--------------------\n"
		"Program nacte XML reprezentaci programu ze zadaneho\n"
		"souboru a tento program s vyuzitim standardniho\n"
		"vstupu a vystupu interpretuje. Vstupní XML reprezentace\n"
		"je napr. generována skriptem parse.php(ale ne nutne)\n"
		"ze zdrojoveho kodu v IPPcode19.")
	sys.exit(0)

class Variable():
	"""docstring for Variable"""
	def __init__(self, name):
		self.name = name
		self.var_type = None
		self.value = None

class Interpret():
	"""docstring for Interpret"""
	global_frame = {}
	frame_stack = []
	data_stack = []
	local_frame = None
	temp_frame = None
	call_stack = []
	args_number = 0
	total_instr = 0

	# Check instruction structure
	def checkInstruct(self, child):
		if len(child.attrib) != 2:
			printErrMessage("ERROR: Invalid XML structure.", ERROR_XML_SYNTAX)
		if child.tag != "instruction" or "order" not in child.attrib or "opcode" not in child.attrib:
			printErrMessage("ERROR: Invalid XML structure.", ERROR_XML_SYNTAX)
	
	# Check arguments and sort them
	def checkArgs(self, child):
		arg_list = []
		self.args_number = 0
		for args in child:
			if "type" not in args.attrib or len(args.attrib) != 1:
				printErrMessage("ERROR: Invalid XML structure, missing or invalid type attribute.", ERROR_XML_SYNTAX)
			if args.attrib["type"] not in INSTRUCTIONS_TYPE:
				printErrMessage("ERROR: Invalid type in arguments.", ERROR_XML_SYNTAX)
			arg_num = args.tag.split("arg")	
			arg_list.append([arg_num[1], args.attrib["type"], args.text])
			self.args_number+=1
		arg_list.sort()
		arg_order = 1
		for arg in arg_list:
			if int(arg[0]) != arg_order:
				printErrMessage("ERROR: Wrong argument numbers for instruction.", ERROR_XML_SYNTAX)
			arg_order+=1
		return arg_list
	
	# Check number of arguments passed to instruction
	def argNumPassed(self, need, opcode):
		if self.args_number != need:
			printErrMessage("ERROR: Bad number of params passed for instruction {0}.".format(opcode), ERROR_XML_SYNTAX)
		
	def AddInstr(self, opcode, args, labels_list, input_list, counter, opts):
		# Instructions without arguments
		opcode = opcode.upper()
		self.total_instr+=1
		if opcode in ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]:
			self.argNumPassed(0, opcode)
			if opcode == "CREATEFRAME":
				self.temp_frame = {}
			elif opcode == "PUSHFRAME":
				self.pushFrame()
			elif opcode == "POPFRAME":
				self.popFrame()
			elif opcode == "BREAK":
				self.breakInstr()
			elif opcode == "RETURN":
				if len(self.call_stack) == 0:
					printErrMessage("ERROR: Call stack is empty.", ERROR_MISS_VALUE)
				call_return = self.call_stack.pop()
				return call_return

		# Instructions with one arg <var>
		elif opcode in ["DEFVAR", "POPS"]:
			self.argNumPassed(1, opcode)
			self.checkVar(args[0][1], args[0][2])
			if opcode == "DEFVAR":
				self.def_var(args[0][2])
			elif opcode == "POPS":
				frame, name = args[0][2].split("@")
				self.pops(args[0][2])
		# Instrcutions with one arg <label>
		elif opcode in ["CALL", "LABEL", "JUMP"]:
			self.argNumPassed(1, opcode)
			self.checkLabel(args[0][1], args[0][2])
			if opcode == "CALL":
				label = self.existLabel(args[0][2], labels_list)
				self.call_stack.append(counter)
				return int(labels_list[label]) - 1
			elif opcode == "JUMP":
				label = self.existLabel(args[0][2], labels_list)
				return int(labels_list[label]) - 1 

		# Instructions with one arg <symb>
		elif opcode in ["PUSHS", "WRITE", "EXIT", "DPRINT"]:
			self.argNumPassed(1, opcode)
			self.checkSymb(args[0][1], args[0][2])
			if opcode in ["WRITE", "DPRINT"]:
				self.write(opcode, args[0][1], args[0][2])
			elif opcode == "EXIT":
				if args[0][1] == "int" or args[0][1] == "var":
					if args[0][1] == "var":
						frame, name = args[0][2].split("@")
						var = self.getFromFrame(frame, name)
						if var.var_type == None:
							printErrMessage("ERROR: Instruction EXIT must have argument integer.", ERROR_MISS_VALUE)
						if var.var_type != "int":
							printErrMessage("ERROR: Instruction EXIT must have argument integer.", ERROR_OPERAND)
						if int(var.value) < 0 or int(var.value) > 49:
							printErrMessage("ERROR: Instruction EXIT must have argument integer in range <0,49>.", ERROR_BAD_VALUE)
						sys.exit(int(var.value))
					else:
						if int(args[0][2]) < 0 or int(args[0][2]) > 49:
							printErrMessage("ERROR: Instruction EXIT must have argument integer in range <0,49>.", ERROR_BAD_VALUE)
						else:
							sys.exit(int(args[0][2]))
				elif args[0][1] != "int" or args[0][1] != "var":
					printErrMessage("ERROR: Instruction EXIT must have argument integer in range <0,49>.", ERROR_OPERAND)
			elif opcode == "PUSHS":
				self.pushs(args[0][1], args[0][2])

		# Instructions with two args <var> <symb>
		elif opcode in ["MOVE", "INT2CHAR", "STRLEN", "TYPE", "NOT"]:
			self.argNumPassed(2, opcode)	
			self.checkVar(args[0][1], args[0][2])
			if args[1][2] is not None:
				self.checkSymb(args[1][1], args[1][2])
			if opcode == "MOVE":
				self.move_var(args[0][2], args[1])
			elif opcode == "INT2CHAR":
				if args[1][1] == "var":
					frame1, name1 = args[1][2].split("@")
					var1 = self.getFromFrame(frame1, name1)
					if var1.value == None:
						printErrMessage("ERROR: Variable is not initialized.", ERROR_MISS_VALUE)
					elif var1.var_type != "int":
						printErrMessage("ERROR: Type of symb must be int.", ERROR_OPERAND)
					else:
						frame2, name2 = args[0][2].split("@")
						var2 = self.getFromFrame(frame2, name2)
						try:
							ord_chr = chr(int(var1.value))
							self.setValue(frame2, name2, "string", ord_chr)
						except ValueError:
							printErrMessage("ERROR: Non valid value of Unicode.", ERROR_STRING_WORK)
				elif args[1][1] == "int":
					frame, name = args[0][2].split("@")
					try:
						ord_chr = chr(int(args[1][2]))
						self.setValue(frame, name, "string", ord_chr)
					except ValueError:
						printErrMessage("ERROR: Non valid value of Unicode.", ERROR_STRING_WORK)
				else:
					printErrMessage("ERROR: Type of symb must be integer.", ERROR_OPERAND)
			elif opcode == "STRLEN":
				if args[1][1] == "var":
					frame1, name1 = args[1][2].split("@")
					var1 = self.getFromFrame(frame1, name1)
					if var1.value == None:
						printErrMessage("ERROR: Variable is not initialized.", ERROR_MISS_VALUE)
					elif var1.var_type != "string":
						printErrMessage("ERROR: Type of symb must be string.", ERROR_OPERAND)
					else:
						frame2, name2 = args[0][2].split("@")
						var2 = self.getFromFrame(frame2, name2)
						self.setValue(frame2, name2, "int", len(var1.value))
				elif args[1][1] == "string":
					frame, name = args[0][2].split("@")
					if args[1][2] == None:
						self.setValue(frame, name, "int", 0)
					else:
						string = self.convertEscape(args[1][2])
						self.setValue(frame, name, "int", len(string))
				else:
					printErrMessage("ERROR: Type of symb must be string.", ERROR_OPERAND)
			elif opcode == "TYPE":
				frame, name = args[0][2].split("@")
				var = self.getFromFrame(frame, name)
				if args[1][1] == "var":
					frame1, name1 = args[1][2].split("@")
					var1 = self.getFromFrame(frame1, name1)
					if var1.var_type == None:
						self.setValue(frame, name, "string", "")
					else:
						self.setValue(frame, name, "string", var1.var_type)
				else:
					self.setValue(frame, name, "string", args[1][1])
			elif opcode == "NOT":
				if args[1][1] == "var":
					frame, name = args[0][2].split("@")
					var = self.getFromFrame(frame, name)
					frame1, name1 = args[1][2].split("@")
					var1 = self.getFromFrame(frame1, name1) 
					if var1.var_type != "bool":
						printErrMessage("ERROR: Type of var must be bool.", ERROR_OPERAND)
					if var1.value == "true":
						self.setValue(frame, name, "bool", "false")
					elif var1.value == "false":
						self.setValue(frame, name, "bool", "true")
				else:
					frame, name = args[0][2].split("@")
					var = self.getFromFrame(frame, name)
					if args[1][1] != "bool":
						printErrMessage("ERROR: Type of <symb> must be bool.", ERROR_OPERAND)
					if args[1][2] == "true":
						self.setValue(frame, name, "bool", "false")
					elif args[1][2] == "false":
						self.setValue(frame, name, "bool", "true")

		# Instruction with two args <var> <type>
		elif opcode == "READ":
			self.argNumPassed(2, opcode)
			self.checkVar(args[0][1], args[0][2])
			frame, name = args[0][2].split("@")
			var = self.getFromFrame(frame, name)
			if opts[0][0] == "--source" and len(sys.argv) == 2:
				value = input()
				if args[1][2] == "int":
					try:
						val = int(value)
						self.setValue(frame, name, args[1][2], val)
					except Exception:
						self.setValue(frame, name, args[1][2], 0)
				elif args[1][2] == "string":
					val = str(value)
					self.setValue(frame, name, args[1][2], val)
				elif args[1][2] == "bool":
					if value.lower() == "true":
						self.setValue(frame, name, args[1][2], value.lower())
					else:
						self.setValue(frame, name, args[1][2], "false")
				else:
					printErrMessage("ERROR: Type can not be nil.", ERROR_XML_SYNTAX)
			elif opts[0][0] == "--input" or opts[1][0] == "--input":
				if args[1][2] == "int":
					try:
						if input_list:
							value = input_list[0]
							val = int(value)
							self.setValue(frame, name, args[1][2], val)
							input_list.pop(0)
						else:
							self.setValue(frame, name, args[1][2], 0)
							input_list.pop(0)
					except Exception:
						self.setValue(frame, name, args[1][2], 0)
				elif args[1][2] == "string":
					if input_list:
						value = input_list[0]
						val = str(value)
						self.setValue(frame, name, args[1][2], val)
						input_list.pop(0)
					else:
						self.setValue(frame, name, args[1][2], "")
				elif args[1][2] == "bool":
					if input_list:
						value = input_list[0]
						if value.lower() == "true":
							self.setValue(frame, name, args[1][2], value.lower())
							input_list.pop(0)
						else:
							self.setValue(frame, name, args[1][2], "false")
							input_list.pop(0)
					else:
						self.setValue(frame, name, args[1][2], "false")
				else:
					printErrMessage("ERROR: Type can not be nil.", ERROR_XML_SYNTAX)	

		# Instructions with three args <var> <symb1> <symb2>
		elif opcode in ["ADD", "MUL", "IDIV", "SUB", "LT", "GT", "EQ", "AND", "OR", "STRI2INT", "CONCAT", "SETCHAR", "GETCHAR"]:
			self.argNumPassed(3, opcode)
			self.checkVar(args[0][1], args[0][2])
			self.checkSymb(args[1][1], args[1][2])
			self.checkSymb(args[2][1], args[2][2])
			frame, name = args[0][2].split("@")
			if opcode in ["ADD", "MUL", "IDIV", "SUB"]:
				self.arithmeticOper(opcode, args[0], args[1], args[2])
			elif opcode in ["AND", "OR"]:
				self.andOrInstruct(args[0], opcode, args[1], args[2])
			elif opcode in ["LT", "GT", "EQ"]:
				self.comparisonInstruct(opcode, args[0], args[1], args[2])
			elif opcode == "STRI2INT":
				self.stri2int(args[0], opcode, args[1], args[2])
			elif opcode == "CONCAT":
				self.concat(args[0], opcode, args[1], args[2])
			elif opcode == "SETCHAR":
				self.setchar(opcode, args[0], args[1], args[2])
			elif opcode == "GETCHAR":
				self.getchar(opcode, args[0], args[1], args[2])

		# Instructions with three args <label> <symb1> <symb2>
		elif opcode in ["JUMPIFEQ", "JUMPIFNEQ"]:
			self.argNumPassed(3, opcode)
			self.checkLabel(args[0][1], args[0][1])
			label = self.existLabel(args[0][2], labels_list)
			if opcode == "JUMPIFEQ":
				if args[1][1] == "var":
					frame, name = args[1][2].split("@")
					var = self.getFromFrame(frame, name)
					type1 = var.var_type
					value1 = var.value
				else:
					type1 = args[1][1]
					value1 = args[1][2]
				if args[2][1] == "var":
					frame1, name1 = args[2][2].split("@")
					var1 = self.getFromFrame(frame1, name1)
					type2 = var1.var_type
					value2 = var1.value
				else:
					type2 = args[2][1]
					value2 = args[2][2]
				if type1 != type2:
					printErrMessage("ERROR: Types are not same.", ERROR_OPERAND)
				if type1 == type2 and value1 == value2:
					return int(labels_list[label]) - 1
			elif opcode == "JUMPIFNEQ":
				if args[1][1] == "var":
					frame, name = args[1][2].split("@")
					var = self.getFromFrame(frame, name)
					type1 = var.var_type
					value1 = var.value
				else:
					type1 = args[1][1]
					value1 = args[1][2]
				if args[2][1] == "var":
					frame1, name1 = args[2][2].split("@")
					var1 = self.getFromFrame(frame1, name1)
					type2 = var1.var_type
					value2 = var1.value
				else:
					type2 = args[2][1]
					value2 = args[2][2]
				if type1 != type2:
					printErrMessage("ERROR: Types are not same.", ERROR_OPERAND)
				if type1 == type2 and value1 != value2:
					return int(labels_list[label]) - 1
		else:
			printErrMessage("ERROR: Unsupported instruction used.", ERROR_XML_SYNTAX)
		return counter

	# Check if label contains legal chars
	def checkLabel(self, type, name):
		if type != "label":
			printErrMessage("ERROR: Invalid type used.", ERROR_XML_SYNTAX)
		if not re.search(r"^([a-zA-Z]|[_\-$&%*!?])([\w_\-$&%*!?])*$", name):
			printErrMessage("ERROR: Label contains unsupported characters.", ERROR_XML_SYNTAX)

	# Check if label was created
	def existLabel(self, name, labels_list):
		if name not in labels_list:
			printErrMessage("ERROR: Label does not exists.", ERROR_SEMANTIC)
		else:
			return name

	# Check if <var> is correct
	def checkVar(self, type, FrameName):
		frame_name = FrameName.split("@")
		if type != "var":
			printErrMessage("ERROR: Wrong type used, expected 'var'.", ERROR_XML_SYNTAX)
		if frame_name[0] not in ["GF", "LF", "TF"]:
			printErrMessage("ERROR: Invalid frame type in variable.", ERROR_XML_SYNTAX)
		if not re.search(r"^([a-zA-Z]|[_\-$&%*!?])([\w_\-$&%*!?])*$", frame_name[1]):
			printErrMessage("ERROR: Invalid characters in variable.", ERROR_XML_SYNTAX)
	
	# Check if <symb> is correct
	def checkSymb(self, type, value):
		if type == "int" and value != None:
			if not re.search(r"^[+-]?[0-9]*$", value):
				printErrMessage("ERROR: Invalid int value.", ERROR_XML_SYNTAX)
		elif type == "bool" and value != None:
			if value not in ["true", "false"]:
				printErrMessage("ERROR: Invalid bool value.", ERROR_XML_SYNTAX)
		elif type == "string" and value != None:
			if re.search(r"(?!\\[0-9]{3})[\s\\#]", value):
				printErrMessage("ERROR: Invalid string value.", ERROR_XML_SYNTAX)
		elif type == "nil" and value != None:
			if value != "nil":
				printErrMessage("ERROR: Invalid nil value.", ERROR_XML_SYNTAX)
		elif type == "var":
			self.checkVar(type, value)

	# Convert escape sequences
	def convertEscape(self, value):
		escape = re.findall(r"\\([0-9]{3})", value)
		for esc in escape:
			value = re.sub("\\\\{0}".format(esc), chr(int(esc)), value)
		return value

	# Add variable to frame
	def addToFrame(self, frame, variable):
		if frame == "GF":
			if variable.name in self.global_frame:
				printErrMessage("ERROR: Variable '{0}' already exists in GF.".format(variable.name), ERROR_SEMANTIC)
			self.global_frame[variable.name] = variable
		elif frame == "LF":
			if self.local_frame == None:
				printErrMessage("ERROR: Local frame is not initialized.", ERROR_FRAME)
			if variable.name in self.local_frame:
				printErrMessage("ERROR: Variable '{0}' already exists in LF.".format(variable.name), ERROR_SEMANTIC)
			self.local_frame[variable.name] = variable
		elif frame == "TF":
			if self.temp_frame == None:
				printErrMessage("ERROR: Temporary frame is not initialized.", ERROR_FRAME)
			self.temp_frame[variable.name] = variable

	# Get variable from frame
	def getFromFrame(self, frame, name):
		if frame == "GF":
			if name not in self.global_frame:
				printErrMessage("ERROR: Variable '{0}' does not exist in GF.".format(name), ERROR_VARIABLE)
			else:
				return self.global_frame[name]
		elif frame == "LF":
			if self.local_frame == None:
				printErrMessage("ERROR: Temp frame does not exist.", ERROR_FRAME)
			if name not in self.local_frame:
				printErrMessage("ERROR: Variable '{0}' does not exist in LF.".format(name), ERROR_VARIABLE)
			else:
				return self.local_frame[name]
		elif frame == "TF":
			if self.temp_frame == None:
				printErrMessage("ERROR: Temp frame does not exist.", ERROR_FRAME)
			if name not in self.temp_frame:
				printErrMessage("ERROR: Variable '{0}' does not exist in TF.".format(name), ERROR_VARIABLE)
			else:
				return self.temp_frame[name]

	# Create variable and add to frame
	def def_var(self, variable):
		frame, name = variable.split("@")
		new_var = Variable(name)
		self.addToFrame(frame, new_var)

	# MOVE, inits the variable in dict
	def move_var(self, variable, symb):
		frame, name = variable.split("@")
		var = self.getFromFrame(frame, name)
		var.var_type = symb[1]
		if symb[1] == "string" and symb[2] != None:
			conv_str = self.convertEscape(symb[2])
			var.value = conv_str
		elif symb[2] == None:
			var.value = ""
		elif symb[1] == "var":
			frame1, name1 = symb[2].split("@")
			var1 = self.getFromFrame(frame1, name1)
			if var1.var_type != None:
				self.setValue(frame, name, var1.var_type, var1.value)
			else:
				printErrMessage("ERROR: Variable is not initialized.", ERROR_MISS_VALUE)
		elif symb[1] not in["var", "bool", "int", "string", "nil"]:
			printErrMessage("ERROR: Bad symb type.", ERROR_XML_SYNTAX)
		else:
			var.value = symb[2]

	# Instruction PUSHFRAME
	def pushFrame(self):
		if self.temp_frame is not None:
			self.frame_stack.append(self.temp_frame.copy())
			self.local_frame = self.temp_frame
			self.temp_frame = None
		else:
			printErrMessage("ERROR: Temporary frame is not defined.", ERROR_FRAME)

	# Instruction POPFRAME
	def popFrame(self):
		if self.local_frame != None:
			self.temp_frame = self.frame_stack.pop()
			try:
				self.local_frame = self.frame_stack[-1]
			except IndexError:
				self.local_frame = None
		else:
			printErrMessage("ERROR: Local frame does not exists.", ERROR_FRAME)

	# Instruction PUSHS
	def pushs(self, types, value):
		if types not in["var", "int", "string", "nil", "bool"]:
			printErrMessage("ERROR: Bad type used.", ERROR_XML_SYNTAX)
		if types == "var":
			frame, name = value.split("@")
			var = self.getFromFrame(frame, name)
			if var.value == None:
				printErrMessage("ERROR: Variable does not contains value.", ERROR_MISS_VALUE)
			self.data_stack.append([var.var_type, var.value])
		elif types == "nil":
			printErrMessage("ERROR: Can not push nil.", ERROR_MISS_VALUE)
		else:
			self.data_stack.append([types, value])

	# Instruction POPS
	def pops(self, variable):
		frame, name = variable.split("@")
		var = self.getFromFrame(frame, name)
		if len(self.data_stack) == 0:
			printErrMessage("ERROR: Frame stack is empty.", ERROR_MISS_VALUE)
		value = self.data_stack.pop()
		try:
			if value[1] == None:
				val = ""
			else:
				val = int(value[1])
		except ValueError:
			val = self.convertEscape(value[1])
		self.setValue(frame, name, value[0], val)
			
	# Instruction WRITE
	def write(self, opcode, types, variable):
		if types == "var":
			frame, name = variable.split("@")
			var = self.getFromFrame(frame,name)
			if var.value == None:
				printErrMessage("ERROR: Variable is not initialized.", ERROR_MISS_VALUE)
			if opcode == "DPRINT":
				print(var.value, end='', file=sys.stderr)
			else:
				print(var.value, end='')
		elif types == "string":
			conv_string = self.convertEscape(variable)
			if opcode == "DPRINT":
				print(conv_string, end='', file=sys.stderr)
			else:
				print(conv_string, end='')
		elif types == "nil":
			if opcode == "DPRINT":
				print("", end='', file=sys.stderr)
			else:
				print("", end='')
		else:
			if opcode == "DPRINT":
				print(variable, end='', file=sys.stderr)
			else:
				print(variable, end='')

	# Set value of var
	def setValue(self, frame, variable, types, result):
		var = self.getFromFrame(frame, variable)
		var.var_type = types
		var.value = result
		
	# Instructions ADD, MUL, SUB, IDIV 
	def arithmeticOper(self, opcode, variable, arg1, arg2):
		frame, var = variable[2].split("@")
		if arg1[1] not in ["int", "var"] or arg2[1] not in ["int", "var"]:
				printErrMessage("ERROR: Invalid type used for instruction {0}.".format(opcode), ERROR_OPERAND)
		if arg1[1] == "var":
			frame1, name1 = arg1[2].split("@")
			var1 = self.getFromFrame(frame1, name1)
			value1 = var1.value
			if var1.var_type != "int":
				printErrMessage("ERROR: Type of var must be int.", ERROR_OPERAND)
		if arg2[1] == "var":
			frame2, name2 = arg2[2].split("@")
			var2 = self.getFromFrame(frame2, name2)
			value2 = var2.value
			if var2.var_type != "int":
				printErrMessage("ERROR: Type of var must be int.", ERROR_OPERAND)
		if arg1[1] == "int":
			value1 = int(arg1[2])
		if arg2[1] == "int":
			value2 = int(arg2[2])
		if opcode == "ADD":
			result = int(value1) + int(value2)
		elif opcode == "SUB":
			result = int(value1) - int(value2)
		elif opcode == "MUL":
			result = int(value1) * int(value2)
		elif opcode == "IDIV":
			if int(value2) == 0:
				printErrMessage("ERROR: Divide by zero.", ERROR_BAD_VALUE)
			result = int(value1) // int(value2)
		self.setValue(frame, var, "int", result)

	# Instructions AND, OR
	def andOrInstruct(self, variable, opcode, arg1, arg2):
		frame, name = variable[2].split("@")
		if arg1[1] == "var" or arg1[1] == "bool":
			if arg1[1] == "var":
				frame1, name1 = arg1[2].split("@")
				var1 = self.getFromFrame(frame1, name1)
				if var1.var_type != "bool":
					printErrMessage("ERROR: Type must be bool.", ERROR_OPERAND)
				value1 = var1.value
			elif arg1[1] == "bool":
				value1 = arg1[2]
			else:
				printErrMessage("ERROR: Type must be bool.", ERROR_OPERAND)
		if arg2[1] == "var" or arg2[1] == "bool":
			if arg2[1] == "var":
				frame2, name2 = arg2[2].split("@")
				var2 = self.getFromFrame(frame2, name2)
				if var2.var_type != "bool":
					printErrMessage("ERROR: Type must be bool.", ERROR_OPERAND)
				value2 = var2.value	
			elif arg2[1] == "bool":
				value2 = arg2[2]
			else:
				printErrMessage("ERROR: Type must be bool.", ERROR_OPERAND)
		if opcode == "AND":
			if value1 == "true" and value2 == "true":
				self.setValue(frame, name, "bool", "true")
			else:
				self.setValue(frame, name, "bool", "false")
		if opcode == "OR":
			if value1 == "true" or value2 == "true":
				self.setValue(frame, name, "bool", "true")
			else:
				self.setValue(frame, name, "bool", "false")

	# Instructions LT, GT, EQ
	def comparisonInstruct(self, opcode, variable, arg1, arg2):
		frame, name = variable[2].split("@")
		if arg1[1] == "var":
			frame1, name1 = arg1[2].split("@")
			var1 = self.getFromFrame(frame1, name1)
			type1 = var1.var_type
			value1 = var1.value
		else:
			type1 = arg1[1]
			value1 = arg1[2]
		if arg2[1] == "var":
			frame2, name2 = arg2[2].split("@")
			var2 = self.getFromFrame(frame2, name2)
			type2 = var2.var_type
			value2 = var2.value
		else:
			type2 = arg2[1]
			value2 = arg2[2]
		if opcode == "LT":
			if type1 == "int" and type2 == "int":
				result = int(value1) < int(value2)
			elif type1 == "string" and type2 == "string":
				result = value1 < value2
			elif type1 == "bool" and type2 == "bool":
				if value1 == value2:
					result = False
				elif value1 == "true" and value2 == "false":
					result = False
				else:
					result = True
			else:
				printErrMessage("ERROR: <symb> must be the same type.", ERROR_OPERAND)
		elif opcode == "GT":
			if type1 == "int" and type2 == "int":
				result = int(value1) > int(value2)
			elif type1 == "string" and type2 == "string":
				result = value1 > value2
			elif type1 == "bool" and type2 == "bool":
				if value1 == value2:
					result = False
				elif value1 == "true" and value2 == "false":
					result = True
				else:
					result = False
			else:
				printErrMessage("ERROR: <symb> must be the same type.", ERROR_OPERAND)
		elif opcode == "EQ":
			if type1 == "int" and type2 == "int":
				result = int(value1) == int(value2)
			elif type1 == "string" and type2 == "string":
				result = value1 == value2
			elif type1 == "bool" and type2 == "bool":
				if value1 == value2:
					result = True
				else:
					result = False
			elif type1 == "nil" or type2 == "nil":
				if value1 == "nil" and value2 == "nil":
					result = True
				else:
					result = False
			else:
				print(type1)
				print(type2)
				printErrMessage("ERROR: <symb> must be the same type.", ERROR_OPERAND)
		if result == True:
			self.setValue(frame, name, "bool", "true")
		elif result == False:
			self.setValue(frame, name, "bool", "false")

	# Instruction STRI2INT
	def stri2int(self, variable, opcode, arg1, arg2):
		frame, name = variable[2].split("@")
		if arg1[1] != "var" and arg1[1] != "string":
			printErrMessage("ERROR: Type of <symb1> must be string.", ERROR_OPERAND)
		else:
			if arg1[1] == "var":
				frame1, name1 = arg1[2].split("@")
				var1 = self.getFromFrame(frame1, name1)
				if var1.var_type != "string":
					printErrMessage("ERROR: Type of <symb1> must be string.", ERROR_OPERAND)
				ord_string = var1.value
			else:
				ord_string = arg1[2]
		if arg2[1] != "var" and arg2[1] != "int":
			printErrMessage("ERROR: Type of <symb2> must be integer.", ERROR_OPERAND)
		else:
			if arg2[1] == "var":
				frame2, name2 = arg2[2].split("@") 
				var2 = self.getFromFrame(frame2, name2)
				if var2.var_type != "int":
					printErrMessage("ERROR: Type of <symb2> must be integer.", ERROR_OPERAND)
				position = var2.value
			else:
				position = arg2[2]
		if int(position) >= len(ord_string):
			printErrMessage("ERROR: Index outside the string.", ERROR_STRING_WORK)
		letter = ord_string[int(position)]
		self.setValue(frame, name, "int", ord(letter))

	# Instruction CONCAT
	def concat(self, variable, opcode, arg1, arg2):
		frame, name = variable[2].split("@")
		if arg1[1] == "var" or arg1[1] == "string":
			if arg1[1] == "var":
				frame1, name1 = arg1[2].split("@")
				var1 = self.getFromFrame(frame1, name1)
				if var1.var_type == None:
					printErrMessage("ERROR: <symb1> must be string.", ERROR_MISS_VALUE)
				if var1.var_type != "string":
					printErrMessage("ERROR: <symb1> must be string.", ERROR_OPERAND)
				string1 = var1.value
			else:
				string1 = arg1[2]
		else:
			printErrMessage("ERROR: <symb1> must be string.", ERROR_OPERAND)
		if arg2[1] == "var" or arg2[1] == "string":
			if arg2[1] == "var":
				frame1, name1 = arg2[2].split("@")
				var1 = self.getFromFrame(frame1, name1)
				if var1.var_type == None:
					printErrMessage("ERROR: <symb1> must be string.", ERROR_MISS_VALUE)
				if var1.var_type != "string":
					printErrMessage("ERROR: <symb2> must be string.", ERROR_OPERAND)
				string2 = var1.value
			else:
				string2 = arg2[2]
		else:
			printErrMessage("ERROR: <symb2> must be string.", ERROR_OPERAND)
		self.setValue(frame, name, "string", string1+string2)

	# Instruction SETCHAR
	def setchar(self, opcode, variable, arg1, arg2):
		frame, name = variable[2].split("@")
		var = self.getFromFrame(frame, name)
		if var.var_type != "string":
			printErrMessage("ERROR: Type of var must be string.", ERROR_OPERAND)
		if arg1[1] == "var" or arg1[1] == "int":
			if arg1[1] == "var":
				frame1, name1 = arg1[2].split("@")
				var1 = self.getFromFrame(frame1, name1)
				if var1.var_type != "int":
					printErrMessage("ERROR: Type of <symb1> must be integer.", ERROR_OPERAND)
				position = int(var1.value)
				if position >= len(var.value):
					printErrMessage("ERROR: Index outside the string.", ERROR_STRING_WORK)
			else:
				position = int(arg1[2])
				if position >= len(var.value):
					printErrMessage("ERROR: Index outside the string.", ERROR_STRING_WORK)
		else:
			printErrMessage("ERROR: Type of <symb1> must be integer.", ERROR_OPERAND)
		if arg2[1] == "var" or arg2[1] == "string":
			if arg2[1] == "var":
				frame2, name2 = arg2[2].split("@")
				var2 = self.getFromFrame(frame2, name2)
				if var2.var_type != "string":
					printErrMessage("ERROR: Type of <symb1> must be string.", ERROR_OPERAND)
				if len(var2.value) == 0:
					printErrMessage("ERROR: String is empty.", ERROR_STRING_WORK)
				c = var2.value[0]
			else:
				if arg2[2] == None:
					printErrMessage("ERROR: String is empty.", ERROR_STRING_WORK)
				c = arg2[2][0]
		else:
			printErrMessage("ERROR: Type of <symb2> must be string.", ERROR_OPERAND)
		s = list(var.value)
		s[position] = c
		modified_s = "".join(s)
		self.setValue(frame, name, "string", modified_s)

	# Instruction GETCHAR
	def getchar(self, opcode, variable, arg1, arg2):
		frame, name = variable[2].split("@")
		if arg1[1] == "var" or arg1[1] == "string":
			if arg1[1] == "var":
				frame1, name1 = arg1[2].split("@")
				var1 = self.getFromFrame(frame1, name1)
				if var1.var_type == None:
					printErrMessage("ERROR: <symb2> is not initialized.", ERROR_MISS_VALUE)
				if var1.var_type != "string":
					printErrMessage("ERROR: Type of <symb1> must be string.", ERROR_OPERAND)
				string = var1.value
			else:
				if arg1[2] == None:
					printErrMessage("ERROR: Missing value for <symb1>.", ERROR_STRING_WORK)
				string = arg1[2]
		else:
			printErrMessage("ERROR: Type of <symb1> must be string.", ERROR_OPERAND)
		if arg2[1] == "var" or arg2[1] == "int":
			if arg2[1] == "var":
				frame2, name2 = arg2[2].split("@")
				var2 = self.getFromFrame(frame2, name2)
				if var2.var_type == None:
					printErrMessage("ERROR: <symb2> is not initialized.", ERROR_MISS_VALUE)
				if var2.var_type != "int":
					printErrMessage("ERROR: Type of <symb2> must be integer.", ERROR_OPERAND)
				position = int(var2.value)
			else:
				if arg2[2] == None:
					printErrMessage("ERROR: Missing value for <symb2>.", ERROR_OPERAND)
				position = int(arg2[2])
		else:
			printErrMessage("ERROR: Type of <symb2> must be integer.", ERROR_OPERAND)
		if position >= len(string):
			printErrMessage("ERROR: Index outside the string.", ERROR_STRING_WORK)
		c = string[position]
		self.setValue(frame, name, "string", c)

	# Instruction BREAK
	def breakInstr(self):
		print("---------------DEBUG----------------", file=sys.stderr)
		print("Number of executed instructions: {0}".format(self.total_instr), file=sys.stderr)
		print("Content of global frame:", file=sys.stderr)
		for var in self.global_frame:
			variable = self.getFromFrame("GF", var)
			print("Name:{0} Type:{1} Value:{2}".format(variable.name, variable.var_type, variable.value), file=sys.stderr)
		print("Content of local frame:", file=sys.stderr)
		if self.local_frame is None or self.local_frame == {}:
			print("Local frame is not initialized.", file=sys.stderr)
		else:
			for var in self.local_frame:
				variable = self.getFromFrame("LF", var)
				print("Name:{0} Type:{1} Value:{2}".format(variable.name, variable.var_type, variable.value), file=sys.stderr)
		print("Content of temp frame:", file=sys.stderr)
		if self.temp_frame == None:
			print("Temp frame is not initialized.", file=sys.stderr)
		else:
			for var in self.temp_frame:
				variable = self.getFromFrame("TF", var)
				print("Name:{0} Type:{1} Value:{2}".format(variable.name, variable.var_type, variable.value), file=sys.stderr)

# Function to sort instructions by order
def sortchildrenby(parent, attr):
    parent[:] = sorted(parent, key=lambda child: int(child.get('order')))

def main():
	# Argument parsing
	input_list = []
	try:
		opts, args = getopt.getopt(sys.argv[1:], ["help", "source=", "input="], ["help", "source=", "input="])
	except getopt.GetoptError:
		printErrMessage("ERROR: Wrong arguments used, use --help for more info.", PARAM_ERROR)
	if len(sys.argv) == 2:
		for option, fileName in opts:
			if option == "--help":
				printHelp()
			elif option == "--source":
				try:
					source_file = open(fileName, "r")
				except IOError:
					printErrMessage("ERROR: Cannot open file in required mode.", FILE_IN_ERR)
			elif option == "--input":
				try:
					input_file = open(fileName, "r")
					input_list = input_file.read().splitlines()
				except IOError:
					printErrMessage("ERROR: Cannot open file in required mode.", FILE_IN_ERR)

	elif len(sys.argv) == 3:
		for option, fileName in opts:
			# Check if filename is writed after source= without whitespaces
			try:
				fileName
			except Exception:
				printErrMessage("ERROR: Wrong number of arguments, use --help for more info.", PARAM_ERROR)
			if option == "--help":
				printErrMessage("ERROR: Help can not be combinated with other arguments.", PARAM_ERROR)
			elif option == "--source":
				try:
					source_file = open(fileName, "r")
				except IOError:
					printErrMessage("ERROR: Cannot open file in required mode.", FILE_IN_ERR)
				# Check if source file is not empty
				if os.stat(fileName).st_size == 0:
					printErrMessage("ERROR: Source file is empty.", ERROR_XML_FORMAT)
			elif option == "--input":
				try:
					input_file = open(fileName, "r")
					input_list = input_file.read().splitlines()
				except IOError:
					printErrMessage("ERROR: Cannot open file in required mode.", FILE_IN_ERR)
	else:
		printErrMessage("ERROR: Wrong number of arguments, use --help for more info.", PARAM_ERROR)

	# Check root node of source file
	try:
		if len(sys.argv) == 2:
			if "--source" in opts[0]:
				tree = ET.parse(opts[0][1])
			else:
				tree = ET.parse(sys.stdin)
		elif len(sys.argv) == 3:
			if "--source" in opts[0]:
				tree = ET.parse(opts[0][1])
			elif "--souce" in opts[1]:
				tree = ET.parse(opts[0][1])
		root = tree.getroot()
	except Exception:
		printErrMessage("ERROR: Invalid XML format.", ERROR_XML_FORMAT)

	sortchildrenby(root, 'order')

	# Check file header
	if root.tag != "program":
		printErrMessage("ERROR: Invalid XML header.", ERROR_XML_FORMAT)
	if "language" not in root.attrib:
		printErrMessage("ERROR: Invalid XML structure, missing or invalid language", ERROR_XML_FORMAT)
	if root.attrib["language"] != "IPPcode19":
		printErrMessage("ERROR: Missing or invalid language.", ERROR_XML_SYNTAX)

	# Delete optional attributes "name" or "description"
	if len(root.attrib) == 2 or len(root.attrib) == 3:
		if "name" in root.attrib:
			del root.attrib["name"]
		if "description" in root.attrib:
			del root.attrib["description"]
	if len(root.attrib) != 1:
		printErrMessage("ERROR: Invalid XML header.", ERROR_XML_SYNTAX)	

	interpret = Interpret()

	# Cycle for add labels to list
	instr_order = 1
	for child in root:
		if int(child.attrib['order']) != instr_order:
			printErrMessage("ERROR: Missing instruction order number.", ERROR_XML_SYNTAX)
		instr_order+=1
		if child.attrib['opcode'] == "LABEL":
			for arg in child:
				if arg.text in labels_list:
					printErrMessage("ERROR: Label already exists.", ERROR_SEMANTIC)
				order = child.attrib['order']
				labels_list[arg.text] = order	

	# Check if there is unwanted text
	for line in source_file:
		line = line.rstrip().lstrip()
		if line[:1] != "<":
			if line[:1] != "":
				printErrMessage("ERROR: XML contains unwanted text.", ERROR_XML_SYNTAX)
		if line[:1] != "":
			if line[-1] != ">":
				printErrMessage("ERROR: XML contains unwanted text.", ERROR_XML_SYNTAX)

	# Check if arg element does not contain other element
	for child in root:
		for arg in child:
			for nieco in arg:
				if nieco.tag:
					printErrMessage("ERROR: XML contains unwanted elements.", ERROR_XML_SYNTAX)

	i = 0
	while i < len(root):
		interpret.checkInstruct(root[i])
		args_list = interpret.checkArgs(root[i])
		i = interpret.AddInstr(root[i].attrib['opcode'], args_list, labels_list, input_list, i, opts) + 1

if __name__ == "__main__":
	main()
