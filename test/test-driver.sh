#!/usr/bin/env bash

DIFF_HEAD_SIZE=10

for var in TEST OUTPUT_FILE ERROR_FILE EXPECTED_OUTPUT_FILE \
	   EXPECTED_ERROR_FILE INPUT_DIR;
do
    if eval "test \"x\${${var}}\" = \"x\"";
    then
        echo 1>&2 "missing variable ${var}"
        exit 1
    fi
done

echo "${TEST} ${TEST_ARGS}"

if test "${IS_MEMCHECK_TEST}" = "yes";
then
    eval ${TEST} ${TEST_ARGS}
    exit $?
fi

if test "x${HASH_PROGRAM}" = "x";
then
    eval ${TEST} ${TEST_ARGS} > "${OUTPUT_FILE}" 2> "${ERROR_FILE}"
    retcode=$?
else
    TMPFILE="${OUTPUT_FILE}-$$"
    trap -- "rm ${TMPFILE}" EXIT
    eval ${TEST} ${TEST_ARGS} > "${TMPFILE}" 2> "${ERROR_FILE}"
    retcode=$?

    ${HASH_PROGRAM} "${TMPFILE}" | awk '{ print $1 }' > "${OUTPUT_FILE}"
fi


if test ${retcode} -eq 0;
then
    if ! test -f "${EXPECTED_OUTPUT_FILE}";
    then
        if test -s "${OUTPUT_FILE}";
        then
            echo 1>&2 "test generates an non-empty output in ${OUTPUT_FILE}."
            exit 1
        fi
    elif ! cmp -s "${OUTPUT_FILE}" "${EXPECTED_OUTPUT_FILE}";
	then
	    echo 1>&2 "Ouput of test differ from expected one. Here are the first diff lines:"
	    diff ${OUTPUT_FILE} ${EXPECTED_OUTPUT_FILE} | head -n ${DIFF_HEAD_SIZE} 1>&2
        echo 1>&2 "Get more details with:"
	    echo 1>&2 "diff ${OUTPUT_FILE} ${EXPECTED_OUTPUT_FILE}"

	    if test -f "${ERROR_FILE}";
	    then
	        echo 1>&2 "${ERROR_FILE} contains:"
	        cat 1>&2 "${ERROR_FILE}"
	    fi
	    exit 1
	fi
fi

if test -f "${EXPECTED_ERROR_FILE}";
then
    if ! cmp -s "${ERROR_FILE}" "${EXPECTED_ERROR_FILE}";
    then
        echo 1>&2 "Test produces unexpected error(s). Here are the first diff lines:"
        diff ${ERROR_FILE} ${EXPECTED_ERROR_FILE} | head -n ${DIFF_HEAD_SIZE} 1>&2
        echo 1>&2 "Get more details with:"
        echo 1>&2 "diff ${ERROR_FILE} ${EXPECTED_ERROR_FILE}"
        exit 1
    fi
    retcode=0
elif test -s "${ERROR_FILE}";
    then
        echo 1>&2 "Test produces unexpected error(s) in '${ERROR_FILE}'."
        echo 1>&2 "Here are the first lines:"
        head -n ${DIFF_HEAD_SIZE} ${ERROR_FILE} 1>&2
        exit 1
fi

test -s "${ERROR_FILE}" || rm "${ERROR_FILE}"

exit ${retcode}
