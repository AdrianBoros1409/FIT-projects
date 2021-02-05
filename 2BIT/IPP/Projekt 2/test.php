<?php
/*
**  Author: Adrian Boros
**  File: test.php
**	Date: 11.04.2019
*/

// Error codes
const PARAM_ERR = 10;
const FILE_IN_ERR = 11;
const FILE_OUT_ERR = 12;

$shortopts = "";
$longopts = array("help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only");
$options = getopt($shortopts, $longopts);

// Global variables
$test_dir = "./";
$recursive = FALSE;
$parse_script_dir = FALSE;
$int_script_dir = FALSE;
$parse_script = "";
$int_script = "";
$parse_only = FALSE;
$int_only = FALSE;
$test_count = 0;
$passed = 0;
$failed = 0;

// Argument parsing
if (count($options) != $argc-1) {
	fwrite(STDERR, "ERROR: Bad params used.\n");
  	exit(PARAM_ERR);
}
if (count($options) == 0 and $argc != 1) {
	fwrite(STDERR, "ERROR: Bad params used.\n");
  	exit(PARAM_ERR);
}
elseif (isset($options["help"])) {
	if ($argc == 2) {
		printHelp();
	}
	else {
		fwrite(STDERR, "ERROR: Help can not be combinated with other arguments.\n");
  		exit(PARAM_ERR);
	}
}
if (isset($options["directory"])) {
	$test_dir = $options["directory"];
}
if (isset($options["recursive"])) {
	$recursive = TRUE;
}
if (isset($options["parse-script"])) {
	$parse_script_dir = TRUE;
	$parse_script = $options["parse-script"];
}
if (isset($options["int-script"])) {
	$int_script_dir = TRUE;
	$int_script = $options["int-script"];
}
if (isset($options["parse-only"])) {
	if (isset($options["int-script"])) {
		fwrite(STDERR, "ERROR: Parse-only can not be combinated with int-script.\n");
  		exit(PARAM_ERR);
	}
	if (isset($options["int-only"])) {
		fwrite(STDERR, "ERROR: Parse-only can not be combinated with int-only.\n");
  		exit(PARAM_ERR);
	}
	else {
		$parse_only = TRUE;
	}
}
if (isset($options["int-only"])) {
	if (isset($options["parse-script"])) {
		fwrite(STDERR, "ERROR: Int-only can not be combinated with parse-script.\n");
  		exit(PARAM_ERR);
	}
	if (isset($options["parse-only"])) {
		fwrite(STDERR, "ERROR: Parse-only can not be combinated with int-only.\n");
  		exit(PARAM_ERR);
	}
	else {
		$int_only = TRUE;
	}
}

# Set to current directory if --directory is not used
if ($test_dir == "./") {
	$test_dir = getcwd();
	$test_dir = $test_dir."/";
}
if ($parse_script_dir == FALSE) {
	$parse_script = getcwd();
	$parse_script = $parse_script."/parse.php";
}
if ($int_script_dir == FALSE) {
	$int_script = getcwd();
	$int_script = $int_script."/interpret.py";
}

// HTML header
echo "<!DOCTYPE HTML>";
echo "<html>";
echo "<head>";
	echo "<meta charset=\"utf-8\">";
	echo "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
	echo "<title>IPP project - test.php</title>";
echo "</head>";
echo "<body>";
echo '<body style="background-color:#e6e6e6">';

echo "<strong><div style='text-align:center'> <font size=\"5\" color=\"#1a1a1a\" face=\"Courier New\">IPP test results - Adrian Boros [xboros03@fit.vutbr.cz]</iconv(in_charset, out_charset, str)></font></strong></div>";

// HTML table
echo '<pre>';
echo '<font face="Courier New" size="2"><table style="margin:0px auto" border="1" width="1000" align="center">';
echo '<tr>';
echo '<th style="text-align:center;">ID</td>';
echo '<th style="text-align:center;">Test file name</td>';
echo '<th style="text-align:center;">Expected code</td>';
echo '<th style="text-align:center;">Return code</td>';
echo '<th style="text-align:center;">Result</td>';
echo '</tr>'; 
echo '</pre>';

getDirContents($test_dir, $recursive);

// Function which scan directories
function getDirContents($test_dir, $recursive) {
	global $test_count;
	$files = scandir($test_dir);
	foreach ($files as $key => $value) {
		$path = realpath($test_dir.DIRECTORY_SEPARATOR.$value);
		if (!is_dir($path)) {
		}
		elseif ($value != "." and $value != ".." and $recursive == TRUE) {
			getDirContents($path, $result);
		}
		$result[] = $value;
	}
	foreach ($result as $source => $value) {
		$ext = pathinfo($value, PATHINFO_EXTENSION);
		$name = basename($value, $ext);
		$name = substr($name, 0, -1);
		if ($ext == "src") {
			$test_count+=1;
			test($test_dir."/", $name);
		}
	}
	if ($test_count == 0) {
		echo '</tr>'; 
	}
}

echo "<strong><div style='text-indent:18%'> <font size=\"5\" color=\"#1a1a1a\" face=\"Courier New\">Summary: </iconv(in_charset, out_charset, str)></font></strong>";
echo "<strong><div style='text-indent:18%'> <font size=\"3\" color=\"#1a1a1a\" face=\"Courier New\">&#9673; Total tests: $test_count</iconv(in_charset, out_charset, str)></font></strong>";
echo "<strong><div style='text-indent:18%'> <font size=\"3\" color=\"#1a1a1a\" face=\"Courier New\">&#9673; Passed tests: $passed </iconv(in_charset, out_charset, str)></font></strong>";
echo "<strong><div style='text-indent:18%'> <font size=\"3\" color=\"#1a1a1a\" face=\"Courier New\">&#9673; Failed tests: $failed </iconv(in_charset, out_charset, str)></font></strong>";
if ($test_count == 0) {
	echo "<strong><div style='text-indent:18%'> <font size=\"3\" color=\"#1a1a1a\" face=\"Courier New\">&#9673; Percentage: 0 &#37;</iconv(in_charset, out_charset, str)></font></strong>";
}
else {
	$percentage = $passed/$test_count*100;
	$percentage = round($percentage, 2);
	echo "<strong><div style='text-indent:18%'> <font size=\"3\" color=\"#1a1a1a\" face=\"Courier New\">&#9673; Percentage: $percentage &#37;</iconv(in_charset, out_charset, str)></font></strong>";
}

// Function which writes test results
function writeResults($test_count, $path, $return, $return_code, $tick_cross) {
	echo '<td style="text-align:center;">' . $test_count . '</td>';
	echo '<td style="text-align:center;">' . $path.".src" . '</td>';
	echo '<td style="text-align:center;">' . $return . '</td>';
	echo '<td style="text-align:center;">' . $return_code . '</td>';
	echo '<td style="text-align:center;">'.$tick_cross.'</td>';
	echo '</tr>';
}

// Function which provides testing
function test($dir, $name) {
	$path = $dir.$name;
	if (!file_exists($path.".in")) {
		$in_file = touch($path.".in");
		if (!$in_file) {
			fwrite(STDERR, "ERROR: Creation of '.in' file was not successful.\n");
  			exit(FILE_IN_ERR);
		}
	}
	if (!file_exists($path.".out")) {
		$out_file = touch($path.".out");
		if (!$out_file) {
			fwrite(STDERR, "ERROR: Creation of '.out' file was not successful.\n");
  			exit(FILE_IN_ERR);
		}
	}
	if (!file_exists($path.".rc")) {
		$out_file = touch($path.".rc");
		if (!$out_file) {
			fwrite(STDERR, "ERROR: Creation of '.rc' file was not successful.\n");
  			exit(FILE_IN_ERR);
		}
		$fp = fopen($path.".rc", "w");
		if (!$fp) {
			fwrite(STDERR, "ERROR: File open failed.\n");
  			exit(FILE_IN_ERR);
		}
		if (fwrite($fp, 0) == FALSE) {
			fwrite(STDERR, "ERROR: Failed to write to file.\n");
  			exit(FILE_IN_ERR);
		}
		fclose($fp);
	}

	global $parse_script;
	global $int_script;
	global $int_only;
	global $parse_only;
	global $test_count;
	global $passed;
	global $failed;

	// Temp files
	touch("./tmp_parse");
	touch("./tmp_inter_out");

	if ($int_only == TRUE) {
		exec("python3.6 ".$int_script." --source=\"$path.src\" --input=\"$path.in\""." > ". "./tmp_inter_out", $output, $return);
		$return_code = exec("cat \"$path.rc\"");
		if (intval($return_code) != 0) {
			if (intval($return_code != $return)) {
				writeResults($test_count, $path, $return, $return_code, "&#10008");
				$failed+=1;
			}
			else {
				writeResults($test_count, $path, $return, $return_code,"&#10004");
				$passed+=1;
			}
		}
		if (intval($return_code == 0)) {
			if (intval($return_code) == $return) {
				exec("diff -w ./tmp_inter_out \"$path.out\"", $output, $difference);
				if (empty($output)) {
					writeResults($test_count, $path, $return, $return_code, "&#10004");
					$passed+=1;
				}
				else {
					writeResults($test_count, $path, $return, $return_code, "&#10008");
					$failed+=1;
				}
			}
			else {
				writeResults($test_count, $path, $return, $return_code, "&#10008");
				$failed+=1;
			}
		}
	}
	elseif ($parse_only == TRUE) {
		exec("php7.3 ".$parse_script." < \"$path.src\" > ./tmp_parse", $output, $return);
		$return_code = $return_code = exec("cat \"$path.rc\"");
		if (intval($return_code) != 0) {
			if (intval($return_code != $return)) {
				writeResults($test_count, $path, $return, $return_code, "&#10008");
				$failed+=1;
			}
			else {
				writeResults($test_count, $path, $return, $return_code,"&#10004");
				$passed+=1;
			}
		}
		if (intval($return_code == 0)) {
			if (intval($return_code) == $return) {
				exec("java -jar /pub/courses/ipp/jexamxml/jexamxml.jar ./tmp_parse \"$path.out\"", $output, $return);
				if ($return == 0) {
					writeResults($test_count, $path, $return, $return_code,"&#10004");
					$passed+=1;
				}
				else {
					writeResults($test_count, $path, $return, $return_code, "&#10008");
					$failed+=1;
				}
			}
			else {
				writeResults($test_count, $path, $return, $return_code, "&#10008");
				$failed+=1;
			}
		}
	}
	else {
		exec("php7.3 ".$parse_script." < \"$path.src\" > ./tmp_parse");
		exec("python3.6 ".$int_script." --source=./tmp_parse --input=\"$path.in\""." > ". "./tmp_inter_out", $output, $return);
		$return_code = exec("cat \"$path.rc\"");
		if (intval($return_code) != 0) {
			if (intval($return_code != $return)) {
				writeResults($test_count, $path, $return, $return_code, "&#10008");
				$failed+=1;
			}
			else{
				writeResults($test_count, $path, $return, $return_code,"&#10004");
				$passed+=1;
			}
		}
		if (intval($return_code == 0)) {
			if (intval($return_code) == $return) {
				exec("diff -w ./tmp_inter_out ".$path.".out", $output, $difference);
				if (empty($output)) {
					writeResults($test_count, $path, $return, $return_code,"&#10004");
					$passed+=1;
				}
				else {
					writeResults($test_count, $path, $return, $return_code, "&#10008");
					$failed+=1;
				}
			}
			else {
				writeResults($test_count, $path, $return, $return_code, "&#10008");
				$failed+=1;
			}
		}
	}

	// Delete temp files
	unlink("./tmp_parse");
	unlink("./tmp_inter_out");
}

// Function which prints help
function printHelp() {
	echo("----------------------Napoveda---------------------\n");
  	echo("Skript bude slouzi pro automaticke testovani postupne\n");
  	echo("aplikace parse.php a interpret.py. Skript projde zadany\n");
  	echo("adresar s testy a vyuzije je pro automaticke otestovanie\n");
  	echo("spravne funkcnosti obou predchodzich programu vcetne\n");
  	echo("vygenerovani prehledneho souhrnu v HTML 5 do standardniho vystupu.\n");
  	exit(0);
}
?>