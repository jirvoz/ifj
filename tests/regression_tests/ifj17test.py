# -*- coding: utf-8 -*-

import filecmp
import subprocess
from pathlib import Path
import os

#Defined colors
GREEN = '\x1b[0;32m'
RED = '\x1b[0;31m'
YELLOW = '\x1b[0;33m'
MAGENTA = '\x1b[0;35m'
COLOR_END = '\x1b[0m'

#deifned list
error_code_list = []

#number of tests
path = './tests/test'
no_of_tests = len([f for f in os.listdir(path)
	if os.path.isfile(os.path.join(path, f))])

#function for convert int to string
def getNo(number):
    return '%02d' % number

print(MAGENTA +'Test_No  Status     Error code' + COLOR_END)

for i in range(no_of_tests):

	#removing old output files
	if (Path('./tests/output/test_output_' + getNo(i) + '.output').is_file()):
		os.remove('./tests/output/test_output_' + getNo(i) + '.output')

	#testing codes 
	test_input = open('./tests/test/test_' + getNo(i) + '.IFJcode', 'r')

	#output and error files
	my_output = open('./tests/output/test_output_' + getNo(i) + '.output', 'a+')
	my_err = open('./tests/output/error_' + getNo(i) + '.output', 'w+')

	#tmp files for code for interpreter and error code to compare
	tmp_ifj_code = './tests/output/!code_' + getNo(i) + '.IFJcode17'
	tmp_error_code = open('./tests/output/!err_' + getNo(i) + '.output', 'a+')

	#running program ifj with test input
	code = subprocess.call("./ifj", stdin = test_input, stdout = my_output, stderr = my_err, timeout = 5)

	#if was ifj without error then run interpreter
	if code == 0:
		with open(tmp_ifj_code, 'w') as ifj_code:
			with open('./tests/output/test_output_' + getNo(i) + '.output', 'r') as output:
				ifj_my_name = output.read()
				ifj_code.write(ifj_my_name)

		code = subprocess.call(["./ic17int", tmp_ifj_code], stdout = tmp_error_code , stderr = my_err, timeout = 5)
		os.remove(tmp_ifj_code)

	#adding last error code to the list
	error_code_list.append(code);
	
	tmp_error_code.write('\n' + getNo(code))

	#making test output file with IFJcode17, error code and input test file
	with open('./tests/output/error_' + getNo(i) + '.output', 'r') as error_file:
		error = error_file.read()

	with open('./tests/test/test_' + getNo(i) + '.IFJcode', 'r') as input_file:
		inputf = input_file.read()


	my_output.write('/' + 30*'*' + 'Error Code' + 30*'*' + '\\\n\n' + error)
	my_output.write('\n' + '/' + 30*'*' + 'Input Code' + 30*'*' + '\\\n\n' + inputf)

	os.remove('./tests/output/error_' + getNo(i) + '.output')

	#closing files
	test_input.close()
	my_output.close()
	my_err.close()
	tmp_error_code.close()

for j in range(no_of_tests):

	#path to compared files
	ex_cmp = './tests/expected/output_' + getNo(j) + '.output'
	my_cmp = './tests/output/!err_' + getNo(j) + '.output'

	#file compare and printing result of test
	if (filecmp.cmp(ex_cmp, my_cmp)):
		if (error_code_list[j] != 0):
			print('Test_' + getNo(j) + ': ' + GREEN + 'Success \t' + YELLOW + getNo(error_code_list[j]) + COLOR_END)
		else:
			print('Test_' + getNo(j) + ': ' + GREEN + 'Success \t' + GREEN + getNo(error_code_list[j]) + COLOR_END)
	else:
		print('Test_' + getNo(j) + ': ' + RED + 'Failed \t' + RED + getNo(error_code_list[j]) + COLOR_END)

	#removing tmp error file
	os.remove(my_cmp)
