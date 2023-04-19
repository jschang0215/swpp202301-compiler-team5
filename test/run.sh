if [ "$#" -ne 6 ]; then
    echo "Usage: LLVM_ROOT ALIVE2_ROOT BUILD_DIR TEST_DIR PASS_NAME FILECHECK"
    exit 1
fi

LLVM_ROOT=$1
ALIVE2_ROOT=$2
BUILD_DIR=$3
TEST_DIR=$4

# Pass name to be tested (ex. SimplePass)
PASS_NAME=$5

# Check file to be tested (ex. test/filecheck/SimplePass/simple1.ll)
FILECHECK=$6

LLVM_OPT=${LLVM_ROOT}/opt
ALIVE2_TV=${ALIVE2_ROOT}/build/alive-tv
FILECHECK_BIN=${LLVM_ROOT}/FileCheck

# Pass file (ex. build/libSimplePass.so)
PASS_FILE="${BUILD_DIR}/lib${PASS_NAME}.so"

# Output file name (ex. test/out/simple1.ll)
FILECHECK_NAME=$(basename ${FILECHECK})
OUTPUT_FILE="${TEST_DIR}/out/${PASS_NAME}/${FILECHECK_NAME}"

# Apply opt
${LLVM_OPT} -load-pass-plugin=${PASS_FILE} -passes="${PASS_NAME}" ${FILECHECK} -S -o ${OUTPUT_FILE}

# Filecheck
${FILECHECK_BIN} ${FILECHECK} < ${OUTPUT_FILE}

# Return nonzero when Filecheck fails
if [ "$?" -ne 0 ]; then
    exit 1
fi

# Alive2
ALIVE2_CORRECT_MSG="1 correct transformations"

# Return nonzero when Alive2 fails
if ! ${ALIVE2_TV} ${FILECHECK} ${OUTPUT_FILE} | grep -q "${ALIVE2_CORRECT_MSG}" > /dev/null; then
	exit 1
fi

# All test passed
exit 0
