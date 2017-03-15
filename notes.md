# Plurality
## FPTP
Simple
## Preferential
* Single winner, ranked choices
* Can rank any number of candidates
* Majority wins
* If no majority, remove last place and recount their ballots
# Proportional
## List
Simple
## STV
* Like preferential with multiple winners
* Candidate must pass a quota to be elected
* Elected candidate's excess votes are recounted
* If no one meets the quota, last place is eliminated
* Continues until every seat is one or seats remaining == candidates remaining
### Counting
* Meek counting: recalculate quota each round
* Most common is Droop: floor(votes / seats + 1) * 1
* More proportional Hare: votes / seats
* Example algorithm from Wikipedia
..1. Compute quota
..2. Assign votes
..3. Declare winners
..4. Transfer votes from winners
..5. Repeat 3 and 4 until no one passes quota
..6. Eliminate last place (or all candidates whose combined votes are less than
     last remaining candidate)
..7. Transfer votes from losers
..8. Repeat 3-7 until all seats are full
### Surplus votes
* When winners pass the quota, their excess votes (beyond quota) are
  transferred. There are a number of ways to determine which votes are excess
* Randomly, e.g. every nth ballot for 1/n selected
* Wright: 
TODO finish this section https://en.wikipedia.org/wiki/Counting_single_transferable_votes
