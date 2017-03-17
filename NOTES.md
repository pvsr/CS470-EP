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
* Wright system: Droop quota, Gregory surplus allocation, iterative exclusion, optional bulk exclusion, ties decided randomly, TODO transfers to winners?
* Meek counting: recalculate quota each round
* Most common is Droop: floor(votes / seats + 1) + 1
* More proportional Hare: votes / seats
#### Example algorithm (from Wikipedia)
1. Compute quota
2. Assign votes
3. Declare winners
4. Transfer votes from winners
5. Repeat 3 and 4 until no one passes quota
6. Eliminate last place (or all candidates whose combined votes are less than
   last remaining candidate)
7. Transfer votes from losers
8. Repeat 3-7 until all seats are full
#### Surplus votes
* When winners pass the quota, their excess votes (beyond quota) are
  transferred. There are a number of ways to determine which votes are excess
* Randomly, e.g. every nth ballot for 1/n selected
* Gregory: Transfer fractional votes. Each vote has a fractional value, starting at 1. Every time the quota is passed and a vote is reallocated, its value is multiplied by (winner's total votes - quota / winner's total votes - exhausted votes)
#### Transfers to previous winners
* Hare and Wright: ignore winners and transfer to next valid preference
* Meek and Warren: TODO need more info
#### Allocation of losers' votes
* Can affect outcome
* Single transaction: Transfer all at once, check quota
* Segmented transactions: Transfer some, check quota, transfer more, etc.
  * Segment based on value
  * One at a time
* Iterative: Restart from the beginning without loser
##### Bulk exclusion/elimination
* Reduces number of steps in a count, good with iterative elimination
* Might be useful for optimization
* Determine breakpoint and eliminate all candidates below
* Calculated from running count of all votes from the bottom
* Quota breakpoint: Max running total less than half the quota
* Running breakpoint: Highest candidate's votes less than running total
* Applied breakpoint: Max running total less than difference of highest candidate's votes and quota
#### Ties
* In rounds after the first, previous round totals can be compared
* Otherwise, random

TODO finish this section https://en.wikipedia.org/wiki/Counting_single_transferable_votes
