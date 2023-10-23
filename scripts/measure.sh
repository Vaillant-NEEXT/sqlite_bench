measure() (
  if [ -n "$O" ]; then
    $* &
  else
    $* &>/dev/null &
  fi
  pid="$!"
  trap "kill $pid" SIGINT
  o='%cpu,%mem,vsz,rss'
  printf '%s\n' "$o"
  i=0
  while s="$(ps --no-headers -o "$o" -p "$pid")"; do
    printf "$i $s\n"
    i=$(($i + 1))
    sleep "${T:-0.1}"
  done
)