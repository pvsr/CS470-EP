import random
import sys

filename = sys.argv[1]
cands = int(sys.argv[2])

file = open(filename, 'w')
file.write('n {0}\n'.format(cands))
file.write('c\n')

for i in range(0, cands):
    votes = random.randrange(10, 400)
    file.write('{0} {1}\n'.format(votes, i))

file.write('!\n')
file.close
