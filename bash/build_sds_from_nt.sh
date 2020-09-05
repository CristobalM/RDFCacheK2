#!/bin/bash

set -e

# Arguments

# ${1} executor, e.g. docker
# ${2} dataset in .nt format
# ${3} output folder
# ${4} max thread count
_TMP_FOLDER=_tmp_sds_builder
PATH_ONE=$(eval realpath ${1})
cp $PATH_ONE ${_TMP_FOLDER}/
FILENAME_EX=$(eval basename -- "${PATH_ONE}")
NEXT_EX_PATH=$(eval realpath ${_TMP_FOLDER}/${FILENAME_EX})
EXECUTOR="/bin/bash ${NEXT_EX_PATH}"
DATASET_INPUT=${2}
REALP_DATASET=$(eval realpath -- ${DATASET_INPUT})
DATASET_FNAME=$(eval basename -- "${REALP_DATASET}")
DATASET_DIR=$(eval dirname "${REALP_DATASET}")
OUTPUT_FOLDER=$(eval realpath ${3})
MAX_THREAD_COUNT=${4}
WORKSPACE=${5}

if [ -z "${WORKSPACE}" ]; then
    WORKSPACE=${_TMP_FOLDER}
fi

mv ${REALP_DATASET} ${_TMP_FOLDER}
DATASET=${DATASET_FNAME}





echo $DATASET
echo $EXECUTOR


mkdir -p ${_TMP_FOLDER}
cd ${_TMP_FOLDER}

declare -a entities=("subjects" "predicates" "objects")

# Generates:
# rawb64.subjects.txt
# rawb64.predicates.txt
# rawb64.objects.txt
to_eval="${EXECUTOR} split_nt_triples -f ${WORKSPACE}/${DATASET} -o ${WORKSPACE}/rawb64 -b"
echo "TO_EVAL: $to_eval"
eval "$to_eval"

function remove_duplicates() {
    entity=${1} # entity
    pwd
    sort -u --parallel=${MAX_THREAD_COUNT} -S 85% rawb64.${entity}.txt -o rawb64.${entity}.sorted-u.txt
    rm -rf rawb64.${entity}.txt
    mv rawb64.${entity}.sorted-u.txt rawb64.${entity}.txt
}

echo "Removing duplicates..."
for entity in "${entities[@]}"; do
    #sort -u --parallel=${4} -S 85% rawb64.${entity}.txt -o rawb64.${entity}.sorted-u.txt
    #rm rawb64.${entity}.txt
    #mv rawb64.${entity}.sorted-u.txt rawb64.${entity}.txt
    remove_duplicates ${entity}
done
echo "Done Removing duplicates."


# Create String Dictionaries
subj_sd="eval ${EXECUTOR} sd_from_fileset_readfull -f ${WORKSPACE}/rawb64.subjects.txt -o ${WORKSPACE}/subjects.sd -tPFC -B128 -b"
pred_sd="eval ${EXECUTOR} sd_from_fileset_readfull -f ${WORKSPACE}/rawb64.predicates.txt -o ${WORKSPACE}/predicates.sd -tPFC -B8 -b"
obj_sd="eval ${EXECUTOR} sd_from_fileset_readfull -f ${WORKSPACE}/rawb64.objects.txt -o ${WORKSPACE}/objects.sd -tHRPDACBlocks -c5000000 -b -p${MAX_THREAD_COUNT}"

eval "$subj_sd"
eval "$pred_sd"
eval "$obj_sd"

# Validate String Dictionaries
current=1
errors=1
while [[ $errors != [0] ]]; do
    echo "Running validate_sd_from_nt (${current})"
    ((current += 1))
    ${EXECUTOR} validate_sd_from_nt -s ${WORKSPACE}/subjects.sd -p ${WORKSPACE}/predicates.sd -o ${WORKSPACE}/objects.sd -n ${WORKSPACE}/${DATASET} -e -mLOG
    errors=0
    for entity in "${entities[@]}"; do
        LOGF=_validate_sd_from_nt.${entity}.not-found.log
        if [ -f ${LOGF} ] && [ ! -s ${LOGF} ]; then # if not empty
            ((errors += 1))
            echo "Some ${entity} were not found in the string dictionary, now adding them..."
            cat ${LOGF} >> rawb64.${entity}.txt
            remove_duplicates ${entity}
            if [ $entity == "subjects" ]; then
                eval "$subj_sd"
            elif [ $entity == "predicates" ]; then
                eval "$pred_sd"
            elif [ $entity == "objects" ]; then
                eval "$obj_sd"
            fi
        fi
    done
done
echo "Validation done!"


for entity in "${entities[@]}"; do
    mv ${entity}.sd ${OUTPUT_FOLDER}/${entity}.sd
done


mv $DATASET ${DATASET_DIR}


cd ..
rm -rf ${_TMP_FOLDER}
