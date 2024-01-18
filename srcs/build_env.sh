#!/bin/bash -x

platform='unknown'
unamestr=`uname`

if [ "$unamestr" == 'FreeBSD' ]; then
	platform='bsd'
elif [ "$unamestr" == 'Linux' ]; then
	platform='linux'
else
	echo "Unknown platform"
	exit 1;
fi

premake5="premake5"

if ! hash $premake5 2>/dev/null; then
	if [ "$platform" == 'bsd' ]; then
		pkg install -y $premake5
	else
		exit 1;
	fi
fi

if ! hash gmake 2>/dev/null; then
	if [ "$platform" == 'bsd' ]; then
		pkg install -y gmake
	else
		exit 1;
	fi
fi

if ! hash $premake5 2>/dev/null; then
	echo "$premake5 not found"
	exit 1;
fi

if ! hash gmake 2>/dev/null; then
	echo "gmake not found"
	exit 1;
fi

build_dir="build"
if ! [ -d $build_dir ]; then
	mkdir $build_dir
fi

#if [ -d $build_dir ]; then
#	rm -r $build_dir
#	mkdir $build_dir
#else
#	mkdir $build_dir
#fi

[ -z "$CPUS" ] && CPUS=`getconf NPROCESSORS_ONLN`

[ -z "$ARCH" ] && ARCH=`getconf LONG_BIT`

compiler="clang"
configuration="release"

if [ $1 ]; then
	compiler=$1
fi

if [ $2 ]; then
	configuration=$2
fi

$premake5 --os=$platform --cc=$compiler gmake
# cd $build_dir && gmake config="$configuration"_x$ARCH all -j$CPUS
cd $build_dir && gmake config="$configuration"_x32 all -j$CPUS
