#!/bin/sh

usage() {
	echo
	echo usage:
	echo ./make.sh [release/debug] [clean]
}

if [ $# -gt 2 ]
then
	echo error: amount of arguments is $#, greater than 2.
	usage
	exit 1
fi

build_type=$1
clean_flag=$2

if [ -z "${build_type}" ]
then
	echo build type default set to debug.
	build_type=debug
fi

echo hint: BUILD TYPE is ${build_type}
echo hint: clean flag is ${clean_flag}

if [ "${build_type}" != debug ] && [ "${build_type}" != release ]
then
	echo error: first argument is build type, valid value should be release or debug.
	usage
	exit 2
fi
if [ "${clean_flag}" != "" ] && [ "${clean_flag}" != clean ]
then
	echo error: second argument indicates clean or not, valid value should be clean or empty.
	usage
	exit 2
fi

make HOST=linux BUILD_TYPE=${build_type} ${clean_flag}
