if [ "$#" -ne 5 ]; then
    echo "Usage: LLVM_ROOT BUILD_DIR TEST_DIR PASS_NAME FILECHECK"
    exit 1
fi

LLVM_ROOT=$1
BUILD_DIR=$2
TEST_DIR=$3

# Pass name to be tested (ex. SimplePass)
PASS_NAME=$4

# Check file to be tested (ex. test/filecheck/SimplePass/simple1.ll)
FILECHECK=$5

LLVM_OPT=${LLVM_ROOT}/opt
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

if [ "$?" -eq 0 ]; then
    exit 0
else
    exit 1
fi