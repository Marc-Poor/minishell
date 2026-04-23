#!/bin/bash

# COLORS for pretty output
GREEN="\033[1;32m"
RED="\033[1;31m"
YELLOW="\033[1;33m"
RESET="\033[0m"

# path to your pipex executable
PIPEX=./pipex

# make sure infile exists
echo -e "apple\nbanana\napple\n" > infile

function run_test() {
    DESC=$1
    CMD=$2
    EXPECTED=$3
    echo -e "${YELLOW}→ $DESC${RESET}"
    eval $CMD > tmp_stdout 2> tmp_stderr
    ACTUAL=$(cat outfile 2>/dev/null)

    if [[ "$EXPECTED" == "VALGRIND" ]]; then
        if grep -q "All heap blocks were freed" tmp_stderr && grep -q "0 open" tmp_stderr; then
            echo -e "${GREEN}✅ Valgrind clean${RESET}"
        else
            echo -e "${RED}❌ Valgrind leak or open fd detected${RESET}"
        fi
    elif [[ "$ACTUAL" == "$EXPECTED" ]]; then
        echo -e "${GREEN}✅ OK${RESET}"
    else
        echo -e "${RED}❌ FAIL${RESET}"
        echo "Expected: '$EXPECTED'"
        echo "Got:      '$ACTUAL'"
    fi
    echo
}

# --------------------------------------------------------------------
# 1. Valid test
run_test "Basic valid: cat | wc -l" \
"$PIPEX infile 'cat' 'wc -l' outfile" \
"4"

# 2. Invalid first command
run_test "Invalid first command" \
"$PIPEX infile 'invalidcmd' 'wc -l' outfile" \
"0"

# 3. Invalid second command
run_test "Invalid second command" \
"$PIPEX infile 'cat' 'fakecommand' outfile" \
""

# 4. Missing infile
run_test "Missing infile" \
"$PIPEX nofile 'cat' 'wc -l' outfile" \
""

# 5. Inaccessible infile
chmod 000 infile
run_test "Permission denied infile" \
"$PIPEX infile 'cat' 'wc -l' outfile" \
""
chmod 644 infile

# 6. Script without exec permission
echo "echo hello" > myscript
chmod 000 myscript
run_test "Execve permission denied" \
"$PIPEX infile './myscript' 'wc -l' outfile" \
"0"
chmod 755 myscript

# 7. Memory stress test (Valgrind)
yes "memory test" | head -n 20000 > infile
run_test "Valgrind memory/fd test" \
"valgrind --track-fds=yes --leak-check=full $PIPEX infile 'grep test' 'wc -l' outfile 2>&1" \
"VALGRIND"

# 8. Empty infile
> infile
run_test "Empty infile" \
"$PIPEX infile 'cat' 'wc -l' outfile" \
"0"

# 9. Command that is a directory
mkdir -p testdir
run_test "Execve on directory" \
"$PIPEX infile 'testdir' 'wc -l' outfile" \
"0"

# Cleanup
rm -rf testdir infile outfile myscript tmp_stdout tmp_stderr
echo -e "${GREEN}All tests completed.${RESET}"
