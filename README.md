## Listing/description of files
* README.md: this file
* PROPOSAL.md: the original project proposal
* NOTES.md: some notes on vote-counting algorithms
* main.c: entry point
* io.c: votefile parser
* comms.h, opts.h: some global variables
* cli.c: command-line interface, mainly getopt
* fptp.c, irv.c, listpr.c, stv.c: the implementations of each counting algorithm
* votes.c: wrapper around the different algorithms
* votes.h: vote data structures
* example_votefile: a votefile with comments describing its structure
* generator.py: a votefile generator
* test: test framework
* timing: timing scripts and data
* Makefile: makefile!

## Building and running
To build the votecounter executable, simply run `make`. MPI must be loaded to
build the executable. votecounter has a number of options detailed in the help
flag, but some example commands are:
* `./votecounter VOTEFILE` to run an FPTP election according to the votefile and
  output to stdout
* `./votecounter VOTEFILE -p -o out.html` to do the same but output pretty html
  to `out.html`
* `./votecounter VOTEFILE -m list -s 80 -p -o out.html` to run an 80-seat list
  election with the same votefile
* `./votecounter VOTEFILE -m list -s 80 -p -o out.html -t 8` to do the same with
  an 8% threshold to get any seats
* `./votecounter VOTEFILE -m list -s 80 -dT -o out.html -t 8` to do the same
  with debug output and timing data enabled

## Testing
Simply run `make test`. MPI must be loaded to run the distributed tests.

## Timing
Run `nonpref/nonpref.sh` to time non-preferential methods (i.e. FPTP, list) and
`pref/pref.sh` for IRV and STV. You can also run e.g. `nonpref/strong/fptp.sh`
for FPTP strong scaling results. All the strong scaling datasets halve in size
  as the number of nodes double, and the weak datasets stay the same.

## Votefiles
A votefile is simply a record of votes, with some other optional information.
See `example_votefile` for further details. The example file is a valid input to
votecounter, but to generate new votefiles see the included `generator.py`. To
generate a new file with 10 candidates, with between 5 and 3000 votes each,
run `python generator.py out.vf -c 10 -m 5 -M 3000`. Note that the generator
uses python 2, as that is all that is installed on the cluster, so if your
machine defaults to python 3 you may have to run `python2 generator.py ...` or
similar. The `-p` flag generates every combination of full ranked votes, which
work as non-ranked votes too. To get a more realistic distribution without every
single possible combination of rankings, run the generator with a negative
minimum vote and all rankings with negative votes will be dropped.
