# Project Proposal
## Team
Peter Rice, Michael Wood
## Goal
We want to create a distributed, online (i.e. live-updating), vote counter that
can handle elections conducted using several different voting systems and
hundreds of millions of votes. Obviously, real-world vote-counting systems
already exist, but we want to test the limits of scalability and how large our
datasets can be, especially for complex voting systems.
## Relevance
In the real world, voting almost always takes place at many different polling
stations, which then report ballots to a central election committee or the like.
This maps very well to a distributed system where most nodes tally results and
report them up the tree to a master node. A parallel approach is practically
essential to count a number of votes on the scale of India or China, especially
for multi-round voting systems that potentially involve many recounted ballots.
## Methods
We will writing this vote counter from scratch in C, using MPI for communication
between distributed nodes, each of which may also use Pthreads or OpenMP to
speed up work that does not need to be distributed. We will also need at least a
minimal user interface to display changing results, for which we may use an
interface library.
## Possible Roadblocks
In order to be able to handle large datasets, we will need to find a good way to
distribute the data so that we don’t spend too much time on communication.
Adding votes to multi-round electoral systems like STV can cause a cascading
effect that might require, in effect, rerunning the entire election, so we need
to find ways to minimize recomputing when this happens.
## Mid-Project Deliverable
We plan to be most of the way to a working offline vote-counter for at least
single-winner first-past-the-post and proportional representation voting, as
well as a small program that generates datasets. At this point it should already
be able to handle large workloads and give correct results.
## Final Deliverable
Our final code will support live updates and several more voting systems
(probably at least single transferable vote and electoral-college-style
district-based FPTP) . In addition to this and the project report, we will also
have a number of datasets available that demonstrate live changes.
## Showcase
This project will be fairly straightforward to showcase using a live text
readout of results. If we have extra time, we can even create a GUI that shows
live changes.
