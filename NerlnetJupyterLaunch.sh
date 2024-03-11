#!/bin/bash

NERLNET_LIB_DIR="/usr/local/lib/nerlnet-lib"
NERLNET_DIR=$NERLNET_LIB_DIR/NErlNet
SRC_PY_DIR=$NERLNET_DIR/src_py
API_SERVER_DIR=$SRC_PY_DIR/apiServer

# Args Default Values
_arg_venv="on"
JUPDIR="$PWD/JupyterLabDir"

# functions 

print()
{
   echo "[NERLNET JUPYTER LAUNCHER] $1"
}

set_env()
{
   cd $NERLNET_DIR
   source ./tests/set_env.sh
   pip3 install jupyterlab
   cd -
}

generate_set_jupyter_env()
{
    STARTUP_FILE_NAME=set_jupyter_env.py
    STARTUP_FILE_PATH=$JUPDIR/$STARTUP_FILE_NAME
    print "Generating $STARTUP_FILE_PATH"
    echo "import sys" > $STARTUP_FILE_PATH
    echo "sys.path.append(\"$API_SERVER_DIR\")" >> $STARTUP_FILE_PATH
}

generate_readme_md()
{
    README_FILE_NAME=README.md
    README_FILE_PATH=$JUPDIR/$README_FILE_NAME
    print "Generating $README_FILE_PATH"
    echo "This directory is intended to be used as a workspace for JupyterLab  " > $README_FILE_PATH
    echo "To use ApiServer from Jupyter Notebook, please run the following command in the first cell:  " >> $README_FILE_PATH
    echo "\`\`\`import set_jupyter_env\`\`\`  " >> $README_FILE_PATH
    echo "Run the first cell  " >> $README_FILE_PATH
    echo "Then you can use ApiServer in the next cells  " >> $README_FILE_PATH
}

# Generated online by https://argbash.io/generate
die()
{
	local _ret="${2:-1}"
	test "${_PRINT_HELP:-no}" = yes && print_help >&2
	echo "$1" >&2
	exit "${_ret}"
}


begins_with_short_option()
{
	local first_option all_short_options='dvh'
	first_option="${1:0:1}"
	test "$all_short_options" = "${all_short_options/$first_option/}" && return 1 || return 0
}



print_help()
{
	printf '%s\n' "The general script's help msg"
	printf 'Usage: %s [-d|--dir <arg>] [-v|--(no-)venv] [-h|--help]\n' "$0"
	printf '\t%s\n' "-d, --dir: directory that jupyter notebook runs from (no default)"
	printf '\t%s\n' "-v, --venv, --no-venv: automatically use nerlnet venv (on by default)"
	printf '\t%s\n' "-h, --help: Prints help"
}


parse_commandline()
{
	while test $# -gt 0
	do
		_key="$1"
		case "$_key" in
			-d|--dir)
				test $# -lt 2 && die "Missing value for the optional argument '$_key'." 1
				JUPDIR="$2"
				shift
				;;
			--dir=*)
				JUPDIR="${_key##--dir=}"
				;;
			-d*)
				JUPDIR="${_key##-d}"
				;;
			-v|--no-venv|--venv)
				_arg_venv="on"
				test "${1:0:5}" = "--no-" && _arg_venv="off"
				;;
			-v*)
				_arg_venv="on"
				_next="${_key##-v}"
				if test -n "$_next" -a "$_next" != "$_key"
				then
					{ begins_with_short_option "$_next" && shift && set -- "-v" "-${_next}" "$@"; } || die "The short option '$_key' can't be decomposed to ${_key:0:2} and -${_key:2}, because ${_key:0:2} doesn't accept value and '-${_key:2:1}' doesn't correspond to a short option."
				fi
				;;
			-h|--help)
				print_help
				exit 0
				;;
			-h*)
				print_help
				exit 0
				;;
			*)
				_PRINT_HELP=yes die "FATAL ERROR: Got an unexpected argument '$1'" 1
				;;
		esac
		shift
	done
}

parse_commandline "$@"

# OTHER STUFF GENERATED BY Argbash

### END OF CODE GENERATED BY Argbash (sortof) ### ])

if [ "$_arg_venv" = "on" ]; then
    print "Setting up nerlnet venv"
    set_env
else
    print "This is user responsibility to set up the environment"
    print "If you want to use nerlnet venv, please use -v or --venv option"
fi

mkdir -p $JUPDIR
cd $JUPDIR

generate_set_jupyter_env
generate_readme_md

# TODO add networkx and pygraphviz installations! 

jupyter-lab
