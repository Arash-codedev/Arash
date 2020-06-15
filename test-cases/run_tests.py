import os
import subprocess
import colorama
from colorama import Fore, Style
import sys


cwd = os.path.dirname(os.path.realpath(__file__))
os.chdir(cwd)
colorama.init(autoreset=True)
test_dirs = os.listdir(cwd)
failed_count = 0
passed_count = 0

for dir in test_dirs:
    if os.path.isdir(os.path.join(cwd, dir)):
        os.chdir(os.path.join(cwd, dir))
        print('testing', dir, '...', end =' ')
        result = subprocess.run(['python3', 'test.py', '-test'], stdout=subprocess.PIPE)
        if result.returncode:
            print(Fore.RED + '[Failed]' + Style.RESET_ALL)
            failed_count += 1
        else:
            print(Fore.GREEN + '[Passed]' + Style.RESET_ALL)
            passed_count += 1
        output_msg = result.stdout.strip().decode('utf-8')
        if output_msg != '':
            print(output_msg)

os.chdir(cwd)
print('')
print('Tests are completed.', failed_count, 'failed and', passed_count, 'passed.')
sys.exit(failed_count)
