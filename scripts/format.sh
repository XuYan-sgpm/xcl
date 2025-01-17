echo $SHELL
project_dir="$(dirname "$(dirname "$0")")"
echo $project_dir

entries=(cmake include src tests)
source_exts=(h c cc cpp)

function is_empty_dir() {
  if [ "$(ls -A $1)" ]; then
    echo 0
  else
    echo 1
  fi
}

function file_type() {
  local file_name
  local file_ext
  file_name="$1"
  file_ext=${file_name#*.}
  local ret=0
  for item in ${source_exts[@]}; do
    if [ "$file_ext" = "$item" ]; then
      ret=1
      break
    elif [[ "$file_name" = "CMakeLists.txt" || "$file_ext" = "cmake" ]]; then
      ret=2
      break
    else
      ret=0
    fi
  done
  echo $ret
}

function format_file() {
  local file_name type cmd
  file_name=$(basename "$1")
  type=$(file_type "$file_name")
  if [ "$type" = "1" ]; then
    cmd="clang-format -style=file -i $1"
    echo $cmd
    clang-format -style=file -i "$1"
  elif [ "$type" = "2" ]; then
    cmd="cmake-format -c "$project_dir/scripts/.cmake-format.json" -i "$1""
    echo $cmd
    cmake-format -c "$project_dir/scripts/.cmake-format.json" -i "$1"
  else
    echo "can not format file $1"
  fi
}

function traverse() {
  if [ -f "$1" ]; then
    format_file $1
    return
  fi
  if [ "$(ls -A "$1")" ]; then
    for abs_path in "$1"/*; do
      traverse $abs_path
    done
  else
    echo "empty dir:$1"
  fi
}

function format_files() {
  for entry in "${entries[@]}"; do
    traverse "$project_dir/$entry"
  done
  format_file "$project_dir/CMakeLists.txt"
}

format_files
