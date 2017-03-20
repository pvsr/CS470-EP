import random

filename = "placeholder"
file = open(filename, 'w')
for i in range(0, 6):
    votes = random.randrange(1, 120)
    file.write('{0}:{1}\n'.format(i, votes))

file.close
