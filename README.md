## Listing/description of files
* README.md: this file
* PROPOSAL.md: the original project proposal
* NOTES.md: some notes on vote-counting algorithms
* Makefile
* \*.{c,h}: source code
* example_votefile: a votefile with comments describing its structure
* generator.py: a votefile generator

## Building and running
To build the votecounter executable, simply run `make`. votecounter has a number
of options detailed in the help flag, but some example commands are:
* `./votecounter VOTEFILE` to run an FPTP election according to the votefile and
  output to stdout
* `./votecounter VOTEFILE -p -o out.html` to do the same but output pretty html
  to `out.html`
* `./votecounter VOTEFILE -v list -s 80 -p -o out.html` to run an 80-seat list
  election with the same votefile

## Votefiles
A votefile is simply a record of votes, with some other optional information.
See `example_votefile` for further details. The example file is a valid input to
votecounter, but to generate new votefiles see the included `generator.py`. To
generate a new file with 10 candidates, with between 5 and 3000 votes each,
run `python generator.py out.vf 10 5 3000`. Note that the generator uses python
2, as that is all that is installed on the cluster, so if your machine defaults
to python 3 you may have to run `python2 generator.py ...` or similar.
