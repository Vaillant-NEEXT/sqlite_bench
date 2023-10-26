# sqlite_bench

bulid with cmake

measure with scripts/measure.sh

query with time can be given via command line

data range need to change the parameter of calls

all the test: single write, bulk write, query , delete will be called with time measure

measure.sh will measure cpu , ram by testing 

IO usage can use sysstat tool suit, https://github.com/Vaillant-NEEXT/plsw-linux-image-base/actions/runs/6654497136 include a build for it 