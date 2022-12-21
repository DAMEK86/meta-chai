inherit image_types

IMAGE_TYPEDEP:ubi = "squashfs-lz4"
do_image_ubi[depends] += "mtd-utils-native:do_populate_sysroot"
do_image_ubi[depends] += "squashfs-tools-native:do_populate_sysroot"


IMAGE_TYPEDEP:squashfs-lz4-ubi = "squashfs-lz4"
do_image_squashfs-lz4-ubi[depends] += "mtd-utils-native:do_populate_sysroot"
do_image_squashfs-lz4-ubi[depends] += "squashfs-tools-native:do_populate_sysroot"

IMAGE_CMD:squashfs-lz4-ubi () {
	squashfsubi_mkfs "${MKUBIFS_ARGS}" "${UBINIZE_ARGS}"
}
#
# Overwrite multiubi_mkfs
# to create a multivolume ubifs following the schema:
#
#+--------------------------+
#|    UBI                   |
#| +----------------------+ |
#| | fitImage A (UBIFS)   | |
#| +----------------------+ |
#| | fitImage B (UBIFS)   | |
#| +----------------------+ |
#| |   rootfs A (UBIFS)   | |
#| +----------------------+ |
#| |   rootfs B (UBIFS)   | |
#| +----------------------+ |
#| |       data (UBIFS)   | |
#| +----------------------+ |
#+--------------------------+

squashfsubi_mkfs() {
	bbwarn "squashfsubi"
	local mkubifs_args="$1"
	local ubinize_args="$2"
	CFG_NAME=ubinize-${IMAGE_NAME}-squashfs-lz4-ubi.cfg

	# Added prompt error message for ubi and ubifs image creation.
	if [ -z "$mkubifs_args"] || [ -z "$ubinize_args" ]; then
		bbfatal "MKUBIFS_ARGS and UBINIZE_ARGS have to be set, see http://www.linux-mtd.infradead.org/faq/ubifs.html for details"
	fi

	echo \[ubifs\] > ${CFG_NAME}
	echo mode=ubi >> ${CFG_NAME}
	echo image=${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.squashfs-lz4 >> ${CFG_NAME}
	echo vol_id=0 >> ${CFG_NAME}
	echo vol_type=dynamic >> ${CFG_NAME}
	echo vol_name=${UBI_VOLNAME} >> ${CFG_NAME}
	echo vol_flags=autoresize >> ${CFG_NAME}
	# normally we shouldn't need to create the squashfs image ourselves,
	# because we have a dependency declared (IMAGE_TYPEDEP)
	# But, if this file is modified, the dependency is _not_ rebuild,
	# so we have to do this ourselves.
	if [ ! -e ${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.squashfs-lz4 ]; then
  		bbwarn "${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.squashfs-lz4 does not exist. Creating."
		${IMAGE_CMD_squashfs-lz4}
 	fi

	ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.squashfs-lz4-ubi ${ubinize_args} ${CFG_NAME}

 	# Cleanup cfg file
 	mv ${CFG_NAME} ${IMGDEPLOYDIR}/
}

multiubi_mkfs() {
	bbwarn "multiubi"
	local mkubifs_args="$1"
	local ubinize_args="$2"

 # Added prompt error message for ubi and ubifs image creation.
 if [ -z "$mkubifs_args" ] || [ -z "$ubinize_args" ]; then
     bbfatal "MKUBIFS_ARGS and UBINIZE_ARGS have to be set, see http://www.linux-mtd.infradead.org/faq/ubifs.html for details"
 fi

	if [ -z "$3" ]; then
		local vname=""
	else
		local vname="_$3"
	fi

	CFG_NAME=ubinize-${IMAGE_NAME}-ubi.cfg
	echo -n > ${CFG_NAME}

	echo \[kernel_a\] >> ${CFG_NAME}
	echo mode=ubi >> ${CFG_NAME}
	echo image=${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE} >> ${CFG_NAME}
	echo vol_id=0 >> ${CFG_NAME}
	echo vol_alignment=1 >> ${CFG_NAME}
	echo vol_type=dynamic >> ${CFG_NAME}
	echo vol_size=10MiB >> ${CFG_NAME}
	echo vol_name=kernel_a >> ${CFG_NAME}

	echo \[kernel_b\] >> ${CFG_NAME}
	echo mode=ubi >> ${CFG_NAME}
	echo image=${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE} >> ${CFG_NAME}
	echo vol_id=1 >> ${CFG_NAME}
	echo vol_alignment=1 >> ${CFG_NAME}
	echo vol_type=dynamic >> ${CFG_NAME}
	echo vol_size=10MiB >> ${CFG_NAME}
	echo vol_name=kernel_b >> ${CFG_NAME}

	echo \[rootfs_a\] >> ${CFG_NAME}
	echo mode=ubi >> ${CFG_NAME}
	echo image=${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.rootfs.ubifs >> ${CFG_NAME}
	echo vol_id=2 >> ${CFG_NAME}
	echo vol_alignment=1 >> ${CFG_NAME}
	echo vol_type=dynamic >> ${CFG_NAME}
	echo vol_size=30MiB >> ${CFG_NAME}
	echo vol_name=${UBI_VOLNAME}_a >> ${CFG_NAME}

	echo \[rootfs_b\] >> ${CFG_NAME}
	echo mode=ubi >> ${CFG_NAME}
	echo image=${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.rootfs.ubifs >> ${CFG_NAME}
	echo vol_id=3 >> ${CFG_NAME}
	echo vol_alignment=1 >> ${CFG_NAME}
	echo vol_type=dynamic >> ${CFG_NAME}
	echo vol_size=30MiB >> ${CFG_NAME}
	echo vol_name=${UBI_VOLNAME}_b >> ${CFG_NAME}
	# normally we shouldn't need to create the squashfs image ourselves,
	# because we have a dependency declared (IMAGE_TYPEDEP)
	# But, if this file is modified, the dependency is _not_ rebuild,
	# so we have to do this ourselves.
	if [ ! -e ${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.squashfs-lz4 ]; then
  		bbwarn "${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.squashfs-lz4 does not exist. Creating."
		${IMAGE_CMD_squashfs-lz4}
 	fi

	echo \[data\] >> ${CFG_NAME}
	echo mode=ubi >> ${CFG_NAME}
	echo image=${DEPLOY_DIR_IMAGE}/empty.ubifs >> ${CFG_NAME}
	echo vol_id=4 >> ${CFG_NAME}
	echo vol_alignment=1 >> ${CFG_NAME}
	echo vol_type=dynamic >> ${CFG_NAME}
	echo vol_name=data >> ${CFG_NAME}
	echo vol_size=30MiB >> ${CFG_NAME}
	echo vol_flags=autoresize >> ${CFG_NAME}

	rm -rf ${IMGDEPLOYDIR}/empty/*
	mkdir -p ${IMGDEPLOYDIR}/empty

	bbwarn cat ${IMGDEPLOYDIR}/${CFG_NAME}

	mkfs.ubifs -r ${IMGDEPLOYDIR}/empty -o ${DEPLOY_DIR_IMAGE}/empty.ubifs ${mkubifs_args}

	mkfs.ubifs -r ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.rootfs.ubifs ${mkubifs_args}
	ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}${vname}.rootfs.ubi ${ubinize_args} ${CFG_NAME}

	# Cleanup cfg file
	mv ${CFG_NAME} ${IMGDEPLOYDIR}/

	# Create own symlinks for 'named' volumes
	if [ -n "$vname" ]; then
		cd ${IMGDEPLOYDIR}
		if [ -e ${IMAGE_NAME}${vname}${IMAGE_NAME_SUFFIX}.ubifs ]; then
			ln -sf ${IMAGE_NAME}${vname}${IMAGE_NAME_SUFFIX}.ubifs \
			${IMAGE_LINK_NAME}${vname}.ubifs
		fi
		if [ -e ${IMAGE_NAME}${vname}${IMAGE_NAME_SUFFIX}.ubi ]; then
			ln -sf ${IMAGE_NAME}${vname}${IMAGE_NAME_SUFFIX}.ubi \
			${IMAGE_LINK_NAME}${vname}.ubi
		fi
		cd -
	fi
}
