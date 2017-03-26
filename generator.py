import random
import sys

argc = len(sys.argv)
if argc < 2:
    print "Usage: python generator.py FILENAME NUM_CANDS MIN_VOTES MAX_VOTES"
    print "  FILENAME is required"
    print "  NUM_CANDS defaults to 6"
    print "  MIN_VOTES defaults to 10"
    print "  MAX_VOTES defaults to MIN_VOTES + 1000"
    sys.exit(0)

filename = sys.argv[1]

if argc > 2:
    cands = int(sys.argv[2])
else:
    cands = 6

if argc > 3:
    minvote = int(sys.argv[3])
else:
    minvote = 10

if minvote < 0:
    minvote = 10

if argc > 4:
    maxvote = int(sys.argv[4])
else:
    maxvote = minvote + 1000

if maxvote < 0 or maxvote < minvote:
    maxvote = minvote + 1000

file = open(filename, 'w')
file.write('n {0}\n\n'.format(cands))
file.write('c\n')

for i in range(0, cands):
    votes = random.randrange(minvote, maxvote)
    file.write('{0} {1}\n'.format(votes, i))

file.write('!\n')
file.close
