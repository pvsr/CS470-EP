import random
import sys

filename = sys.argv[1]
cands = int(sys.argv[2])
if len(sys.argv) > 3:
    minvote = int(sys.argv[3])
if minvote < 0:
    minvote = 10

if len(sys.argv) > 4:
    maxvote = int(sys.argv[4])
if maxvote < 0 or maxvote < minvote:
    maxvote = minvote + 1000

file = open(filename, 'w')
file.write('n {0}\n'.format(cands))
file.write('c\n')

for i in range(0, cands):
    votes = random.randrange(minvote, maxvote)
    file.write('{0} {1}\n'.format(votes, i))

file.write('!\n')
file.close
