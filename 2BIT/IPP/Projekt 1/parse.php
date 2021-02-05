<?php
/*
**  Author: Adrian Boros
**  File: parse.php
**	Date: 09.03.2019
*/

// Error constants
const PARAM_ERR = 10;
const FILE_IN_ERR = 11;
const FILE_OUT_ERR = 12;
const HEAD_ERR = 21;
const OPCODE_ERR = 22;
const LEX_ERR = 23;

$shortopts = "";
$longopts = array("help", "stats:", "loc", "comments", "labels", "jumps");
$options = getopt($shortopts, $longopts);
$comment_count = 0;
$order = 0;
$jump_counter = 0;
$label_array= "";
$labels = "";

$passed_arg = array_unique($argv);

// Check if there is not allowed params 
if (count($options) != count($passed_arg)-1) {
	fwrite(STDERR, "ERROR: Bad parameters used!\n");
  	exit(PARAM_ERR);
}

if (isset($options["stats"])) {
  $output_file = $options["stats"];
}
if (in_array($output_file, array('--loc', '--labels', '--jumps', '--comments', '--stats'))) {
	fwrite(STDERR, "ERROR: Missing or bad parameters for stats!\n");
  	exit(PARAM_ERR);
}
if (isset($options["stats"]) and isset($options["help"])) {
  fwrite(STDERR, "ERROR: Help can not be combinated with other arguments.\n");
  exit(PARAM_ERR);
}

// Set variables for STATP output
$writeCom = false;
$writeLoc = false;
$writeJump = false;
$writeLabel = false;
foreach ($options as $key => $option) {
  if ($key == "comments") {
    $writeCom = true;
  }
  elseif ($key == "loc") {
    $writeLoc = true;
  }
  elseif ($key == "jumps") {
    $writeJump = true;
  }
  elseif ($key == "labels") {
    $writeLabel = true;
  }
}
if (isset($output_file) and !$writeCom and !$writeLoc and !$writeJump and !$writeLabel) {
  fwrite(STDERR, "ERROR: Missing or bad parameters for stats!\n");
  exit(PARAM_ERR);
}

// Check first line of input
if(($argc == 1 && empty($options)) or (!empty($options) and isset($options["stats"]))) {
  $first_line = fgets(STDIN);
  $comment = strrpos($first_line, "#");
  if($comment) {
    $comment_count++;
  }
  if(!$first_line) {
    fwrite(STDERR, "ERROR: Missing input.\n");
    exit(HEAD_ERR);
  }
  $first_line = strtoupper(trim(preg_replace("~#.*~", ' ', $first_line))); // Remove newline and commentary (#)
  if($first_line !== ".IPPCODE19"){
    fwrite(STDERR, "ERROR: Missing or invalid header IPPcode19.\n");
    exit(HEAD_ERR);
  }
}
elseif ($argc == 2 && isset($options["help"])) {
  printHelp();
}
else {
  fwrite(STDERR, "ERROR: Bad parameters, use --help for more info.\n");
  exit(PARAM_ERR);
}

// Set XMLWriter
$xmlWriter = new XMLWriter();
$xmlWriter->openMemory();
$xmlWriter->setIndent(4);
$xmlWriter->setIndentString('  ');
$xmlWriter->startDocument('1.0','UTF-8');
$xmlWriter->startElement('program');
$xmlWriter->startAttribute('language');
$xmlWriter->text('IPPcode19');
$xmlWriter->endAttribute();

while($input_line = fgets(STDIN)) {
  $input_array = preg_split("~\s+~",trim(preg_replace('/\s\s+/', ' ', $input_line))); // Remove spaces, tabs, newline 
  $array_length = 0;
  for($i = 0; $i < count($input_array); $i++) {
    $first_char = substr($input_array[$i], 0,1);
    if($input_array[$i] == "") {  // Whole line is a comment
      $comment_count++;
      break;
    }
    elseif (strrpos($input_array[$i], "#")) {
      $input_array[$i] = preg_replace('~#.*~', '', $input_array[$i]); // Replace everything after # - it is a comment
      $array_length = $i + 1;
      $comment_count++;
      break;
    }
    elseif ($first_char == "#") {
      $array_length = $i;
      $comment_count++;
      break;
    }
    else {
      $array_length++;	// New lenght of array
    }
  }
  if($array_length == 0) {
    $input_array = (array) null;
  }
  else {
    $input_array = array_slice($input_array, 0, $array_length);	// Array truncation
  }
  if(isset($input_array[0])) {	// Check opcode
    $opcode = strtoupper($input_array[0]);
    $xmlWriter->startElement('instruction');
    $xmlWriter->startAttribute('order');
    $xmlWriter->text(++$order);
    $xmlWriter->startAttribute('opcode');
    switch ($opcode) {
      // No argument
      case 'CREATEFRAME':
      case 'PUSHFRAME':
      case 'POPFRAME':
      case 'RETURN':
      case 'BREAK':
        $xmlWriter->text($opcode);
        checkArgNum($input_array, 0, $opcode);
        break;
      // One argument = <var>
      case 'DEFVAR':
      case 'POPS':
        $xmlWriter->text($opcode);
        checkArgNum($input_array, 1, $opcode);
        addOneArg($input_array[1], "var");
        isVarValid($input_array[1]);
        break;
      // One argument = <label>
      case 'CALL':
      case 'LABEL':
      case 'JUMP':
        $xmlWriter->text($opcode);
        checkArgNum($input_array, 1, $opcode);
        addOneArg($input_array[1], "label");
        isLabelValid($input_array[1]);
        break;
      // One argument = <symb>
      case 'PUSHS':
      case 'WRITE':
      case 'EXIT':
      case 'DPRINT':
        $xmlWriter->text($opcode);
        checkArgNum($input_array, 1, $opcode);
        $symb_type = explode("@", $input_array[1], 2);
        if (in_array($symb_type[0], array('LF', 'GF', 'TF'))) {
          isVarValid($input_array[1]);
          addOneArg($input_array[1], 'var');
        }
        else {
          checkType($symb_type[0], $symb_type[1]);
          addOneArg($symb_type[1], $symb_type[0]);
        }
        break;
      // Two arguments = <var> <symb>
      case 'MOVE':
      case 'INT2CHAR':
      case 'STRLEN':
      case 'TYPE':
      case 'NOT':
        $xmlWriter->text($opcode);
        checkArgNum($input_array, 2, $opcode);
        isVarValid($input_array[1]);
        $symb_type = explode("@", $input_array[2], 2);
        if (in_array($symb_type[0], array('LF', 'GF', 'TF'))) {
          isVarValid($input_array[2]);
          addTwoArg('var', $input_array[1], 'var', $input_array[2]);
        }
        else {
          checkType($symb_type[0], $symb_type[1]);
          addTwoArg('var', $input_array[1], $symb_type[0], $symb_type[1]);
        }
        break;
      // Two arguments = <var> <type>
      case 'READ':
        $xmlWriter->text($opcode);
        isVarValid($input_array[1]);
        checkArgNum($input_array, 2, $opcode);
        if (in_array($input_array[2], array('int', 'bool', 'string', 'nil'))) {
          addTwoArg('var', $input_array[1], 'type', $input_array[2]);
        }
        else {
          fwrite(STDERR, "ERROR: Argument type is not valid\n");
          exit(LEX_ERR);
        }
        break;
      // Three arguments = <var> <symb1> <symb2>
      case 'ADD':
      case 'SUB':
      case 'MUL':
      case 'IDIV':
      case 'LT':
      case 'GT':
      case 'EQ':
      case 'AND':
      case 'OR':
      case 'STRI2INT':
      case 'CONCAT':
      case 'GETCHAR':
      case 'SETCHAR':
        $xmlWriter->text($opcode);
        checkArgNum($input_array, 3, $opcode);
        isVarValid($input_array[1]);
        $symb_type1 = explode("@", $input_array[2]);
        $symb_type2 = explode("@", $input_array[3]);
        check3ArgInstr($symb_type1, $input_array, $symb_type2, 'var', 'var');
        break;
      // Three arguments = <label> <symb1> <symb2>
      case 'JUMPIFEQ':
      case 'JUMPIFNEQ':
        $xmlWriter->text($opcode);
        checkArgNum($input_array, 3, $opcode);
        isLabelValid($input_array[1]);
        $symb_type1 = explode("@", $input_array[2]);
        $symb_type2 = explode("@", $input_array[3]);
        check3ArgInstr($symb_type1, $input_array, $symb_type2, 'label', 'var');
        break;
      default:
        fwrite(STDERR, "ERROR: Invalid instruction\n");
        exit(OPCODE_ERR);
        break;
    }
    // STATP - counter for jumps
    if (in_array($opcode, array('JUMPIFEQ', 'JUMPIFNEQ', 'JUMP'))) {
      $jump_counter++;
    }
    // STATP - put labels to array
    if ($opcode === "LABEL") {
      $labels = $labels. " " . $input_array[1];
    }
    $xmlWriter->endElement();
  }
}
// Remove whitespace from start of array
$labels = ltrim($labels);
$label_array = explode(" ", $labels);
$label_array = array_unique($label_array, SORT_LOCALE_STRING); // Remove duplicates from array with labels

$xmlWriter->endElement();
$xmlWriter->endDocument();
echo $xmlWriter->outputMemory(TRUE);

// Set the output file for STATP
if (isset($options["stats"])) {
  $output_file = $options["stats"];
}

// STATP - check what user want to write to file
$output = '';
foreach ($argv as $key => $option) {
  if ($option == "--comments") {
    $output .= $comment_count . "\n";
  }
  elseif ($option == "--loc") {
    $output .= $order . "\n";
  }
  elseif ($option == "--jumps") {
    $output .= $jump_counter . "\n";
  }
  elseif ($option == "--labels") {
  	if ($label_array == "") {
  		$output .= 0 . "\n";
  	}
  	else {
  		$output .= count($label_array) . "\n";
  	}
  }
}
if (isset($output_file)) {
  $myfile = fopen($output_file, "w");
  if (!$myfile) {
    fwrite(STDERR, "ERROR: Failed to open file $myfile\n");
    exit(FILE_OUT_ERR);
  }
  $output = trim(preg_replace('/\s\s+/', ' ', $output));
  $written_stats = fwrite($myfile, $output);
  if (!$written_stats) {
  	fwrite(STDERR, "ERROR: Failed to open file $myfile\n");
    exit(FILE_OUT_ERR);
  }
  fclose($myfile);
}

// Function to add one argument
function addOneArg($input, $arg1) {
  global $xmlWriter;
  $xmlWriter->startElement('arg1');
  $xmlWriter->startAttribute('type');
  $xmlWriter->text($arg1);
  $xmlWriter->endAttribute();
  if ($input == 'nil')
    $xmlWriter->text('');
  else
    $xmlWriter->text($input);
  $xmlWriter->endElement();
}

// Function to add two arguments
function addTwoArg($arg1_type, $arg1, $arg2_type , $arg2){
  global $xmlWriter;
  $xmlWriter->startElement('arg1');
  $xmlWriter->startAttribute('type');
  $xmlWriter->text($arg1_type);
  $xmlWriter->endAttribute();
  $xmlWriter->text($arg1);
  $xmlWriter->endElement();
  $xmlWriter->startElement('arg2');
  $xmlWriter->startAttribute('type');
  $xmlWriter->text($arg2_type);
  $xmlWriter->endAttribute();
  $xmlWriter->text($arg2);
  $xmlWriter->endElement();
}

// Function to add three arguments
function addThreeArg($arg1_type, $arg1, $arg2_type , $arg2, $arg3_type, $arg3) {
  global $xmlWriter;
  $xmlWriter->startElement('arg1');
  $xmlWriter->startAttribute('type');
  $xmlWriter->text($arg1_type);
  $xmlWriter->endAttribute();
  $xmlWriter->text($arg1);
  $xmlWriter->endElement();
  $xmlWriter->startElement('arg2');
  $xmlWriter->startAttribute('type');
  $xmlWriter->text($arg2_type);
  $xmlWriter->endAttribute();
  $xmlWriter->text($arg2);
  $xmlWriter->endElement();
  $xmlWriter->startElement('arg3');
  $xmlWriter->startAttribute('type');
  $xmlWriter->text($arg3_type);
  $xmlWriter->endAttribute();
  $xmlWriter->text($arg3);
  $xmlWriter->endElement();
}

// Function to check if identificator is valid
function isVarValid($value) {
  if (!preg_match('/^(GF|LF|TF)@([[:alpha:]]|[\_\-\$\&\%\*\!\?])([\w]|[\_\-\$\&\%\*\!\?])*$/', $value)) {
  	// Var must start with GF/LF/TF after must be @, after must be letters a-Z or special chars (_-$&%*!?)
  	// and after alphanumerical char or special chars
    fwrite(STDERR, "ERROR: Argument is not valid\n");
    exit(LEX_ERR);
  }
}

// Function to check if integer is valid
function isIntValid($value) {
  if (!preg_match('/^[+-]?[[:digit:]]*$/', $value)) {
  	// + or - are optional 
    fwrite(STDERR, "ERROR: Int constant is not valid\n");
    exit(LEX_ERR);
  }
}

// Function to check if string is valid
function isStringValid($value) {
  if (preg_match('/(?!\\\\[0-9]{3})[\#\s\\\\]/', $value)) {
  	// There can not be #, \ or whitespace, there can be escape sequence from \000 to \999
    fwrite(STDERR, "ERROR: String constant is not valid\n");
    exit(LEX_ERR);
  }
}

// Function to check if bool is valid
function isBoolValid($value) {
  if (!preg_match('/^(true|false)$/', $value)) {
  	// There can be just true or false, it is case sensitive
    fwrite(STDERR, "ERROR: Bool constant is not valid\n");
    exit(LEX_ERR);
  }
}

// Function to check if label is valid
function isLabelValid($value) {
  if (!preg_match('/^([[:alpha:]]|[\_\-\$\&\%\*\!\?])([\w]|[\_\-\$\&\%\*\!\?])*$/', $value)) {
  	// First char must be [a-Z] or special char from array(_-$&%*!?) and after can be the same as first + numbers
    fwrite(STDERR, "ERROR: Argument is not valid\n");
    exit(LEX_ERR);
  }
}

// Function to check if nil is valid
function isNilValid($value) {
  if (!preg_match('/^(nil)$/', $value)) {
    fwrite(STDERR, "ERROR: Nil constant is not valid\n");
    exit(LEX_ERR);
  }
}

// Function to check type
function checkType($type, $value) {
  switch ($type) {
    case 'string':
      isStringValid($value);
      break;
    case 'int':
      isIntValid($value);
      break;
    case 'bool':
      isBoolValid($value);
      break;
    case 'nil':
      isNilValid($value);
      break;
    default:
      fwrite(STDERR, "ERROR: Type is not valid\n");
      exit(LEX_ERR);
      break;
  }
}

// Function to check number of arguments of instruction
function checkArgNum($input, $arg_num, $instruct) {
  if ((count($input)-1) != $arg_num) {
    fwrite(STDERR, "ERROR: Bad number of arguments for instruction $instruct\n");
    exit(LEX_ERR);
  }
}

// Function to check 3 arg instructions
function check3ArgInstr($symb_type1, $array, $symb_type2, $var_symb1, $var_symb2) {
  // <var> <var>
  if (in_array($symb_type1[0], array('LF', 'GF', 'TF'))) {
    isVarValid($array[2]);
    // <var> <var> <symb>
    if (in_array($symb_type2[0], array('string', 'bool', 'int', 'nil'))) {
      checkType($symb_type2[0], $symb_type2[1]);
      addThreeArg($var_symb1, $array[1], $var_symb2, $array[2], $symb_type2[0], $symb_type2[1]);
    } // <var> <var> <var>
    else {
      isVarValid($array[3]);
      addThreeArg($var_symb1, $array[1], $var_symb2, $array[2], $var_symb2, $array[3]);
    }
  } // <var> <symb>
  elseif (in_array($symb_type1[0], array('string', 'bool', 'int', 'nil'))) {
    checkType($symb_type1[0], $symb_type1[1]);
    // <var> <symb> <symb>
    if (in_array($symb_type2[0], array('string', 'bool', 'int', 'nil'))) {
      checkType($symb_type2[0], $symb_type2[1]);
      addThreeArg($var_symb1, $array[1], $symb_type1[0], $symb_type1[1], $symb_type2[0], $symb_type2[1]);
    } // <var> <symb> <var>
    else {
      isVarValid($array[3]);
      addThreeArg($var_symb1, $array[1], $symb_type1[0], $symb_type1[1], $var_symb2, $array[3]);
    }
  }
  else {
    fwrite(STDERR, "ERROR: Argument type is not valid\n");
    exit(LEX_ERR);
  }
}

// Function to print --help
function printHelp() {
  echo("-----Napoveda-----\n");
  echo("Program nacte ze standardniho vstupu zdrojovy kod v IPPcode19\n");
  echo("a zkontroluje lexikalni a syntaktickou spravnost kodu \n");
  echo("a vypise na standardni vystup XML reprezentaci.\n");
  exit(0);
}
?>
