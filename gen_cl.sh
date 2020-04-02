filename=p4_changelist.h

GIT_COMMIT_ID="$(git rev-parse HEAD)"
test $? -ne 0 &&  GIT_COMMIT_ID=""
test -n "${GIT_COMMIT_ID}" && echo ${GIT_COMMIT_ID} > ${filename} || echo "NoRevInf" > ${filename}

version="$(cat ${filename})"
sed -i "s/#define\ \+KERN_CL\ \+.*/#define\ KERN_CL\ \"KERN-KERNEL-3.$(date +%s).$(perl get_last_commit.pl).$(date +%Y%m%d%H)\"/g" include/linux/release_version.h
