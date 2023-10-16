#!/bin/bash

NERLNET_PREFIX="[NERLNET_SCRIPT]"
INPUT_DATA_DIR="inputDataDir"

# arguments parsing 
# Thanks to https://github.com/matejak/argbash
Branch="master"
JobsNum=4
NerlWolf=OFF

help()
{
    echo "-------------------------------------" && echo "Nerlnet Build" && echo "-------------------------------------"
    echo "Usage:"
    echo "--p or --pull Warning! this uses checkout -f! and branch name checkout to branch $Branch and pull the latest"
    echo "--w or --wolf wolfram engine workers extension (nerlwolf)"
	echo "--j or --jobs number of jobs to cmake build"
    echo "--c or --clean remove build directory"
    exit 2
}

gitOperations()
{
    echo "$NERLNET_PREFIX Warning! git checkout -f is about to be executed"
    sleep 5
    echo "$NERLNET_PREFIX Interrupt is possible in the next 10 seconds"
    sleep 10
    git checkout -f $Branch
    git pull origin $Branch
    git submodule update --init --recursive
}

die()
{
	local _ret="${2:-1}"
	test "${_PRINT_HELP:-no}" = yes && print_help >&2
	echo "$1" >&2
	exit "${_ret}"
}


begins_with_short_option()
{
	local first_option all_short_options='hjp'
	first_option="${1:0:1}"
	test "$all_short_options" = "${all_short_options/$first_option/}" && return 1 || return 0
}

# THE DEFAULTS INITIALIZATION - OPTIONALS
clean_build_directory()
{
        echo "Are you sure that you want to remove build directory?"
        sleep 1
        echo "Intterupt this process is possible with ctrl+c"
        echo "Remove build directory in 10 seconds"
        sleep 10
        rm -rf build   
}

print_help()
{
	printf 'Usage: %s [-h|--help] [-c|--clean] [-j|--jobs <arg>] [-p|--pull <arg>]\n' "$0"
	printf '\t%s\n' "-j, --jobs: number of jobs (default: '4')"
	printf '\t%s\n' "-p, --pull: pull from branch (default: '4')"
	printf '\t%s\n' "-w, --wolf: wolfram engine extension build (default: 'off')"

}


parse_commandline()
{
	while test $# -gt 0
	do
		_key="$1"
		case "$_key" in
			-h|--help)
				help
				exit 0
				;;
			-h*)
				help
				exit 0
				;;
		        -c|--clean)
				clean_build_directory
				exit 0
				;;
			-c*)
				clean_build_directory
				exit 0
				;;
			-w|--wolf)
				test $# -lt 2 && die "Missing value for the optional argument '$_key'." 1
				NerlWolf="$2"
				shift
				;;
			--wolf=*)
				NerlWolf="${_key##--jobs=}"
				;;
			-w*)
				NerlWolf="${_key##-j}"
				;;
			-j|--jobs)
				test $# -lt 2 && die "Missing value for the optional argument '$_key'." 1
				JobsNum="$2"
				shift
				;;
			--jobs=*)
				JobsNum="${_key##--jobs=}"
				;;
			-j*)
				JobsNum="${_key##-j}"
				;;
			-p|--pull)
				test $# -lt 2 && die "Missing value for the optional argument '$_key'." 1
				Branch="$2"
                                gitOperations
				shift
				;;
			--pull=*)
				Branch="${_key##--pull=}"
                                gitOperations
				;;
			-p*)
				Branch="${_key##-p}"
                                gitOperations
				;;
			*)
				_PRINT_HELP=yes die "FATAL ERROR: Got an unexpected argument '$1'" 1
				;;
		esac
		shift
	done
}

parse_commandline "$@"
# end of args parsing

NERLNET_BUILD_PREFIX="[Nerlnet Build] "


OPTION="add_compile_definitions(EIGEN_MAX_ALIGN_BYTES=8)"
is_rasp="$(grep -c raspbian /etc/os-release)"
if [ $is_rasp -gt "0" ]; then 
        echo "$NERLNET_BUILD_PREFIX Detected raspberrypi => setting alignment to 8"
        sed -i "s/^.*#\(${OPTION}\)/\1/" CMakeLists.txt
else 
        echo "$NERLNET_BUILD_PREFIX Using default alignment"
        sed -i "s/^.*\(${OPTION}.*$\)/#\1/" CMakeLists.txt
fi

echo "$NERLNET_BUILD_PREFIX Building Nerlnet Library"
echo "$NERLNET_BUILD_PREFIX Cmake command of Nerlnet NIFPP"
set -e
cmake -S . -B build/release -DNERLWOLF=$NerlWolf -DCMAKE_BUILD_TYPE=RELEASE
cd build/release
echo "$NERLNET_BUILD_PREFIX Script CWD: $PWD"
echo "$NERLNET_BUILD_PREFIX Build Nerlnet"
echo "Jobs Number: $JobsNum"
make -j$JobsNum 
cd ../../
echo "$NERLNET_BUILD_PREFIX Script CWD: $PWD"
set +e

REBAR3_FILE=src_erl/rebar3/rebar3
REBAR3_SYMLINK=/usr/local/bin/rebar3

if [ -f "$REBAR3_FILE" ]; then
	echo "$NERLNET_BUILD_PREFIX rebar3 is installed, location: $REBAR3_FILE"
else 
	echo "$NERLNET_BUILD_PREFIX rebar3 Builder Start"
	cd src_erl/rebar3
	./bootstrap
	cd ../../	
	echo "$NERLNET_BUILD_PREFIX rebar3 is Built at $REBAR3_FILE"
fi

if [ -f "$REBAR3_SYMLINK" ]; then
        echo "$NERLNET_BUILD_PREFIX rebar3 Synlink exists in /usr/local/bin"
else
        echo "$NERLNET_BUILD_PREFIX $(tput setaf 1) Please run the following command from Nerlnet library root folder (or install rebar3 to usr/local/bin): $(tput sgr 0)"
        echo "$NERLNET_BUILD_PREFIX $(tput setaf 1) sudo ln -s `pwd`/src_erl/rebar3/rebar3 /usr/local/bin/rebar3 $(tput sgr 0)"
        echo "$NERLNET_BUILD_PREFIX "
fi

if [ -d "$INPUT_DATA_DIR" ]; then
        echo "$NERLNET_BUILD_PREFIX Input data directory of nerlnet is: $INPUT_DATA_DIR"
else
        echo "$NERLNET_BUILD_PREFIX Generating $INPUT_DATA_DIR"
        mkdir $INPUT_DATA_DIR
        echo "$NERLNET_BUILD_PREFIX Add input data to $INPUT_DATA_DIR"
fi
