import random
import sys
import argparse

parser = argparse.ArgumentParser(description="generate a random votefile")
parser.add_argument("-n, --num_cands", metavar="N", type=int,
                    dest="cands", default=6, help="number of candidates, default 6")
parser.add_argument("-m, --min_votes", metavar="N", type=int,
                    dest="min", default=0, help="minimum number of votes, default 0")
parser.add_argument("-M, --max_votes", metavar="N", type=int,
                    dest="max", default=-1, help="maximum number of votes, default min_votes + 1000")
parser.add_argument("-p, --preferential", dest="pref", action="store_true",
                    help="whether to generate full preferential votes")
parser.add_argument("filename", metavar="FILE", type=str,
                    help="output file")
args = parser.parse_args()

if args.max == -1:
    args.max = args.min + 1000

file = open(args.filename, 'w')
file.write('n {0}\n\n'.format(args.cands))
file.write('c\n')

if args.pref:
    print "uhh"
else:
    for i in range(0, args.cands):
        votes = random.randrange(args.min, args.max)
        file.write('{0} {1}\n'.format(votes, i))

file.write('!\n')
file.close
