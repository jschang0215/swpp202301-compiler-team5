if [ "$#" -ne 7 ]; then
    echo "Usage: LLVM_ROOT ALIVE2_ROOT INTERPRETER_ROOT BUILD_DIR TEST_DIR PASS_NAME FILECHECK"
    exit 1
fi

LLVM_ROOT=$1
ALIVE2_ROOT=$2
INTERPRETER_ROOT=$3
BUILD_DIR=$4
TEST_DIR=$5

# Pass name to be tested (ex. SimplePass)
PASS_NAME=$6

# Check file to be tested (ex. test/filecheck/SimplePass/simple1.ll)
FILECHECK=$7

LLVM_OPT=${LLVM_ROOT}/opt
ALIVE2_TV=${ALIVE2_ROOT}/build/alive-tv
FILECHECK_BIN=${LLVM_ROOT}/FileCheck
SWPP_COMPILER=${BUILD_DIR}/swpp-compiler
SWPP_INTERPRETER=${INTERPRETER_ROOT}/swpp-interpreter

# Pass file (ex. build/libSimplePass.so)
PASS_FILE="${BUILD_DIR}/lib${PASS_NAME}.so"

# Output file name (ex. test/out/simple1.ll)
FILECHECK_NAME=$(basename ${FILECHECK})
OUTPUT_FILE="${TEST_DIR}/out/${PASS_NAME}/${FILECHECK_NAME}"

# Create dir if not exist
mkdir -p ${TEST_DIR}/out
mkdir -p ${TEST_DIR}/out/${PASS_NAME}
mkdir -p ${TEST_DIR}/asm
mkdir -p ${TEST_DIR}/asm/${PASS_NAME}
mkdir -p ${TEST_DIR}/log
mkdir -p ${TEST_DIR}/log/${PASS_NAME}

# Apply opt
${LLVM_OPT} -load-pass-plugin=${PASS_FILE} -passes="${PASS_NAME}" ${FILECHECK} -S -o ${OUTPUT_FILE}

# Filecheck
${FILECHECK_BIN} ${FILECHECK} < ${OUTPUT_FILE}

# Return nonzero when Filecheck fails
if [ "$?" -ne 0 ]; then
    echo "Filecheck failed..."
    exit 1
fi

# Alive2
ALIVE2_CORRECT_MSG="0 incorrect transformations"

# Return nonzero when Alive2 fails
if ! ${ALIVE2_TV} ${FILECHECK} ${OUTPUT_FILE} | grep -q "${ALIVE2_CORRECT_MSG}" > /dev/null; then
    echo "Alive2 failed..."
	exit 1
fi

# Interpreter testing
OUTPUT_ASM="${TEST_DIR}/asm/${PASS_NAME}/${FILECHECK_NAME}.s"
${SWPP_COMPILER} ${FILECHECK}  ${OUTPUT_ASM}
${SWPP_INTERPRETER} ${OUTPUT_ASM}
# Move log files
FILECHECK_LOG_PATH=${TEST_DIR}/log/${PASS_NAME}/${FILECHECK_NAME%.*}
mkdir -p ${FILECHECK_LOG_PATH}
mv *.log ${FILECHECK_LOG_PATH}

# Return nonzero when Interpreter crashes
if [ "$?" -ne 0 ]; then
    echo "Interpreter crashed..."
    exit 1
fi

echo "====Interpreter Result===="
cat ${FILECHECK_LOG_PATH}/swpp-interpreter.log
echo "========Cost Result========"
cat ${FILECHECK_LOG_PATH}/swpp-interpreter-cost.log

# All test passed
exit 0
